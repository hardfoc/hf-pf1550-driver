/**
 * @file mock_pf1550.hpp
 * @brief A register-level model of a PF1550 behind the driver's bus interface.
 *
 * The model answers at one 7-bit address (0x08 by default) and NACKs every other address. It holds
 * the 256 register bytes the driver addresses (PMIC page 0x00-0x7F, charger page 0x80-0xFF) with
 * the values a running MC34PF1550A0EP reports (DEVICE_ID 0x7C, STATE_INFO RUN, VBUS valid), treats
 * the interrupt status registers as write-1-to-clear, models the FMRADDR / FMRDATA indirect OTP
 * read behind the three unlock keys, and records every transfer, strap-pin change and delay so
 * tests can check what went over the bus. Datasheet: NXP PF1550 Rev. 7, section 12.
 */
#pragma once

#include "pf1550.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pf1550_test {

class MockPf1550 : public pf1550::BusInterface<MockPf1550> {
public:
  struct Transfer {
    bool write;
    uint8_t addr;
    uint8_t reg;
    uint8_t value;
  };

  explicit MockPf1550(uint8_t address = pf1550::kDefaultI2cAddress) noexcept : address_(address) {
    regs_.fill(0x00);
    regs_[0x00] = 0x7C;    // DEVICE_ID: FAMILY 01111, DEVICE_ID 100 (Table 86)
    regs_[0x67] = 0x0C;    // STATE_INFO: RUN (Table 146)
    regs_[0x86] = 1u << 5; // VBUS_SNS: VBUS_VALID_SNS (Table 153)
    regs_[0x87] = 0x08;    // CHG_SNS: charger off (Table 154 default)
    regs_[0x94] = 0x68;    // VBUS_INLIM_CNFG: POR code 01101 = 400 mA (Table 58)
    for (uint8_t r : {0x09, 0x0C, 0x0F, 0x19, 0x21, 0x25, 0x29}) {
      regs_[r] = 0xFF; // interrupt masks power up set
    }
  }

  // ---- BusInterface ----
  bool EnsureInitialized() noexcept {
    ++init_calls;
    return bus_ok;
  }

  bool Write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) noexcept {
    const uint8_t value = len > 0 ? data[0] : uint8_t{0};
    log_.push_back({true, addr, reg, value});
    if (addr != address_ || len != 1 || reg == fail_write_reg) {
      return false;
    }
    if (isStatusRegister(reg)) {
      regs_[reg] = static_cast<uint8_t>(regs_[reg] & ~value); // RW1C
      return true;
    }
    if (reg == 0xC4) { // FMRADDR: latch an OTP address, only once all three keys are written
      otp_addr_ = value;
    }
    regs_[reg] = value;
    return true;
  }

  bool Read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) noexcept {
    log_.push_back({false, addr, reg, 0});
    if (addr != address_ || len != 1) {
      return false;
    }
    if (reg == 0xC5) { // FMRDATA
      data[0] = otpUnlocked() ? otp[otp_addr_ & 0x3F] : 0x00;
      return true;
    }
    data[0] = regs_[reg];
    return true;
  }

  void GpioSet(pf1550::CtrlPin pin, pf1550::GpioSignal signal) noexcept {
    pins[static_cast<size_t>(pin)] = signal;
  }

  void DelayUs(uint32_t us) noexcept {
    delays.push_back(us);
  }

  // ---- Test controls ----
  uint8_t& reg(uint8_t r) noexcept {
    return regs_[r];
  }
  const std::vector<Transfer>& log() const noexcept {
    return log_;
  }
  void clearLog() noexcept {
    log_.clear();
    delays.clear();
  }
  std::vector<Transfer> writes() const {
    std::vector<Transfer> w;
    for (const auto& t : log_) {
      if (t.write) {
        w.push_back(t);
      }
    }
    return w;
  }

  bool bus_ok = true;
  int init_calls = 0;
  int fail_write_reg = -1; ///< A register address whose write NACKs, or -1.
  std::array<uint8_t, 64> otp{};
  std::array<pf1550::GpioSignal, 4> pins{pf1550::GpioSignal::Inactive, pf1550::GpioSignal::Inactive,
                                         pf1550::GpioSignal::Inactive,
                                         pf1550::GpioSignal::Inactive};
  std::vector<uint32_t> delays;

private:
  static bool isStatusRegister(uint8_t reg) noexcept {
    switch (reg) {
    case 0x08:
    case 0x0B:
    case 0x0E:
    case 0x18:
    case 0x20:
    case 0x24:
    case 0x28:
    case 0x80:
      return true;
    default:
      return false;
    }
  }

  bool otpUnlocked() const noexcept {
    return regs_[0x6F] == 0x15 && regs_[0x9F] == 0x50 && regs_[0xDF] == 0xAB;
  }

  uint8_t address_;
  uint8_t otp_addr_ = 0;
  std::array<uint8_t, 256> regs_{};
  std::vector<Transfer> log_;
};

} // namespace pf1550_test
