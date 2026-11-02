/**
 * @file my_i2c.hpp
 * @brief The bus port shown on the documentation's Quickstart and Porting pages, compiled by the host build.
 *
 * Every platform call goes through the `platform_*` functions declared below; a real port replaces them
 * with its SDK (ESP-IDF, STM32 HAL, Zephyr, Linux i2c-dev). The class is what the driver needs, nothing more.
 */
#pragma once

#include "pf1550.hpp"

#include <cstddef>
#include <cstdint>

// Platform hooks: your SDK's calls. Declared only, so the docs build checks the port compiles.
bool platform_i2c_init() noexcept;
bool platform_i2c_write(uint8_t addr, const uint8_t* bytes, std::size_t len) noexcept;  // START .. STOP
bool platform_i2c_write_read(uint8_t addr, const uint8_t* tx, std::size_t tx_len, uint8_t* rx,
                             std::size_t rx_len) noexcept;  // write, repeated START, read
void platform_gpio_write(int gpio, bool level) noexcept;
void platform_delay_us(uint32_t us) noexcept;

// --8<-- [start:port]
class MyI2c : public pf1550::BusInterface<MyI2c> {
public:
  // Bring the bus up once; the driver calls this before it first talks to the chip.
  bool EnsureInitialized() noexcept {
    if (!ready_) {
      ready_ = platform_i2c_init();
    }
    return ready_;
  }

  // One register write: [addr+W] [reg] [data] STOP. Return false on a NACK.
  bool Write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) noexcept {
    if (len != 1) {
      return false;  // the driver writes one register at a time
    }
    const uint8_t frame[2] = {reg, data[0]};
    return platform_i2c_write(addr, frame, sizeof frame);
  }

  // One register read: [addr+W] [reg] repeated START [addr+R] [data] STOP.
  bool Read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) noexcept {
    return platform_i2c_write_read(addr, &reg, 1, data, len);
  }

  // Strap pins wired to the MCU. Active means: STANDBY high (standby), USB enables high.
  void GpioSet(pf1550::CtrlPin pin, pf1550::GpioSignal signal) noexcept {
    const int gpio = gpioFor(pin);
    if (gpio >= 0) {
      platform_gpio_write(gpio, signal == pf1550::GpioSignal::Active);
    }
  }

  // Optional: profiles wait between some writes. Without it, the waits are skipped.
  void DelayUs(uint32_t us) noexcept {
    platform_delay_us(us);
  }

private:
  static int gpioFor(pf1550::CtrlPin pin) noexcept {
    switch (pin) {
    case pf1550::CtrlPin::Standby:   return 10;  // your board's GPIO numbers; -1 = not wired
    case pf1550::CtrlPin::UsbVbusEn: return 11;
    case pf1550::CtrlPin::UsbOtgEn:  return 12;
    default:                         return -1;
    }
  }

  bool ready_ = false;
};
// --8<-- [end:port]
