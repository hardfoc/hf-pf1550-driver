/**
 * @file pf1550_esp32c6_provision.cpp
 * @brief External ESP32-C6 PMIC provisioning for pre-MCU board bring-up.
 *
 * Use when the PF1550 I2C bus is exposed on test pads and the STM32 (or other
 * host MCU) is held in reset or not yet powered. Typical workflow:
 *
 * 1. Power the module from USB or bench supply (PMIC may be in OTP defaults).
 * 2. Wire ESP32-C6 SDA/SCL to PMIC I2C @ 0x08 (and optional strap GPIOs).
 * 3. Flash this app and monitor serial @ 115200.
 * 4. Confirm self-test PASS and rail setpoints in the log.
 * 5. Remove ESP32, release MCU reset, flash manufacturing firmware.
 *
 * @see docs/esp32-provisioning.md
 */

#include "esp32_pf1550_bus.hpp"

#include "pf1550.hpp"
#include "pf1550_diagnostics.hpp"
#include "pf1550_profiles.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "pf1550_prov";

namespace {

void LogRailChecklist(const pf1550::DiagnosticSnapshot& s) {
  ESP_LOGI(TAG, "--- Bench checklist (measure at module) ---");
  ESP_LOGI(TAG, "  VSYS:        4.1-4.5 V (USB) / 4.5 V (5 V input)");
  ESP_LOGI(TAG, "  SW1 +3V1SW:  ~3000 mV (set=%u mV) — LDO inputs",
           static_cast<unsigned>(s.rails[static_cast<size_t>(pf1550::RailId::Sw1)].set_voltage_mv));
  ESP_LOGI(TAG, "  LDO1:        ~1000 mV");
  ESP_LOGI(TAG, "  LDO2:        ~1800 mV");
  ESP_LOGI(TAG, "  LDO3:        ~1200 mV");
  ESP_LOGI(TAG, "  SW3 VCORE:   ~3100 mV (set=%u mV)",
           static_cast<unsigned>(s.rails[static_cast<size_t>(pf1550::RailId::Sw3)].set_voltage_mv));
  ESP_LOGI(TAG, "  SW2 +3V3:    3300 mV ±3%% (set=%u mV) — carrier / JTAG VTref",
           static_cast<unsigned>(s.rails[static_cast<size_t>(pf1550::RailId::Sw2)].set_voltage_mv));
  ESP_LOGI(TAG, "  PMIC state:  %s (expect Run)", pf1550::PmicStateName(s.state));
}

bool ApplySelectedProfile(pf1550::PF1550<Esp32Pf1550Bus>& pmic) {
#if defined(CONFIG_PF1550_PROFILE_CARRIER) && CONFIG_PF1550_PROFILE_CARRIER
  ESP_LOGI(TAG, "Applying profile '%s'", pf1550::profiles::kPortentaH7CarrierName);
  return pmic.ApplyPortentaH7CarrierProfile();
#else
  ESP_LOGI(TAG, "Applying profile '%s'", pf1550::profiles::kPortentaH7DefaultName);
  return pmic.ApplyPortentaH7DefaultProfile();
#endif
}

}  // namespace

extern "C" void app_main(void) {
  ESP_LOGI(TAG, "PF1550 external provision — hf-pf1550 v%s",
           pf1550::PF1550<Esp32Pf1550Bus>::GetDriverVersion());

  auto bus = CreateEsp32Pf1550Bus();
  if (!bus) {
    ESP_LOGE(TAG, "I2C init failed — check SDA/SCL wiring");
    return;
  }

  pf1550::PF1550<Esp32Pf1550Bus> pmic(bus.get());

  if (!pmic.EnsureInitialized()) {
    ESP_LOGE(TAG, "PF1550 not detected at 0x%02X — verify I2C tap and power",
             pf1550::kDefaultI2cAddress);
    ESP_LOGI(TAG, "Tip: MCU reset is not required; PMIC I2C must be reachable");
    return;
  }

  // Straps: RUN mode + USB rails (no-op if Kconfig straps disabled)
  (void)pmic.SetPowerMode(pf1550::PowerMode::Run);
  (void)pmic.SetUsbRails(true, true);
  vTaskDelay(pdMS_TO_TICKS(10));

  if (!ApplySelectedProfile(pmic)) {
    ESP_LOGE(TAG, "Profile apply failed (flags=0x%04X)", pmic.GetErrorFlags());
    return;
  }
  ESP_LOGI(TAG, "Profile applied — allow rails to settle");
  vTaskDelay(pdMS_TO_TICKS(50));

  (void)pmic.ClearLatchedFaults();

  pf1550::SelfTestResult st{};
  const bool ok = pmic.RunPowerSelfTest(st);
  ESP_LOGI(TAG, "Self-test: %s device_id=%d state_run=%d rails=%d worst=%s",
           ok ? "PASS" : "FAIL",
           static_cast<int>(st.device_id_ok),
           static_cast<int>(st.state_run),
           static_cast<int>(st.all_rails_enabled),
           pf1550::FaultSeverityName(st.worst_severity));

  LogRailChecklist(st.snapshot);

  if (!ok) {
    ESP_LOGW(TAG, "Self-test did not pass — check faults and cold power cycle if needed");
  } else {
    ESP_LOGI(TAG, "Provisioning complete — safe to remove ESP32 and bring up host MCU");
  }

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(10000));
    pf1550::DiagnosticSnapshot snap{};
    if (pmic.ReadDiagnosticSnapshot(snap)) {
      ESP_LOGI(TAG, "Monitor: state=%s SW1=%u mV SW2=%u mV SW3=%u mV faults=0x%08lX",
               pf1550::PmicStateName(snap.state),
               static_cast<unsigned>(snap.rails[static_cast<size_t>(pf1550::RailId::Sw1)].set_voltage_mv),
               static_cast<unsigned>(snap.rails[static_cast<size_t>(pf1550::RailId::Sw2)].set_voltage_mv),
               static_cast<unsigned>(snap.rails[static_cast<size_t>(pf1550::RailId::Sw3)].set_voltage_mv),
               static_cast<unsigned long>(snap.faults.value));
    }
    (void)pmic.ClearLatchedFaults();
  }
}
