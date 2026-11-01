/**
 * @file esp32_pf1550_bus.hpp
 * @brief ESP32-C6 I2C bus adapter for pf1550::PF1550
 */
#pragma once

#include <array>
#include <cstring>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifdef __cplusplus
}
#endif

#include "pf1550_i2c_interface.hpp"

#include "sdkconfig.h"

static constexpr const char* TAG_PF1550_I2C = "PF1550_I2C";

class Esp32Pf1550Bus : public pf1550::BusInterface<Esp32Pf1550Bus> {
public:
  struct I2CConfig {
    i2c_port_t port = I2C_NUM_0;
    gpio_num_t sda_pin = static_cast<gpio_num_t>(CONFIG_PF1550_I2C_SDA_GPIO);
    gpio_num_t scl_pin = static_cast<gpio_num_t>(CONFIG_PF1550_I2C_SCL_GPIO);
    uint32_t frequency = static_cast<uint32_t>(CONFIG_PF1550_I2C_FREQ_HZ);
    bool pullup_enable = true;
  };

  Esp32Pf1550Bus() : Esp32Pf1550Bus(I2CConfig{}) {}

  explicit Esp32Pf1550Bus(const I2CConfig& config)
      : config_(config), bus_handle_(nullptr), initialized_(false), straps_initialized_(false) {}

  ~Esp32Pf1550Bus() { Deinit(); }

  bool EnsureInitialized() noexcept { return Init(); }

  bool Init() noexcept {
    if (initialized_) {
      return true;
    }

    initStrapGpios();

    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = config_.port;
    bus_config.sda_io_num = config_.sda_pin;
    bus_config.scl_io_num = config_.scl_pin;
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = config_.pullup_enable;

    esp_err_t ret = i2c_new_master_bus(&bus_config, &bus_handle_);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG_PF1550_I2C, "i2c_new_master_bus: %s", esp_err_to_name(ret));
      return false;
    }

    initialized_ = true;
    ESP_LOGI(TAG_PF1550_I2C, "I2C ready port=%d SDA=%d SCL=%d @ %lu Hz",
             static_cast<int>(config_.port), static_cast<int>(config_.sda_pin),
             static_cast<int>(config_.scl_pin),
             static_cast<unsigned long>(config_.frequency));
    return true;
  }

  void Deinit() noexcept {
    if (dev_handle_ != nullptr) {
      i2c_master_bus_rm_device(dev_handle_);
      dev_handle_ = nullptr;
      cached_dev_addr_ = 0xFF;
    }
    if (bus_handle_ != nullptr) {
      i2c_del_master_bus(bus_handle_);
      bus_handle_ = nullptr;
    }
    initialized_ = false;
  }

  bool Write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) noexcept {
    if (!initialized_ || bus_handle_ == nullptr) {
      return false;
    }

    i2c_master_dev_handle_t dev = getOrCreateDeviceHandle(addr);
    if (dev == nullptr) {
      return false;
    }

    std::array<uint8_t, 32> buf{};
    if (len > 31) {
      return false;
    }
    buf[0] = reg;
    if (len > 0 && data != nullptr) {
      std::memcpy(&buf[1], data, len);
    }

    return i2c_master_transmit(dev, buf.data(), len + 1, pdMS_TO_TICKS(1000)) == ESP_OK;
  }

  bool Read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) noexcept {
    if (!initialized_ || bus_handle_ == nullptr || data == nullptr || len == 0) {
      return false;
    }

    i2c_master_dev_handle_t dev = getOrCreateDeviceHandle(addr);
    if (dev == nullptr) {
      return false;
    }

    return i2c_master_transmit_receive(dev, &reg, 1, data, len, pdMS_TO_TICKS(1000)) == ESP_OK;
  }

  void DelayUs(uint32_t us) noexcept {
    if (us >= 1000) {
      vTaskDelay(pdMS_TO_TICKS((us + 999) / 1000));
    } else if (us > 0) {
      esp_rom_delay_us(us);
    }
  }

  void GpioSet(pf1550::CtrlPin pin, pf1550::GpioSignal signal) noexcept {
#if defined(CONFIG_PF1550_GPIO_STRAPS_ENABLE) && CONFIG_PF1550_GPIO_STRAPS_ENABLE
    const gpio_num_t gpio = strapPin(pin);
    if (gpio == GPIO_NUM_NC) {
      return;
    }
    const int level = strapLevel(pin, signal);
    if (level >= 0) {
      gpio_set_level(gpio, level);
    }
#else
    (void)pin;
    (void)signal;
#endif
  }

