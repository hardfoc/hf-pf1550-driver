/**
 * @file pf1550_esp32c6_register_dump.cpp
 * @brief Dump selected PF1550 registers over I2C (ESP32-C6)
 */

#include "esp32_pf1550_bus.hpp"

#include <array>

#include "pf1550.hpp"
#include "pf1550_registers.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "pf1550_dump";

namespace {

constexpr std::array<pf1550::Register, 16> kDumpRegs = {
    pf1550::Register::DeviceId,
    pf1550::Register::Sw1Volt,
    pf1550::Register::Sw1Ctrl,
    pf1550::Register::Sw2Volt,
    pf1550::Register::Sw2Ctrl,
    pf1550::Register::Sw3Volt,
    pf1550::Register::Sw3Ctrl1,
    pf1550::Register::Ldo1Volt,
    pf1550::Register::Ldo1Ctrl,
    pf1550::Register::Ldo2Volt,
    pf1550::Register::Ldo2Ctrl,
    pf1550::Register::Ldo3Volt,
    pf1550::Register::Ldo3Ctrl,
    pf1550::Register::VbusInCurrentLimit,
    pf1550::Register::StateInfo,
    pf1550::Register::ChgSense,
};

} // namespace

extern "C" void app_main(void) {
  auto bus = CreateEsp32Pf1550Bus();
  if (!bus) {
    ESP_LOGE(TAG, "I2C init failed");
    return;
  }

  pf1550::PF1550<Esp32Pf1550Bus> pmic(bus.get());

  if (!pmic.EnsureInitialized()) {
    ESP_LOGW(TAG, "PF1550 not detected — register dump skipped");
    return;
  }

  ESP_LOGI(TAG, "Register dump:");
  for (const auto reg : kDumpRegs) {
    uint8_t val = 0;
    if (pmic.ReadRegister(reg, val)) {
      ESP_LOGI(TAG, "  0x%02X = 0x%02X", static_cast<unsigned>(reg), val);
    }
  }

  while (true) {
    vTaskDelay(portMAX_DELAY);
  }
}
