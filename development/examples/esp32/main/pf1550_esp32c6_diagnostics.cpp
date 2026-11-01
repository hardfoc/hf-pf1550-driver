/**
 * @file pf1550_esp32c6_diagnostics.cpp
 * @brief ESP32-C6 PF1550 diagnostic-snapshot + self-test example.
 *
 * Reads a full @ref pf1550::DiagnosticSnapshot every 5 seconds and logs:
 *   - DEVICE_ID, OTP_FLAVOR, SILICON_REV,
 *   - PMIC state machine (`Wait / Run / Standby / Sleep / RegsDisable`),
 *   - per-rail enable + setpoint mV,
 *   - latched fault flags + worst-case severity (Portenta H7 wiring),
 *   - charger sub-state + decoded VBUS input current limit.
 *
 * Use this as a template when porting the driver to a custom board.
 */

#include "esp32_pf1550_bus.hpp"

#include "pf1550.hpp"
#include "pf1550_diagnostics.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "pf1550_diag";

namespace {

void LogSnapshot(const pf1550::DiagnosticSnapshot& s) {
  ESP_LOGI(TAG, "id=0x%02X otp=0x%02X rev=0x%02X state=%s charger=%s VBUS_LIM=%u mA",
           s.device_id, s.otp_flavor, s.silicon_rev,
           pf1550::PmicStateName(s.state),
           pf1550::ChargerStateName(s.charger),
           static_cast<unsigned>(s.vbus_in_limit_ma));
  for (size_t i = 0; i < static_cast<size_t>(pf1550::RailId::COUNT); ++i) {
    const auto& r = s.rails[i];
    ESP_LOGI(TAG, "  %-5s ctrl=0x%02X V=0x%02X (%u mV) RUN=%d STBY=%d latched=%d live=%d",
             pf1550::RailName(static_cast<pf1550::RailId>(i)),
             r.ctrl_reg, r.volt_reg,
             static_cast<unsigned>(r.set_voltage_mv),
             static_cast<int>(r.enabled_run),
             static_cast<int>(r.enabled_stby),
             static_cast<int>(r.stat_latched),
             static_cast<int>(r.sense_live));
  }
  if (s.faults.Any()) {
    ESP_LOGW(TAG, "Latched faults=0x%08lX worst=%s",
             static_cast<unsigned long>(s.faults.value),
             pf1550::FaultSeverityName(pf1550::WorstSeverityPortentaH7(s.faults)));
  }
}

}  // namespace

extern "C" void app_main(void) {
  auto bus = CreateEsp32Pf1550Bus();
  if (!bus) {
    ESP_LOGE(TAG, "I2C init failed");
    return;
  }

  pf1550::PF1550<Esp32Pf1550Bus> pmic(bus.get());

  if (!pmic.EnsureInitialized()) {
    ESP_LOGW(TAG, "PF1550 not detected — wire SDA=GPIO4 SCL=GPIO5 to a PF1550 board");
    return;
  }

  pf1550::SelfTestResult st{};
  const bool ok = pmic.RunPowerSelfTest(st);
  ESP_LOGI(TAG, "Self-test: ran=%d ok=%d device_id_ok=%d state_run=%d rails_en=%d worst=%s",
           static_cast<int>(st.ran), static_cast<int>(ok),
           static_cast<int>(st.device_id_ok),
           static_cast<int>(st.state_run),
           static_cast<int>(st.all_rails_enabled),
           pf1550::FaultSeverityName(st.worst_severity));
  LogSnapshot(st.snapshot);

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    pf1550::DiagnosticSnapshot snap{};
    if (pmic.ReadDiagnosticSnapshot(snap)) {
      LogSnapshot(snap);
    } else {
      ESP_LOGW(TAG, "Snapshot read failed (driver flags=0x%04X)", pmic.GetErrorFlags());
    }
    (void)pmic.ClearLatchedFaults();
  }
}
