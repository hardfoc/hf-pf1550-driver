/**
 * @file pf1550_diagnostics.hpp
 * @brief PF1550 diagnostic / fault-classification types and snapshot containers.
 *
 * These POD types are exchanged between the low-level driver and HAL handlers
 * / managers. They contain **no I²C dependencies** so they can be passed
 * across cores, logged, or unit-tested on the host.
 *
 * The PMIC reports faults via three orthogonal sources:
 *
 *  1. **INT_CATEGORY (0x06)** — *master* summary of pending interrupt
 *     categories (CHG, SW1/2/3, LDO, ONKEY, TEMP, MISC).
 *  2. **Per-category STAT registers** — RW1C latched events
 *     (`SW_INT_STAT0..2`, `LDO_INT_STAT0`, `TEMP_INT_STAT0`, …).
 *  3. **Per-category SENSE registers** — read-only *live* state used to
 *     differentiate a transient event from an ongoing condition.
 *
 * The diagnostics snapshot in this file captures **all three** so a host
 * can decide between *latched warning*, *ongoing fault*, and
 * *MCU-power-affecting fault* without re-reading the bus.
 *
 * @par Host fault mapping
 *   Downstream products that reuse this driver must keep their own
 *   traceability table in sync when adding or removing a flag here.
 *
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "pf1550_registers.hpp"

namespace pf1550 {

/**
 * @enum RailId
 * @brief Logical identifier for each independently-controllable PF1550 output.
 *
 * Used to index into @ref RailDiagnostics arrays and to label log lines.
 */
enum class RailId : uint8_t {
  Sw1   = 0,  ///< SW1 / BUCK1 — Portenta +3V1SW (feeds LDO inputs).
  Sw2   = 1,  ///< SW2 / BUCK2 — Portenta +3V3 / carrier VOUT.
  Sw3   = 2,  ///< SW3 / BUCK3 — Portenta +3V1 VCORE (MCU + QSPI).
  Ldo1  = 3,  ///< LDO1 — Portenta +1V0 (MIPI bridge analog).
  Ldo2  = 4,  ///< LDO2 — Portenta +1V8 (USB PHY, MIPI).
  Ldo3  = 5,  ///< LDO3 — Portenta +1V2 (STM32 DSI, ETH PHY).
  Vsnvs = 6,  ///< VSNVS — RTC / backup LDO (≤2 mA).
  COUNT = 7,
};

/**
 * @brief Severity classification for a fault.
 *
 * - @ref kInfo      — informational latched bit (e.g. PWRON press).
 * - @ref kWarning   — recoverable condition (e.g. junction temp warning).
 * - @ref kCritical  — affects a rail that is **not** powering the MCU
 *                     directly; the MCU can still execute and mitigate.
 * - @ref kMcuKill   — affects an MCU-critical rail (SW3/LDO1/LDO3 on
 *                     Portenta H7) — by definition the MCU may already be
 *                     resetting; an **external monitor** must take over.
 */
enum class FaultSeverity : uint8_t {
  kInfo     = 0,
  kWarning  = 1,
  kCritical = 2,
  kMcuKill  = 3,
};

/**
 * @brief Bitset of latched fault flags.
 *
 * Mirrors the union of all per-category STAT registers, normalised to a
 * single 32-bit value for IPC and logging. Layout is **stable** — never
 * reorder; append-only.
 */
struct FaultFlags {
  uint32_t value{0};

  // ---- Switching regulator (per-rail HS / LS current limits) ----
  static constexpr uint32_t kSw1Ls = 1u << 0;
  static constexpr uint32_t kSw1Hs = 1u << 1;
  static constexpr uint32_t kSw2Ls = 1u << 2;
  static constexpr uint32_t kSw2Hs = 1u << 3;
  static constexpr uint32_t kSw3Ls = 1u << 4;
  static constexpr uint32_t kSw3Hs = 1u << 5;

  // ---- LDO faults ----
  static constexpr uint32_t kLdo1Fault = 1u << 8;
  static constexpr uint32_t kLdo2Fault = 1u << 9;
  static constexpr uint32_t kLdo3Fault = 1u << 10;

  // ---- Temperature (TEMP_INT_STAT0, Table 102) ----
  static constexpr uint32_t kTempWarn = 1u << 12; ///< THERM110I: die crossed 110 °C.
  static constexpr uint32_t kTempShdn = 1u << 13; ///< THERM125I: die crossed 125 °C.

  // ---- Misc system events (MISC_INT_STAT0, Table 108) ----
  static constexpr uint32_t kPwrOn     = 1u << 16; ///< PWRON_I: power-on button event.
  static constexpr uint32_t kVsysLow   = 1u << 17; ///< LOW_SYS_WARN_I: VSYS crossed LOW_SYS_WARN.
  static constexpr uint32_t kVsysOv    = 1u << 18; ///< SYS_OVLO_I: VSYS crossed SYS_OVLO.
  static constexpr uint32_t kWdiAssert = 1u << 19; ///< Never set: the PF1550 has no WDI interrupt bit.
  static constexpr uint32_t kResetBmcu = 1u << 20; ///< Never set: the PF1550 has no RESETBMCU interrupt bit.

