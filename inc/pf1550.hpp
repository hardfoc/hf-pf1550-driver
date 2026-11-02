/**
 * @file pf1550.hpp
 * @brief Hardware-agnostic driver for the NXP PF1550 PMIC family.
 *
 * The PF1550 is a multi-rail power-management IC (3 bucks, 3 LDOs, Li-Ion
 * charger, VSNVS backup LDO, OTP-programmable sequencing). Named register
 * sequences for the Arduino Portenta H7 eval module live in
 * @ref pf1550_profiles.hpp; other hosts pass their own table to
 * @ref PF1550::ApplyProfile.
 *
 * @par Architecture
 *  - Templated on a **CRTP `BusInterface`** so the same driver runs on STM32H7
 *    HAL, ESP-IDF i2c-master, or a host stub for unit tests.
 *  - Header-only with a `.ipp` template implementation file.
 *  - Returns plain `bool` for success; persistent `error_flags_` records the
 *    last failure category (see @ref Error).
 *  - Diagnostic surface (@ref ReadDiagnosticSnapshot, @ref RunPowerSelfTest,
 *    interrupt clear helpers) for a host monitor thread.
 *
 * @par Datasheet
 *  - Authoritative reference: NXP PF1550 Rev. 7 (29 September 2021).
 *  - Local extract: `docs/datasheet/PF1550-i2c-register-reference.md`.
 *
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "pf1550_diagnostics.hpp"
#include "pf1550_i2c_interface.hpp"
#include "pf1550_profiles.hpp"
#include "pf1550_registers.hpp"
#include "pf1550_voltage_tables.hpp"
#include "pf1550_version.h"

namespace pf1550 {

/**
 * @class PF1550
 * @brief CRTP-based PF1550 PMIC driver.
 *
 * @tparam BusType Platform bus type inheriting from `pf1550::BusInterface<BusType>`.
 *         Must implement `Write(addr, reg, data, len)`, `Read(addr, reg, data, len)`,
 *         `EnsureInitialized()`, `GpioSet(CtrlPin, GpioSignal)`, and
 *         `DelayUs(uint32_t)`.
 *
 * @par Thread-safety
 *   The driver itself is **not** thread-safe by design — it expects the
 *   `BusType` adapter (or a host mutex wrapping it) to enforce
 *   single-writer access. Concurrent reads from another core are tolerated
 *   provided the bus adapter serialises I²C transactions.
 *
 * @par Single-point-of-failure mitigation
 *   See @ref pf1550::FaultSeverityPortentaH7 — that mapping is the Portenta H7
 *   MCU-on-SW3 eval tree. Other hosts supply their own severity table.
 */
template <typename BusType>
class PF1550 {
public:
  /// @brief Bitmask error codes accumulated in `error_flags_`.
  enum class Error : uint16_t {
    None             = 0,
    I2cWrite         = 1 << 0, ///< Last I²C write transaction failed.
    I2cRead          = 1 << 1, ///< Last I²C read transaction failed.
    InvalidParam     = 1 << 2, ///< Caller-side argument validation failed.
    DeviceNotFound   = 1 << 3, ///< DEVICE_ID read returned NACK / wrong address.
    NotInitialized   = 1 << 4, ///< Called before EnsureInitialized() succeeded.
    WrongDeviceId    = 1 << 5, ///< Returned DEVICE_ID ≠ kExpectedDeviceId (0x7C).
    ProfileApply     = 1 << 6, ///< At least one write in the last applied profile failed.
    DiagnosticRead   = 1 << 7, ///< One or more reads while building a snapshot failed.
    SelfTestFailed   = 1 << 8, ///< RunPowerSelfTest() reported a non-Info severity.
  };

  // ---------------------------------------------------------------------------
  // Construction / lifecycle
  // ---------------------------------------------------------------------------

  /**
   * @brief Construct a driver instance bound to a CRTP bus adapter.
   *
   * @param bus      Pointer to a `BusType` instance (lifetime: longer than the
   *                 driver). May be `nullptr` only for testing; all methods
   *                 will then return `false` and set `Error::InvalidParam`.
   * @param address  7-bit I²C address. Defaults to @ref kDefaultI2cAddress.
   */
  PF1550(BusType* bus, uint8_t address = kDefaultI2cAddress) noexcept;

  /**
   * @brief Idempotently initialise the bus and verify the device.
   *
   * Calls `bus_->EnsureInitialized()`, then performs @ref VerifyDevice.
   * After success, subsequent calls are O(1) no-ops.
   *
   * @return `true` if a PF1550-family device was found at the configured address.
   */
  bool EnsureInitialized() noexcept;

  /// @copydoc EnsureInitialized — query without re-running the bus init.
  bool IsInitialized() const noexcept { return initialized_; }

  /// @brief Currently configured 7-bit I²C address.
  uint8_t GetI2cAddress() const noexcept { return address_; }

