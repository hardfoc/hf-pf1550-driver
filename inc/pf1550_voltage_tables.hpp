/**
 * @file pf1550_voltage_tables.hpp
 * @brief PF1550 SW / LDO voltage-code ↔ millivolt conversion helpers.
 *
 * Encodings come from the PF1550 datasheet (Rev. 7) Tables 31 (SW1/2/3) and 41
 * (LDO1/2/3). All conversions are `constexpr` and table-free where possible so
 * they can be used in driver self-test paths and `static_assert`.
 *
 * @par Notation
 *   - "code"  — 6-bit (SW) or 5-bit (LDO) voltage register field.
 *   - "mV"    — output voltage in millivolts (integer).
 *
 * @par Validity
 *   The PF1550 only programs voltages within a discrete grid; codes outside
 *   the documented range return `0` for mV and `Invalid` for code-construction
 *   helpers. Callers must use these helpers — never `static_cast` arbitrary
 *   integers into @ref pf1550::SwVoltageCode / @ref pf1550::LdoVoltageCode.
 *
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cstdint>

#include "pf1550_registers.hpp"

namespace pf1550 {

/**
 * @brief SW1 / SW2 output voltage, **DVS disabled** (`OTP_SWx_DVS_SEL = 1`).
 *
 * Datasheet Rev. 7 Table 31, right column: eight set points, every code from
 * 7 up is 3.30 V. Portenta H7 runs SW1 at 3.0 V and SW2 at 3.3 V — values that
 * exist only in this table, so its SW1/SW2 are DVS-disabled. (In that mode the
 * datasheet makes `SWx_VOLT` read-only.)
 *
 * | Code | mV | | Code | mV |
 * |------|----|-|------|----|
 * | 0 | 1100 | | 4 | 1800 |
 * | 1 | 1200 | | 5 | 2500 |
 * | 2 | 1350 | | 6 | 3000 |
 * | 3 | 1500 | | ≥ 7 | 3300 |
 *
 * @param code Raw `SWx_VOLT[5:0]`.
 * @return Output voltage in mV.
 */
constexpr uint16_t SwCodeToMillivolts(uint8_t code) noexcept {
  constexpr uint16_t kTable[7] = {1100, 1200, 1350, 1500, 1800, 2500, 3000};
  const uint8_t c = static_cast<uint8_t>(code & 0x3FU);
  return c < 7U ? kTable[c] : 3300U;
}

/**
 * @brief SW1 / SW2 output voltage, **DVS enabled** (`OTP_SWx_DVS_SEL = 0`):
 *        0.6000 V + 12.5 mV × code (Table 31, left column), rounded to mV.
 */
constexpr uint16_t SwDvsCodeToMillivolts(uint8_t code) noexcept {
  return static_cast<uint16_t>(600U + (static_cast<uint32_t>(code & 0x3FU) * 25U + 1U) / 2U);
}

/**
 * @brief SW3 output voltage (Table 37): 1.80 V + 100 mV × `SW3_VOLT[3:0]`.
 * @note SW3 is loaded from OTP and read-only (it has no DVS) — writes are ignored.
 */
constexpr uint16_t Sw3CodeToMillivolts(uint8_t code) noexcept {
  return static_cast<uint16_t>(1800U + 100U * (code & 0x0FU));
}

/**
 * @brief Build a 6-bit SW voltage code from millivolts (Portenta-verified codes).
 *
 * Returns `0xFF` if the requested voltage is not a verified Portenta setting.
 * Use @ref SwVoltageCode for the type-safe variant.
 */
constexpr uint8_t SwMillivoltsToCode(uint16_t mv) noexcept {
  switch (mv) {
    case 1100: return 0x00;
    case 1200: return 0x01;
    case 1350: return 0x02;
    case 1500: return 0x03;
    case 1800: return 0x04;
    case 2500: return 0x05;
    case 3000: return 0x06;
    case 3300: return 0x07;
    default:   return 0xFFU;
  }
}