  // ---- Charger / VBUS ----
  static constexpr uint32_t kChgFault   = 1u << 24;
  static constexpr uint32_t kVbusInval  = 1u << 25;
  static constexpr uint32_t kBattOv     = 1u << 26;

  // ---- Driver-internal (I²C path) ----
  static constexpr uint32_t kI2cReadFail   = 1u << 30;
  static constexpr uint32_t kI2cWriteFail  = 1u << 31;

  [[nodiscard]] constexpr bool Any() const noexcept { return value != 0; }
  [[nodiscard]] constexpr bool Has(uint32_t mask) const noexcept { return (value & mask) != 0; }
  constexpr void Set(uint32_t mask) noexcept { value |= mask; }
  constexpr void Clear(uint32_t mask) noexcept { value &= ~mask; }
};

/**
 * @brief Per-rail status snapshot (latched + live + control byte).
 */
struct RailDiagnostics {
  uint8_t  ctrl_reg{0};      ///< Raw `SWn_CTRL` or `LDOn_CTRL` byte.
  uint8_t  volt_reg{0};      ///< Raw voltage register byte (RUN mode).
  uint16_t set_voltage_mv{0};///< Decoded target voltage in mV.
  bool     enabled_run{false};///< `EN` bit set.
  bool     enabled_stby{false};///< `STBY_EN` bit set.
  bool     stat_latched{false};///< Any latched fault on this rail since last clear.
  bool     sense_live{false}; ///< Live current-limit / fault sense bit set.
};

/**
 * @brief Full diagnostic snapshot returned by
 * `PF1550::ReadDiagnosticSnapshot()`.
 *
 * All fields are valid only if @ref read_ok is `true`.
 */
struct DiagnosticSnapshot {
  bool          read_ok{false};         ///< All required I²C reads succeeded.
  uint8_t       device_id{0};           ///< Reg 0x00 — must equal `0x7C`.
  uint8_t       silicon_rev{0};         ///< Reg 0x02.
  uint8_t       otp_flavor{0};          ///< Reg 0x01 — `0x00` for A0EP.
  uint8_t       int_category{0};        ///< Reg 0x06 — top-level interrupt summary.
  uint8_t       state_info_reg{0};      ///< Reg 0x67 raw.
  PmicState     state{PmicState::Unknown};      ///< Decoded STATE_INFO[5:0].
  uint8_t       chg_sense_reg{0};       ///< Reg 0x87 raw.
  ChargerState  charger{ChargerState::Unknown}; ///< Decoded CHG_SNS[3:0].
  uint8_t       vbus_sns_reg{0};        ///< Reg 0x86.
  uint16_t      vbus_in_limit_ma{0};    ///< Decoded VBUS input current limit (mA).
  FaultFlags    faults{};               ///< Aggregated latched + sense conditions.
  RailDiagnostics rails[static_cast<size_t>(RailId::COUNT)]{};
};

/**
 * @brief Result of a power self-test pass.
 *
 * Used by the boot recipe and the periodic monitor. `worst_severity` is
 * `kInfo` when everything looks healthy.
 */
struct SelfTestResult {
  bool          ran{false};
  bool          device_id_ok{false};
  bool          state_run{false};
  bool          all_rails_enabled{false};
  FaultSeverity worst_severity{FaultSeverity::kInfo};
  FaultFlags    faults{};                  ///< Sticky fault snapshot at the time of test.
  DiagnosticSnapshot snapshot{};
};

/**
 * @brief Map a single fault flag to the rail it relates to, or
 * @ref RailId::COUNT if the fault is system-wide.
 */
constexpr RailId FaultRail(uint32_t single_flag) noexcept {
  if (single_flag == FaultFlags::kSw1Ls || single_flag == FaultFlags::kSw1Hs) return RailId::Sw1;
  if (single_flag == FaultFlags::kSw2Ls || single_flag == FaultFlags::kSw2Hs) return RailId::Sw2;
  if (single_flag == FaultFlags::kSw3Ls || single_flag == FaultFlags::kSw3Hs) return RailId::Sw3;
  if (single_flag == FaultFlags::kLdo1Fault) return RailId::Ldo1;
  if (single_flag == FaultFlags::kLdo2Fault) return RailId::Ldo2;
  if (single_flag == FaultFlags::kLdo3Fault) return RailId::Ldo3;
  return RailId::COUNT;
}

/**
 * @brief Classify the severity of a *Portenta H7 eval* fault.
 *
 * On other platforms the same flag may have a different severity (e.g. a
 * board that runs the MCU from SW2 instead of SW3). Callers should provide
 * board-specific overrides if they re-use the driver for non-Portenta wiring.
 */