  // ---------------------------------------------------------------------------
  // Identity
  // ---------------------------------------------------------------------------

  /**
   * @brief Read DEVICE_ID (Reg 0x00).
   *
   * Expected value: @ref kExpectedDeviceId. Other family IDs cause
   * `Error::WrongDeviceId` to be set.
   */
  bool ReadDeviceId(uint8_t& id) noexcept;

  /**
   * @brief Read DEVICE_ID and assert it matches @ref kExpectedDeviceId.
   *
   * @return `true` on a positive match; `false` on NACK or wrong ID.
   */
  bool VerifyDevice() noexcept;

  // ---------------------------------------------------------------------------
  // Raw register access
  // ---------------------------------------------------------------------------

  /// @brief Read any PF1550 register by enumerated address.
  bool ReadRegister(Register reg, uint8_t& value) noexcept;

  /// @brief Write any PF1550 register by enumerated address.
  bool WriteRegister(Register reg, uint8_t value) noexcept;

  // ---------------------------------------------------------------------------
  // Profile application
  // ---------------------------------------------------------------------------

  /**
   * @brief Apply an arbitrary profile (sequence of register writes).
   *
   * The profile is applied **in order**, observing each entry's `delay_us`
   * setting. Used for the Portenta H7 default and carrier variants below, and
   * for board-specific profiles in downstream projects.
   *
   * @note PF1550 state survives MCU reset — voltage changes typically require
   *       a **full power cycle** to take effect. Soft reset alone is
   *       insufficient.
   */
  bool ApplyProfile(std::span<const profiles::RegisterWrite> profile) noexcept;

  /// @brief Apply the legacy VFR-heritage Portenta H7 default profile.
  bool ApplyPortentaH7DefaultProfile() noexcept;

  /**
   * @brief Apply the Portenta-H7-on-carrier profile.
   *
   * Differences from the default:
   *  1. **SW1 (+3V1SW) sequenced first** — feeds LDO inputs on Portenta.
   *  2. `SW2_CTRL = 0x0F` — full enable including STANDBY (carrier +3V3 stable).
   *  3. `SW1_CTRL = 0x0F` — SW1 stays on in STANDBY and SLEEP as well.
   */
  bool ApplyPortentaH7CarrierProfile() noexcept;

  // ---------------------------------------------------------------------------
  // Power mode / strap GPIOs
  // ---------------------------------------------------------------------------

  /**
   * @brief Drive the STANDBY strap pin to RUN or STANDBY.
   *
   * Only meaningful if the bus adapter wires `CtrlPin::Standby` to a real GPIO.
   * STANDBY polarity follows the PF1550 STANDBYINV bit (RUN = LOW on Portenta).
   */
  bool SetPowerMode(PowerMode mode) noexcept;

  /// @brief Drive `USB_VBUS_EN` and `USB_OTG_EN` straps (HIGH = enabled on Portenta).
  bool SetUsbRails(bool vbus_en, bool otg_en) noexcept;

  // ---------------------------------------------------------------------------
  // Status / OTP
  // ---------------------------------------------------------------------------

  /**
   * @brief Read STATE_INFO (Reg 0x67) and return the raw byte.
   *
   * Use @ref ReadPmicState for a decoded @ref PmicState enumerator.
   */
  bool ReadPmicStatus(uint8_t& status) noexcept;

  /// @brief Decoded variant of @ref ReadPmicStatus.
  bool ReadPmicState(PmicState& state) noexcept;

  /// @brief Read CHG_SNS (Reg 0x87) — charger sub-state. Returns decoded enum.
  bool ReadChargerState(ChargerState& state, uint8_t* raw_reg = nullptr) noexcept;

  /// @brief Read a single OTP byte via FMRADDR/FMRDATA indirection.
  bool ReadOtpByte(uint8_t otp_addr, uint8_t& value) noexcept;

  /// @brief Read a contiguous region of OTP memory (default: full 0x1C..0x36 range).
  bool ReadOtpRegion(uint8_t* buffer, size_t len, uint8_t start = OtpRegion::kStart) noexcept;

  // ---------------------------------------------------------------------------
  // Voltage / current configuration
  // ---------------------------------------------------------------------------

  /// @brief Write `SWn_VOLT` for n=1..3. Returns `false` if `sw_index` is invalid.
  bool SetSwVoltage(uint8_t sw_index, SwVoltageCode run_code) noexcept;

  /// @brief Write `LDOn_VOLT` for n=1..3. Returns `false` if `ldo_index` is invalid.
  bool SetLdoVoltage(uint8_t ldo_index, LdoVoltageCode code) noexcept;

  /// @brief Convenience: write VBUS_IN_LIM_CNFG (0x94) from a mA value.
  bool SetVbusCurrentLimitMa(uint16_t limit_ma) noexcept;

  // ---------------------------------------------------------------------------
  // Interrupt management
  // ---------------------------------------------------------------------------

