/**
 * @file pf1550_profiles.hpp
 * @brief Named PF1550 register-write sequences (eval / reference hosts).
 *
 * Downstream products that are not these eval boards pass their own table
 * to @ref pf1550::PF1550::ApplyProfile.
 *
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <array>
#include <cstdint>

namespace pf1550::profiles {

/**
 * @struct RegisterWrite
 * @brief Single I2C register write with optional post-write delay.
 */
struct RegisterWrite {
  uint8_t reg;
  uint8_t value;
  /** Microseconds to wait after this write (0 = none). */
  uint32_t delay_us;
};

/**
 * @brief Portenta H7 eval MCU-domain profile (VFR heritage).
 *
 * Sequence derived from PortentaH7_VFR I2C_PMIC_Initialize(). Some SW3 voltage
 * registers may be OTP-locked on programmed parts; writes are still issued for
 * A0EP (unprogrammed OTP) bring-up.
 *
 * Register 0x50 (LDO2_CTRL commit) is intentionally last in this table.
 */
inline constexpr std::array<RegisterWrite, 23> kPortentaH7Default = {{
    {0x4F, 0x00, 0},   // LDO2 -> 1.8 V
    {0x4C, 0x05, 0},   // LDO1 -> 1.0 V
    {0x4D, 0x0F, 0},   // LDO1 enable
    {0x52, 0x09, 0},   // LDO3 -> 1.2 V
    {0x53, 0x0F, 0},   // LDO3 enable
    {0x58, 0x03, 0},   // PWRCTRL0: STANDBYDLY = 3, STANDBYINV = 0
    {0x9C, 0x80, 1000}, // charger LED duty (Portenta bootloader)
    {0x9E, 0x20, 1000}, // disable charger LED
    {0x42, 0x02, 1000}, // SW3_CTRL1: current limit 1.5 A typical (Table 126)
    {0x94, 0xA0, 0},   // VBUS input limit 1500 mA
    {0x38, 0x07, 0},   // SW2 RUN 3.3 V
    {0x39, 0x05, 0},   // SW2 STBY/DVS
    {0x3A, 0x05, 0},   // SW2 alt
    {0x3B, 0x01, 0},   // SW2_CTRL: on in RUN only
    {0x32, 0x07, 0},   // SW1 RUN 3.3 V (VCAP / MCU digital)
    {0x33, 0x05, 0},   // SW1 STBY/DVS
    {0x34, 0x05, 0},   // SW1 alt
    {0x35, 0x01, 0},   // SW1 ctrl
    {0x3E, 0x0F, 0},   // SW3 RUN (OTP may override on programmed parts)
    {0x3F, 0x0E, 0},   // SW3 STBY/DVS
    {0x40, 0x0E, 0},   // SW3 alt
    {0x41, 0x0F, 0},   // SW3 ctrl
    {0x50, 0x0F, 0},   // LDO2 commit / final enable (must be last)
}};

/** @brief Human-readable profile identifier for logging and HAL binding. */
inline constexpr const char* kPortentaH7DefaultName = "portenta_h7_default";

/**
 * @brief Portenta H7 + carrier bring-up profile (SW1-first, full SW2 enable).
 *
 * Use when LDO1/2/3 inputs are wired to +3V1SW (BUCK1 / SW1 output) and the
 * carrier HDC needs +3V3 on SW2 (BUCK2) including JTAG VTref.
 *
 * Differences from kPortentaH7Default:
 * - SW1 (+3V1SW) voltage + enable written **before** LDO registers.
 * - SW2_CTRL = 0x0F (RUN + STBY + sleep + LPWR) keeps carrier +3V3 up.
 * - SW1 RUN set to 3.0 V (code 0x06) as used on the Portenta H7 SW1 rail.
 *
 * Requires a **full PMIC power cycle** after first apply on cold boot.
 */
inline constexpr std::array<RegisterWrite, 23> kPortentaH7Carrier = {{
    {0x32, 0x06, 0},    // SW1 RUN 3.0 V (+3V1SW) — LDO input rail
    {0x33, 0x05, 0},    // SW1 STBY/DVS
    {0x34, 0x05, 0},    // SW1 SLP
    {0x35, 0x0F, 5000}, // SW1 enable (delay for +3V1SW before LDOs)
    {0x4F, 0x00, 0},    // LDO2 -> 1.8 V
    {0x4C, 0x05, 0},    // LDO1 -> 1.0 V
    {0x4D, 0x0F, 0},    // LDO1 enable
    {0x52, 0x09, 0},    // LDO3 -> 1.2 V
    {0x53, 0x0F, 0},    // LDO3 enable
    {0x58, 0x03, 0},    // PWRCTRL0: STANDBYDLY = 3, STANDBYINV = 0
    {0x9C, 0x80, 1000}, // charger LED duty
    {0x9E, 0x20, 1000}, // disable charger LED (CHGB blips)
    {0x42, 0x02, 1000}, // SW3_CTRL1: current limit 1.5 A typical (Table 126)
    {0x94, 0xA0, 0},    // VBUS input limit 1500 mA
    {0x38, 0x07, 0},    // SW2 RUN 3.3 V (carrier +VOUT)
    {0x39, 0x05, 0},    // SW2 STBY/DVS
    {0x3A, 0x05, 0},    // SW2 SLP
    {0x3B, 0x0F, 0},    // SW2 ctrl — full enable for carrier
    {0x3E, 0x0F, 0},    // SW3 RUN (+3V1 VCORE, OTP may override)
    {0x3F, 0x0E, 0},    // SW3 STBY/DVS
    {0x40, 0x0E, 0},    // SW3 SLP
    {0x41, 0x0F, 0},    // SW3 ctrl
    {0x50, 0x0F, 0},    // LDO2 commit / final enable (must be last)
}};

inline constexpr const char* kPortentaH7CarrierName = "portenta_h7_carrier";

} // namespace pf1550::profiles