private:
  I2CConfig config_;
  i2c_master_bus_handle_t bus_handle_;
  bool initialized_;
  bool straps_initialized_;
  i2c_master_dev_handle_t dev_handle_{nullptr};
  uint8_t cached_dev_addr_{0xFF};

  void initStrapGpios() noexcept {
#if defined(CONFIG_PF1550_GPIO_STRAPS_ENABLE) && CONFIG_PF1550_GPIO_STRAPS_ENABLE
    if (straps_initialized_) {
      return;
    }
    const gpio_num_t pins[] = {
        strapPin(pf1550::CtrlPin::Standby),
        strapPin(pf1550::CtrlPin::UsbVbusEn),
        strapPin(pf1550::CtrlPin::UsbOtgEn),
    };
    for (gpio_num_t gpio : pins) {
      if (gpio == GPIO_NUM_NC) {
        continue;
      }
      gpio_config_t cfg = {};
      cfg.pin_bit_mask = 1ULL << static_cast<unsigned>(gpio);
      cfg.mode = GPIO_MODE_OUTPUT;
      cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
      cfg.pull_up_en = GPIO_PULLUP_DISABLE;
      cfg.intr_type = GPIO_INTR_DISABLE;
      (void)gpio_config(&cfg);
    }
    straps_initialized_ = true;
    ESP_LOGI(TAG_PF1550_I2C, "Strap GPIOs enabled (STBY=%d VBUS=%d OTG=%d)",
             CONFIG_PF1550_GPIO_STANDBY, CONFIG_PF1550_GPIO_USB_VBUS_EN,
             CONFIG_PF1550_GPIO_USB_OTG_EN);
#endif
  }

  static gpio_num_t strapPin(pf1550::CtrlPin pin) noexcept {
#if defined(CONFIG_PF1550_GPIO_STRAPS_ENABLE) && CONFIG_PF1550_GPIO_STRAPS_ENABLE
    int raw = -1;
    switch (pin) {
    case pf1550::CtrlPin::Standby:
      raw = CONFIG_PF1550_GPIO_STANDBY;
      break;
    case pf1550::CtrlPin::UsbVbusEn:
      raw = CONFIG_PF1550_GPIO_USB_VBUS_EN;
      break;
    case pf1550::CtrlPin::UsbOtgEn:
      raw = CONFIG_PF1550_GPIO_USB_OTG_EN;
      break;
    default:
      return GPIO_NUM_NC;
    }
    return raw >= 0 ? static_cast<gpio_num_t>(raw) : GPIO_NUM_NC;
#else
    (void)pin;
    return GPIO_NUM_NC;
#endif
  }

  static int strapLevel(pf1550::CtrlPin pin, pf1550::GpioSignal signal) noexcept {
    switch (pin) {
    case pf1550::CtrlPin::Standby:
      // LOW = RUN on Portenta (active-high for standby)
      return signal == pf1550::GpioSignal::Active ? 1 : 0;
    case pf1550::CtrlPin::UsbVbusEn:
    case pf1550::CtrlPin::UsbOtgEn:
      return signal == pf1550::GpioSignal::Active ? 1 : 0;
    default:
      return -1;
    }
  }

  i2c_master_dev_handle_t getOrCreateDeviceHandle(uint8_t addr) noexcept {
    if (dev_handle_ != nullptr && cached_dev_addr_ == addr) {
      return dev_handle_;
    }
    if (dev_handle_ != nullptr) {
      i2c_master_bus_rm_device(dev_handle_);
      dev_handle_ = nullptr;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = config_.frequency,
        .scl_wait_us = 0,
        .flags = {},
    };

    if (i2c_master_bus_add_device(bus_handle_, &dev_config, &dev_handle_) != ESP_OK) {
      dev_handle_ = nullptr;
      cached_dev_addr_ = 0xFF;
      return nullptr;
    }
    cached_dev_addr_ = addr;
    return dev_handle_;
  }
};

inline std::unique_ptr<Esp32Pf1550Bus> CreateEsp32Pf1550Bus(
    const Esp32Pf1550Bus::I2CConfig& config = Esp32Pf1550Bus::I2CConfig{}) {
  auto bus = std::make_unique<Esp32Pf1550Bus>(config);
  if (!bus->Init()) {
    return nullptr;
  }
  return bus;
}