/**
 * @brief Decode a 5-bit LDO voltage register field to millivolts.
 *
 * Per datasheet Table 41 (LDO1/2/3 voltage encoding):
 *  - LDO1, LDO3 (Group A): codes `0x00..0x0F` cover 750 mV → 1500 mV (step 50 mV),
 *                         codes `0x10..0x1F` cover 1800 mV → 3300 mV (step 100 mV).
 *  - LDO2       (Group B): codes `0x00..0x0F` cover 1800 mV → 3300 mV (step 100 mV).
 *
 * @param code  Raw 5-bit value from `LDOn_VOLT[4:0]`.
 * @param is_ldo2 `true` for LDO2 (Group B), `false` for LDO1/LDO3 (Group A).
 * @return Output voltage in mV, or `0` if reserved.
 */
constexpr uint16_t LdoCodeToMillivolts(uint8_t code, bool is_ldo2) noexcept {
  const uint8_t c = static_cast<uint8_t>(code & 0x1FU);
  if (is_ldo2) {
    if (c <= 0x0FU) {
      return static_cast<uint16_t>(1800U + 100U * c);
    }
    return 0U;
  }
  if (c <= 0x0FU) {
    return static_cast<uint16_t>(750U + 50U * c);
  }
  return static_cast<uint16_t>(1800U + 100U * (c - 0x10U));
}

/**
 * @brief Decode VBUS_IN_LIM_CNFG (absolute address 0x94) register byte to mA.
 *
 * The VBUS input current limit field occupies bits **[7:3]** of the register
 * and uses a non-linear lookup table (datasheet Table 58):
 *
 * | code | mA  | | code | mA   |
 * |------|-----|-|------|------|
 * | 0x00 | 10  | | 0x10 | 700  |
 * | 0x01 | 15  | | 0x11 | 800  |
 * | 0x02 | 20  | | 0x12 | 900  |
 * | 0x03 | 25  | | 0x13 | 1000 |
 * | 0x04 | 30  | | 0x14 | **1500** (Portenta default) |
 * | 0x05 | 35  | | 0x15..0x1F | reserved |
 * | 0x06 | 40  | |      |      |
 * | 0x07 | 45  | |      |      |
 * | 0x08 | 50  | |      |      |
 * | 0x09 | 100 | |      |      |
 * | …    | …   | |      |      |
 * | 0x0F | 600 | |      |      |
 *
 * Portenta H7 writes `(20 << 3) == 0xA0` (code 0x14 = 1500 mA).
 *
 * @param reg_value Raw register byte (bits 2:0 ignored).
 * @return Decoded current limit in mA; reserved codes (21..31) return `0`.
 */
constexpr uint16_t VbusLimitRegToMilliamps(uint8_t reg_value) noexcept {
  const uint8_t code = static_cast<uint8_t>((reg_value >> 3U) & 0x1FU);
  // First 9 entries: 10, 15, 20, 25, 30, 35, 40, 45, 50 mA (5 mA step from 10).
  if (code <= 0x08U) {
    return static_cast<uint16_t>(10U + 5U * code);
  }
  // 0x09..0x0F: 100, 150, 200, 300, 400, 500, 600 mA.
  switch (code) {
    case 0x09: return 100;
    case 0x0A: return 150;
    case 0x0B: return 200;
    case 0x0C: return 300;
    case 0x0D: return 400;
    case 0x0E: return 500;
    case 0x0F: return 600;
    case 0x10: return 700;
    case 0x11: return 800;
    case 0x12: return 900;
    case 0x13: return 1000;
    case 0x14: return 1500;  // Portenta default
    default: return 0;       // codes 21..31 are reserved (Table 58)
  }
}

/**
 * @brief Encode mA → VBUS_IN_LIM_CNFG (0x94) register byte (bits 7:3).
 *
 * Only common values are supported (10/100/500/1000/1500 mA). Out-of-grid
 * requests return `0xFF`.
 */
constexpr uint8_t VbusLimitMilliampsToReg(uint16_t mA) noexcept {
  uint8_t code = 0xFFU;
  switch (mA) {
    case 10:   code = 0x00; break;
    case 100:  code = 0x09; break;
    case 500:  code = 0x0E; break;
    case 1000: code = 0x13; break;
    case 1500: code = 0x14; break;  // Portenta default
  }
  return (code == 0xFFU) ? 0xFFU : static_cast<uint8_t>(code << 3U);
}

/**
 * @brief Decode the `SW3_CTRL1[1:0]` (or SW1/SW2_CTRL1) current limit field.
 *
 * Encoded current limits per Tables 116, 121, 126:
 *  - `00` → 1.0 A typical
 *  - `01` → 1.2 A typical
 *  - `10` → 1.5 A typical
 *  - `11` → 2.0 A typical
 */