constexpr FaultSeverity FaultSeverityPortentaH7(uint32_t single_flag) noexcept {
  switch (single_flag) {
    // MCU is powered from SW3 → losing SW3 kills the MCU.
    case FaultFlags::kSw3Hs:
    case FaultFlags::kSw3Ls:
    // LDO1 (1.0 V) feeds the SOC core analog and MIPI bridge — losing it
    // crashes the platform.
    case FaultFlags::kLdo1Fault:
    // LDO3 (1.2 V) feeds Ethernet PHY VDDCR and DSI core.
    case FaultFlags::kLdo3Fault:
    case FaultFlags::kVsysLow:
      return FaultSeverity::kMcuKill;

    // SW1 (+3V1SW) feeds USB/ULPI, SDRAM, ETH PHY and LDO inputs — losing it
    // takes USB / network down but MCU can still run on SW3+SW3-derived
    // domains. It IS however necessary for LDO outputs.
    case FaultFlags::kSw1Hs:
    case FaultFlags::kSw1Ls:
    // SW2 feeds carrier +3V3 only; MCU keeps running.
    case FaultFlags::kSw2Hs:
    case FaultFlags::kSw2Ls:
    // LDO2 (1.8 V) for USB PHY — USB dies, MCU survives.
    case FaultFlags::kLdo2Fault:
    case FaultFlags::kTempShdn:
    case FaultFlags::kBattOv:
    case FaultFlags::kVsysOv:
      return FaultSeverity::kCritical;

    case FaultFlags::kTempWarn:
    case FaultFlags::kChgFault:
    case FaultFlags::kVbusInval:
    case FaultFlags::kWdiAssert:
    case FaultFlags::kI2cReadFail:
    case FaultFlags::kI2cWriteFail:
      return FaultSeverity::kWarning;

    case FaultFlags::kPwrOn:
    case FaultFlags::kResetBmcu:
    default:
      return FaultSeverity::kInfo;
  }
}

/**
 * @brief Return the worst-case severity across an aggregated bitmask.
 */
constexpr FaultSeverity WorstSeverityPortentaH7(FaultFlags fl) noexcept {
  FaultSeverity worst = FaultSeverity::kInfo;
  for (uint32_t bit = 0; bit < 32; ++bit) {
    const uint32_t mask = 1u << bit;
    if ((fl.value & mask) == 0) continue;
    const auto sev = FaultSeverityPortentaH7(mask);
    if (static_cast<uint8_t>(sev) > static_cast<uint8_t>(worst)) {
      worst = sev;
    }
  }
  return worst;
}

/// @brief Human-readable rail label (`"SW1"`, `"LDO2"`, …).
constexpr const char* RailName(RailId id) noexcept {
  switch (id) {
    case RailId::Sw1:   return "SW1";
    case RailId::Sw2:   return "SW2";
    case RailId::Sw3:   return "SW3";
    case RailId::Ldo1:  return "LDO1";
    case RailId::Ldo2:  return "LDO2";
    case RailId::Ldo3:  return "LDO3";
    case RailId::Vsnvs: return "VSNVS";
    default:            return "?";
  }
}

/// @brief Human-readable name for a PmicState enumerator.
constexpr const char* PmicStateName(PmicState s) noexcept {
  switch (s) {
    case PmicState::Wait:        return "Wait";
    case PmicState::Run:         return "Run";
    case PmicState::Standby:     return "Standby";
    case PmicState::Sleep:       return "Sleep";
    case PmicState::RegsDisable: return "RegsDisable";
    default:                     return "Unknown";
  }
}

/// @brief Human-readable name for a ChargerState enumerator.
constexpr const char* ChargerStateName(ChargerState s) noexcept {
  switch (s) {
    case ChargerState::PreCharge:    return "PreCharge";
    case ChargerState::FastCC:       return "FastCC";
    case ChargerState::FastCV:       return "FastCV";
    case ChargerState::EndOfCharge:  return "EndOfCharge";
    case ChargerState::Done:         return "Done";
    case ChargerState::TimerFault:   return "TimerFault";
    case ChargerState::ThermSuspend: return "ThermSuspend";
    case ChargerState::Off:          return "Off";
    case ChargerState::BatteryOv:    return "BatteryOv";
    case ChargerState::TempShutdown: return "TempShutdown";
    case ChargerState::LinearOnly:   return "LinearOnly";
    default:                         return "Unknown";
  }
}

/// @brief Human-readable name for a FaultSeverity.
constexpr const char* FaultSeverityName(FaultSeverity s) noexcept {
  switch (s) {
    case FaultSeverity::kInfo:     return "info";
    case FaultSeverity::kWarning:  return "warn";
    case FaultSeverity::kCritical: return "critical";
    case FaultSeverity::kMcuKill:  return "mcu-kill";
    default:                       return "?";
  }
}

} // namespace pf1550
