/**
 * @file pf1550_i2c_interface.hpp
 * @brief CRTP I2C + GPIO interface for PF1550 driver
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace pf1550 {

/**
 * @enum CtrlPin
 * @brief Platform-wired PF1550 control signals (beyond I2C).
 *
 * Portenta H7 mapping:
 * - STANDBY: PJ0, active-high for standby (drive low for RUN)
 * - USB_VBUS_EN: PJ4
 * - USB_OTG_EN: PJ6
 * - PMIC_INT: PK0 (input, optional)
 */
enum class CtrlPin : uint8_t {
  Standby = 0,
  UsbVbusEn = 1,
  UsbOtgEn = 2,
  PmicInt = 3,
};

enum class GpioSignal : uint8_t {
  Inactive = 0,
  Active = 1,
};

enum class PowerMode : uint8_t {
  Run = 0,
  Standby = 1,
};

/**
 * @brief CRTP template interface for PF1550 bus and strap pins.
 * @tparam Derived Platform bus implementation type.
 */
namespace detail {
/// True when @p Bus has a callable `DelayUs(uint32_t)` (the wait hook the driver requires).
template <typename Bus, typename = void>
inline constexpr bool kHasDelayUs = false;
template <typename Bus>
inline constexpr bool
    kHasDelayUs<Bus, std::void_t<decltype(std::declval<Bus&>().DelayUs(uint32_t{}))>> = true;
} // namespace detail

template <typename Derived>
class BusInterface {
public:
  bool Write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) noexcept {
    return static_cast<Derived*>(this)->Write(addr, reg, data, len);
  }

  bool Read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) noexcept {
    return static_cast<Derived*>(this)->Read(addr, reg, data, len);
  }

  bool EnsureInitialized() noexcept {
    return static_cast<Derived*>(this)->EnsureInitialized();
  }

  void GpioSet(CtrlPin pin, GpioSignal signal) noexcept {
    static_cast<Derived*>(this)->GpioSet(pin, signal);
  }

  void GpioSetActive(CtrlPin pin) noexcept { GpioSet(pin, GpioSignal::Active); }

  void GpioSetInactive(CtrlPin pin) noexcept { GpioSet(pin, GpioSignal::Inactive); }

  // DelayUs(uint32_t us) is required in the derived bus and has no default here: the driver's
  // waits (profile pacing, the 2 ms OTP read wait, power sequencing) must really wait. A bus without
  // it is rejected at compile time by PF1550<BusType> (see detail::kHasDelayUs).

  BusInterface(const BusInterface&) = delete;
  BusInterface& operator=(const BusInterface&) = delete;
  BusInterface(BusInterface&&) = delete;
  BusInterface& operator=(BusInterface&&) = delete;

protected:
  BusInterface() = default;
  ~BusInterface() = default;
};

} // namespace pf1550