  /**
   * @brief Read INT_CATEGORY (Reg 0x06).
   *
   * Top-level summary of pending interrupt categories — useful for fast
   * polling before reading individual STAT registers.
   */
  bool ReadInterruptCategory(uint8_t& category) noexcept;

  /**
   * @brief Read every category STAT register and aggregate latched faults.
   *
   * Does **not** clear the STAT bits. Use @ref ClearLatchedFaults for that.
   */
  bool ReadLatchedFaults(FaultFlags& faults) noexcept;

  /**
   * @brief Write `0xFF` to every category STAT register to clear all RW1C bits.
   *
   * Call after handling a fault to re-arm the interrupt path.
   */
  bool ClearLatchedFaults() noexcept;

  /**
   * @brief Mask (1) or unmask (0) all interrupt categories at once.
   *
   * @param masked  `true` = silence all interrupts (powers up like this);
   *                `false` = allow events to propagate to INTB.
   */
  bool SetInterruptMaskAll(bool masked) noexcept;

  // ---------------------------------------------------------------------------
  // Diagnostics / self-test
  // ---------------------------------------------------------------------------

  /**
   * @brief Build a full diagnostic snapshot (≈30 register reads).
   *
   * Safe to call from a periodic monitor thread. If any read fails, the
   * snapshot is returned with `read_ok = false` and `Error::DiagnosticRead`
   * is set; partial fields are still populated for forensic logging.
   */
  bool ReadDiagnosticSnapshot(DiagnosticSnapshot& out) noexcept;

  /**
   * @brief Refresh only the status part of a snapshot (≈14 register reads).
   *
   * Re-reads what can change without a register write: INT_CATEGORY,
   * STATE_INFO, charger and VBUS sense, and the latched + live fault
   * registers (with the per-rail fault marks). Identity, rail set points and
   * enables and the VBUS input limit are kept from the last
   * @ref ReadDiagnosticSnapshot into @p snap — a monitor polling at 20 Hz
   * does a full read now and then (say once a second) to catch a
   * configuration change, and the status in between at half the bus time.
   *
   * @param[in,out] snap Snapshot from a previous full read.
   * @return true when every status read succeeded (sets `read_ok`).
   */
  bool RefreshStatusSnapshot(DiagnosticSnapshot& snap) noexcept;

  /**
   * @brief Boot-time self-test.
   *
   * Reads a fresh snapshot and classifies it against the
   * Portenta-H7-carrier expectations (PMIC in RUN, all SW/LDO enabled, no
   * MCU-affecting faults latched). Used by the @c SystemBootRecipe row.
   */
  bool RunPowerSelfTest(SelfTestResult& out) noexcept;

  // ---------------------------------------------------------------------------
  // Driver-internal error tracking
  // ---------------------------------------------------------------------------

  /// @brief Read all sticky error bits (see @ref Error).
  uint16_t GetErrorFlags() const noexcept { return error_flags_; }

  /// @brief Clear sticky error bits matching `mask`.
  void ClearErrorFlags(uint16_t mask = 0xFFFF) noexcept {
    error_flags_ &= static_cast<uint16_t>(~mask);
  }

  /**
   * @brief Declare the SW1 / SW2 voltage encoding the part's OTP selects.
   * @details `OTP_SWx_DVS_SEL` decides between two tables (datasheet Table
   *          31): DVS enabled 0.6–1.3875 V in 12.5 mV steps, DVS disabled an
   *          8-point table up to 3.3 V. It lives in OTP (indirect access only),
   *          so the board declares it; the default is **disabled** (Portenta
   *          H7: SW1 3.0 V / SW2 3.3 V exist only in that table).
   * @param sw_index 1 or 2.
   */
  void SetSwDvsEnabled(uint8_t sw_index, bool enabled) noexcept {
    if (sw_index == 1U || sw_index == 2U) {
      sw_dvs_enabled_[sw_index - 1U] = enabled;
    }
  }

  /// @brief Driver version (string).
  static constexpr const char* GetDriverVersion() noexcept { return HF_PF1550_VERSION; }

private:
  bool writeReg8(uint8_t reg, uint8_t value) noexcept;
  bool readReg8(uint8_t reg, uint8_t& value) noexcept;
  void setError(Error e) noexcept { error_flags_ |= static_cast<uint16_t>(e); }

  // Snapshot helpers (templated to keep the header-only interface tidy).
  bool fillRailSnapshot(DiagnosticSnapshot& snap) noexcept;
  bool fillFaultSnapshot(DiagnosticSnapshot& snap) noexcept;

  BusType* bus_;
  uint8_t  address_;
  bool     initialized_;
  uint16_t error_flags_;
  bool     sw_dvs_enabled_[2]{false, false};  ///< SW1, SW2: OTP_SWx_DVS_SEL == 0
};

} // namespace pf1550

#include "../src/pf1550.ipp"
