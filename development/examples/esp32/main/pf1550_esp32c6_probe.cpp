/**
 * @file pf1550_esp32c6_probe.cpp
 * @brief ESP32-C6 I2C probe for NXP PF1550 @ 0x08
 */

#include "esp32_pf1550_bus.hpp"

#include "pf1550.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "pf1550_probe";

extern "C" void app_main(void) {
  ESP_LOGI(TAG, "hf-pf1550 driver v%s", pf1550::PF1550<Esp32Pf1550Bus>::GetDriverVersion());

  auto bus = CreateEsp32Pf1550Bus();
  if (!bus) {
    ESP_LOGE(TAG, "Failed to init I2C bus");
    return;
  }

  pf1550::PF1550<Esp32Pf1550Bus> pmic(bus.get(), pf1550::kDefaultI2cAddress);

  uint8_t device_id = 0;
  if (!pmic.ReadDeviceId(device_id)) {
    ESP_LOGW(TAG, "No PF1550 at 0x%02X (expected on Portenta/Synapse boards only)",
             pf1550::kDefaultI2cAddress);
    ESP_LOGW(TAG, "Wire SDA=GPIO4 SCL=GPIO5 to a PF1550 eval if testing hardware");
    return;
  }

  ESP_LOGI(TAG, "DEVICE_ID=0x%02X (expected 0x%02X)", device_id, pf1550::kExpectedDeviceId);

  if (!pmic.VerifyDevice()) {
    ESP_LOGW(TAG, "Unexpected device ID — not PF1550 family");
    return;
  }

  uint8_t status = 0;
  if (pmic.ReadPmicStatus(status)) {
    ESP_LOGI(TAG, "PMIC_STATUS (0x67)=0x%02X", status);
  }

#if defined(CONFIG_PF1550_APPLY_PORTENTA_PROFILE) && CONFIG_PF1550_APPLY_PORTENTA_PROFILE
  ESP_LOGW(TAG, "Applying Portenta profile — use only on unprogrammed / lab PMIC");
#if defined(CONFIG_PF1550_PROFILE_CARRIER) && CONFIG_PF1550_PROFILE_CARRIER
  if (pmic.ApplyPortentaH7CarrierProfile()) {
    ESP_LOGI(TAG, "Profile '%s' applied", pf1550::profiles::kPortentaH7CarrierName);
  } else {
    ESP_LOGE(TAG, "Profile apply failed (flags=0x%04X)", pmic.GetErrorFlags());
  }
#else
  if (pmic.ApplyPortentaH7DefaultProfile()) {
    ESP_LOGI(TAG, "Profile '%s' applied", pf1550::profiles::kPortentaH7DefaultName);
  } else {
    ESP_LOGE(TAG, "Profile apply failed (flags=0x%04X)", pmic.GetErrorFlags());
  }
#endif
#endif

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    if (pmic.ReadPmicStatus(status)) {
      ESP_LOGI(TAG, "PMIC_STATUS=0x%02X", status);
    }
  }
}