constexpr uint16_t SwCurrentLimitCodeToMilliamps(uint8_t code) noexcept {
  switch (code & 0x03U) {
    case 0: return 1000;
    case 1: return 1200;
    case 2: return 1500;
    case 3: return 2000;
  }
  return 0;
}

/**
 * @brief Convert STATE_INFO (0x67) full byte into a typed @ref PmicState.
 *
 * Only `STATE[5:0]` carries meaning; upper bits are reserved.
 */
constexpr PmicState DecodeStateInfo(uint8_t reg) noexcept {
  switch (reg & 0x3FU) {
    case 0b000000: return PmicState::Wait;
    case 0b001100: return PmicState::Run;
    case 0b001101: return PmicState::Standby;
    case 0b001110: return PmicState::Sleep;
    case 0b101011: return PmicState::RegsDisable;
    default:       return PmicState::Unknown;
  }
}

/// @brief Convert CHG_SNS (0x87) into a typed @ref ChargerState.
constexpr ChargerState DecodeChargerSense(uint8_t reg) noexcept {
  switch (reg & 0x0FU) {
    case 0:  return ChargerState::PreCharge;
    case 1:  return ChargerState::FastCC;
    case 2:  return ChargerState::FastCV;
    case 3:  return ChargerState::EndOfCharge;
    case 4:  return ChargerState::Done;
    case 6:  return ChargerState::TimerFault;
    case 7:  return ChargerState::ThermSuspend;
    case 8:  return ChargerState::Off;
    case 9:  return ChargerState::BatteryOv;
    case 10: return ChargerState::TempShutdown;
    case 12: return ChargerState::LinearOnly;
    default: return ChargerState::Unknown;
  }
}

/**
 * @brief Compile-time sanity checks for the conversion helpers.
 *
 * Kept inline so misconfiguration would fail to compile the driver itself.
 */
static_assert(SwCodeToMillivolts(0x06) == 3000, "SW code 0x06 → 3.00 V (Portenta SW1 RUN)");
static_assert(SwCodeToMillivolts(static_cast<uint8_t>(SwVoltageCode::V3_3)) == 3300,
              "SwVoltageCode::V3_3 must decode to 3.30 V");
static_assert(Sw3CodeToMillivolts(static_cast<uint8_t>(SwVoltageCode::V3_1)) == 3100,
              "SW3 code 0x0D → 3.10 V (Table 37)");
static_assert(SwCodeToMillivolts(0x0D) == 3300, "SW1/SW2 DVS-disabled: every code ≥ 7 is 3.30 V");
static_assert(SwDvsCodeToMillivolts(0x00) == 600 && SwDvsCodeToMillivolts(0x3F) == 1388 &&
                  SwDvsCodeToMillivolts(0x20) == 1000,
              "SW1/SW2 DVS-enabled: 0.6 V + 12.5 mV steps");
static_assert(Sw3CodeToMillivolts(0x0F) == 3300 && Sw3CodeToMillivolts(0x00) == 1800, "SW3 Table 37");
static_assert(SwMillivoltsToCode(3300) == 0x07, "3300 mV round-trip → 0x07");
static_assert(LdoCodeToMillivolts(static_cast<uint8_t>(LdoVoltageCode::V1_0), /*is_ldo2=*/false) == 1000,
              "LDO1 code 0x05 → 1.0 V");
static_assert(LdoCodeToMillivolts(static_cast<uint8_t>(LdoVoltageCode::V1_8), /*is_ldo2=*/true) == 1800,
              "LDO2 code 0x00 → 1.8 V");
static_assert(LdoCodeToMillivolts(static_cast<uint8_t>(LdoVoltageCode::V1_2), /*is_ldo2=*/false) == 1200,
              "LDO3 code 0x09 → 1.2 V");
static_assert(VbusLimitRegToMilliamps(0xA0) == 1500,
              "Portenta sets 0xA0 (code 0x14) → 1500 mA per VBUS_IN_LIM_CNFG lookup");
static_assert(VbusLimitMilliampsToReg(1500) == 0xA0,
              "VbusLimitMilliampsToReg(1500) must round-trip to 0xA0");

} // namespace pf1550
