/**
 * @file pf1550_registers.hpp
 * @brief NXP PF1550 family I2C register map, bit-field constants, and OTP keys.
 *
 * Covers MC34PF1550*, MC32PF1550*, and the unprogrammed-OTP variant
 * `MC34PF1550A0EP` used on Arduino Portenta H7 eval modules.
 *
 * Register addresses follow the NXP PF1550 datasheet (Rev. 7,
 * September 2021) Section 12 *Register map*:
 *
 *  - **12.1** *Specific PMIC registers* — base I2C offset `0x00`
 *    (DEVICE_ID, interrupts, regulators, power control).
 *  - **12.2** *Specific Charger registers* — base I2C offset `0x80`
 *    (CHG_INT, VBUS_SNS, CHG_SNS …).
 *
 * Per-bit details are intentionally kept compact here; consult
 * `docs/datasheet/PF1550-i2c-register-reference.md` (auto-generated extract
 * of Section 12) for the authoritative bit semantics.
 *
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 *
 * @par Versioning
 *   Driver version exposed via @ref HF_PF1550_VERSION (see pf1550_version.h).
 */
#pragma once

#include <cstdint>

namespace pf1550 {

/// @brief Default 7-bit I²C slave address (settable via OTP — see Table 147).
inline constexpr uint8_t kDefaultI2cAddress = 0x08;

/// @brief Expected `DEVICE_ID[2:0]` family identifier (PF1550 family = `100b`).
inline constexpr uint8_t kExpectedDeviceId = 0x7C;

/// @brief Base offset for Section 12.2 charger sub-page (see datasheet §12.2).
inline constexpr uint8_t kChargerPageBase = 0x80;

/**
 * @enum Register
 * @brief Symbolic register addresses used by the PF1550 driver.
 *
 * Names follow datasheet *Table N — Register XXX - ADDR yy* exactly so
 * cross-reference is trivial. Charger-page registers (§12.2) are mapped to
 * their @ref kChargerPageBase-offset absolute addresses.
 */
enum class Register : uint8_t {
  // --- §12.1 — identity & metadata ---
  DeviceId       = 0x00,  ///< DEVICE_ID — Table 86. Bits[2:0] = family, [7:3] = `01111` for PF1550.
  OtpFlavor      = 0x01,  ///< OTP_FLAVOR — Table 87. 0x00 = OTP not burned (A0EP).
  SiliconRev     = 0x02,  ///< SILICON_REV — Table 88. Metal/full-layer/fab-fin identifiers.

  // --- §12.1 — top-level interrupt category mask ---
  IntCategory    = 0x06,  ///< INT_CATEGORY — Table 89. CHG/SW1/SW2/SW3/LDO/ONKEY/TEMP/MISC summary.

  // --- §12.1 — switching regulator interrupt status / mask / sense ---
  SwIntStat0     = 0x08,  ///< SW_INT_STAT0 — low-side current limits (SW1/2/3).
  SwIntMask0     = 0x09,  ///< SW_INT_MASK0
  SwIntSense0    = 0x0A,  ///< SW_INT_SENSE0
  SwIntStat1     = 0x0B,  ///< SW_INT_STAT1 — high-side current limits.
  SwIntMask1     = 0x0C,  ///< SW_INT_MASK1
  SwIntSense1    = 0x0D,  ///< SW_INT_SENSE1
  SwIntStat2     = 0x0E,  ///< SW_INT_STAT2 — DVS done.
  SwIntMask2     = 0x0F,  ///< SW_INT_MASK2
  SwIntSense2    = 0x10,  ///< SW_INT_SENSE2

  // --- §12.1 — LDO fault / temperature / ONKEY / MISC ---
  LdoIntStat0    = 0x18,  ///< LDO_INT_STAT0 — Table 99. LDO1/2/3 current-limit faults.
  LdoIntMask0    = 0x19,  ///< LDO_INT_MASK0
  LdoIntSense0   = 0x1A,  ///< LDO_INT_SENSE0

  TempIntStat0   = 0x20,  ///< TEMP_INT_STAT0 — Table 102. Junction-temperature events.
  TempIntMask0   = 0x21,  ///< TEMP_INT_MASK0
  TempIntSense0  = 0x22,  ///< TEMP_INT_SENSE0

  OnKeyIntStat0  = 0x24,  ///< ONKEY_INT_STAT0 — Table 105. Push / 1s/2s/3s/4s/8s long-press.
  OnKeyIntMask0  = 0x25,  ///< ONKEY_INT_MASK0
  OnKeyIntSense0 = 0x26,  ///< ONKEY_INT_SENSE0

  MiscIntStat0   = 0x28,  ///< MISC_INT_STAT0 — Table 108. PWRON / VSYS / RESETBMCU events.
  MiscIntMask0   = 0x29,  ///< MISC_INT_MASK0
  MiscIntSense0  = 0x2A,  ///< MISC_INT_SENSE0

  CoinCellCtrl   = 0x30,  ///< COINCELL_CONTROL — Table 111. LICELL charging configuration.

  // --- §12.1 — Switch 1 (SW1 / BUCK1) ---
  Sw1Volt        = 0x32,  ///< SW1_VOLT — RUN-mode voltage code (Table 31).
  Sw1StbyVolt    = 0x33,  ///< SW1_STBY_VOLT — STANDBY-mode voltage.
  Sw1SlpVolt     = 0x34,  ///< SW1_SLP_VOLT — SLEEP-mode voltage.
  Sw1Ctrl        = 0x35,  ///< SW1_CTRL — EN/STBY_EN/OMODE/LPWR/DVSSPEED/FPWM/RDIS_ENB.
  Sw1Ctrl1       = 0x36,  ///< SW1_CTRL1 — SW1_ILIM[1:0] current limit + TMODE_SEL.

  // --- §12.1 — Switch 2 (SW2 / BUCK2) ---
  Sw2Volt        = 0x38,  ///< SW2_VOLT — Table 117.
  Sw2StbyVolt    = 0x39,  ///< SW2_STBY_VOLT
  Sw2SlpVolt     = 0x3A,  ///< SW2_SLP_VOLT
  Sw2Ctrl        = 0x3B,  ///< SW2_CTRL — Table 120.
  Sw2Ctrl1       = 0x3C,  ///< SW2_CTRL1 — SW2_ILIM[1:0].

  // --- §12.1 — Switch 3 (SW3 / BUCK3) ---
  Sw3Volt        = 0x3E,  ///< SW3_VOLT — Table 122.
  Sw3StbyVolt    = 0x3F,  ///< SW3_STBY_VOLT
  Sw3SlpVolt     = 0x40,  ///< SW3_SLP_VOLT
  Sw3Ctrl        = 0x41,  ///< SW3_CTRL
  Sw3Ctrl1       = 0x42,  ///< SW3_CTRL1 — Table 126. SW3_ILIM[1:0] + SW3_TMODE_SEL (Portenta writes `0x02` = 1.5 A typical).

  // --- §12.1 — VSNVS / VREFDDR ---
  VsnvsCtrl      = 0x48,  ///< VSNVS_CTRL — Table 127. RTC/SNVS backup LDO control.
  VrefDdrCtrl    = 0x4A,  ///< VREFDDR_CTRL — Table 128. DDR reference (unused on Portenta).

  // --- §12.1 — LDOs ---
  Ldo1Volt       = 0x4C,  ///< LDO1_VOLT
  Ldo1Ctrl       = 0x4D,  ///< LDO1_CTRL
  Ldo2Volt       = 0x4F,  ///< LDO2_VOLT
  Ldo2Ctrl       = 0x50,  ///< LDO2_CTRL — **must be written last** in Portenta profile.
  Ldo3Volt       = 0x52,  ///< LDO3_VOLT
  Ldo3Ctrl       = 0x53,  ///< LDO3_CTRL

  // --- §12.1 — Power-control / debounce ---
  PwrCtrl0       = 0x58,  ///< PWRCTRL0 — STANDBYDLY, STANDBYINV, POR_DLY, TGRESET.
  PwrCtrl1       = 0x59,  ///< PWRCTRL1 — PWRONDBNC, ONKEYDBNC, PWRONRSTEN, RESTARTEN, REGSCPEN.
  PwrCtrl2       = 0x5A,  ///< PWRCTRL2 — WDI_EN, WDI_POL etc.
  PwrCtrl3       = 0x5B,  ///< PWRCTRL3 — sequence delay etc.

  // --- §12.1 — Power-down sequencing ---
  Sw1PwrDnSeq    = 0x5F,  ///< SW1_PWRDN_SEQ
  Sw2PwrDnSeq    = 0x60,  ///< SW2_PWRDN_SEQ
  Sw3PwrDnSeq    = 0x61,  ///< SW3_PWRDN_SEQ
  Ldo1PwrDnSeq   = 0x62,  ///< LDO1_PWRDN_SEQ
  Ldo2PwrDnSeq   = 0x63,  ///< LDO2_PWRDN_SEQ
  Ldo3PwrDnSeq   = 0x64,  ///< LDO3_PWRDN_SEQ
  VrefDdrPwrDnSeq = 0x65, ///< VREFDDR_PWRDN_SEQ

  // --- §12.1 — Machine state / addressing / clock ---
  StateInfo      = 0x67,  ///< STATE_INFO — Table 146. STATE[5:0] = Wait/RUN/STANDBY/SLEEP/REGS_DISABLE.
  I2cAddrReg     = 0x68,  ///< I2C_ADDR — Table 147 (read-only, OTP-driven).
  Rc16Mhz        = 0x6B,  ///< RC_16MHZ — 16 MHz clock force, analog core overrides.

  // --- §12.1 — OTP indirect read ---
  OtpKey1        = 0x6F,  ///< KEY1 unlock (write `0x15`).
  ChargerLedDuty = 0x9C,  ///< LED_PWM — Table 170. LED_EN, LED_RAMP, LED_PWM[5:0] duty (Portenta writes `0x80`).
  OtpKey2        = 0x9F,  ///< KEY2 unlock (write `0x50`).
  ChargerLedCtrl = 0x9E,  ///< LED_CNFG — Table 172. LEDOVRD, LED_CFG, current, frequency (Portenta writes `0x20`).
  VbusInCurrentLimit = 0x94, ///< VBUS input current limit ((mA/50) << 3 — Portenta uses 1500 mA → `0xA0`).
  FmrAddr        = 0xC4,  ///< FMRADDR — OTP indirect address.
  FmrData        = 0xC5,  ///< FRMDATA — OTP indirect data.
  TestRegKey3    = 0xDF,  ///< TEST_REG_KEY3 — third OTP unlock key (write `0xAB`).

  // --- §12.2 — Charger sub-page (offset 0x80) ---
  ChgInt         = 0x80,  ///< CHG_INT — Table 150. SUP_I/BAT2SOC_I/CHGREV_I/CHG_I.
  ChgIntMask     = 0x82,  ///< CHG_INT_MASK
  ChgIntOk       = 0x84,  ///< CHG_INT_OK — Table 152. CHG_OK, BATT_OK, VBUS_OK.
  VbusSns        = 0x86,  ///< VBUS_SNS — Table 153. VBUS_VALID, DPM_SNS, OVLO, UVLO.
  ChgSense       = 0x87,  ///< CHG_SNS — Table 154. Charger mode (precharge, fast-cc, fast-cv, eoc …).
};

/**
 * @brief Bit positions within INT_CATEGORY (Reg 0x06).
 */
struct IntCategoryBits {
  static constexpr uint8_t kChg   = 1u << 0; ///< CHG_INT — any charger interrupt pending.
  static constexpr uint8_t kSw1   = 1u << 1; ///< SW1_INT — SW1 fault interrupt.
  static constexpr uint8_t kSw2   = 1u << 2; ///< SW2_INT — SW2 fault interrupt.
  static constexpr uint8_t kSw3   = 1u << 3; ///< SW3_INT — SW3 fault interrupt.
  static constexpr uint8_t kLdo   = 1u << 4; ///< LDO_INT — any LDO fault.
  static constexpr uint8_t kOnKey = 1u << 5; ///< ONKEY_INT — push-button events.
  static constexpr uint8_t kTemp  = 1u << 6; ///< TEMP_INT — die temperature events.
  static constexpr uint8_t kMisc  = 1u << 7; ///< MISC_INT — PWRON / RESETBMCU / VSYS / WDI.
};

/**
 * @brief Bit positions within SW_INT_STAT0 (Reg 0x08) — **low-side** current-limit faults.
 *
 * STAT bits are RW1C: read 1 = event latched, write 1 to clear.
 * The corresponding SW_INT_SENSE0 (0x0A) shows the *instantaneous* (live) state.
 */
struct SwIntStat0Bits {
  static constexpr uint8_t kSw1Ls = 1u << 0; ///< SW1 low-side current limit hit.
  static constexpr uint8_t kSw2Ls = 1u << 2; ///< SW2 low-side current limit hit.
  static constexpr uint8_t kSw3Ls = 1u << 4; ///< SW3 low-side current limit hit.
};

/// @brief Bit positions within SW_INT_STAT1 (Reg 0x0B) — **high-side** current-limit faults.
struct SwIntStat1Bits {
  static constexpr uint8_t kSw1Hs = 1u << 0; ///< SW1 high-side current limit hit (over-current).
  static constexpr uint8_t kSw2Hs = 1u << 2; ///< SW2 high-side current limit hit.
  static constexpr uint8_t kSw3Hs = 1u << 4; ///< SW3 high-side current limit hit.
};

/// @brief Bit positions within LDO_INT_STAT0 (Reg 0x18) — LDO current-limit faults.
struct LdoIntStat0Bits {
  static constexpr uint8_t kLdo1Fault = 1u << 0; ///< LDO1 fault.
  static constexpr uint8_t kLdo2Fault = 1u << 1; ///< LDO2 fault.
  static constexpr uint8_t kLdo3Fault = 1u << 2; ///< LDO3 fault.
};

/// @brief Bit positions within TEMP_INT_STAT0 (Reg 0x20).
struct TempIntStat0Bits {
  static constexpr uint8_t kTempWarn = 1u << 0; ///< Junction temperature warning.
  static constexpr uint8_t kTempShdn = 1u << 1; ///< Junction temperature thermal shutdown asserted.
};

/// @brief Bit positions within MISC_INT_STAT0 (Reg 0x28).
struct MiscIntStat0Bits {
  static constexpr uint8_t kPwrOn      = 1u << 0; ///< PWRON pin event.
  static constexpr uint8_t kVsysLow    = 1u << 1; ///< VSYS undervoltage / brown-out.
  static constexpr uint8_t kVsysOv     = 1u << 2; ///< VSYS overvoltage.
  static constexpr uint8_t kWdiAssert  = 1u << 3; ///< WDI (watchdog input) low event.
  static constexpr uint8_t kResetBmcu  = 1u << 4; ///< RESETBMCU event.
};

/// @brief STATE_INFO[5:0] values (Reg 0x67) describing the PMIC state machine.
enum class PmicState : uint8_t {
  Wait        = 0b000000, ///< Wait / start-up before regulators are sequenced.
  Run         = 0b001100, ///< RUN — all OTP-sequenced regulators enabled.
  Standby     = 0b001101, ///< STANDBY — standby-enabled regulators only.
  Sleep       = 0b001110, ///< SLEEP / LPSR — minimal regulators.
  RegsDisable = 0b101011, ///< REGS_DISABLE — all regulators off (post PWRON long-press).
  Unknown     = 0xFF,
};

/// @brief CHG_SNS[3:0] values (Reg 0x87) describing the charger sub-state.
enum class ChargerState : uint8_t {
  PreCharge       = 0,   ///< Precharge mode (V_BATT < V_PRECHG.LB).
  FastCC          = 1,   ///< Fast-charge constant-current.
  FastCV          = 2,   ///< Fast-charge constant-voltage.
  EndOfCharge     = 3,   ///< End-of-charge (I_BAT = I_EOC).
  Done            = 4,   ///< Done — battery full.
  TimerFault      = 6,   ///< Timer fault.
  ThermSuspend    = 7,   ///< Thermistor suspend.
  Off             = 8,   ///< Input invalid or charger disabled.
  BatteryOv       = 9,   ///< Battery overvoltage.
  TempShutdown    = 10,  ///< Off because TJ > TSHDN.
  LinearOnly      = 12,  ///< Linear-only mode, no charge.
  Unknown         = 0xFF,
};

/**
 * @brief Common SW buck output voltage codes (RUN mode).
 *
 * Full encoding is `code → V_out` per datasheet Table 31:
 *  - DVS disabled (`OTP_SWx_DVS_SEL = 1`): 1.10, 1.20, 1.35, 1.50, 1.80, 2.50,
 *    3.00 V for codes 0..6, and 3.30 V for every code from 7 up.
 *  - DVS enabled (`OTP_SWx_DVS_SEL = 0`): 0.6000 V + 12.5 mV × code.
 *  These enumerators are DVS-disabled codes; SW3 uses Table 37 instead.
 *
 * Only the codes used on Arduino Portenta H7 eval are enumerated.
 *
 * @note The reverse mapping (code → mV) is also provided as a free function in
 *       @ref pf1550_voltage_tables.hpp for diagnostics.
 */
enum class SwVoltageCode : uint8_t {
  V1_1  = 0x00, ///< 1.100 V (datasheet base)
  V2_5  = 0x05, ///< 2.500 V (Portenta STBY/SLP setting)
  V3_0  = 0x06, ///< 3.000 V (SW1 RUN on Portenta carrier profile)
  V3_3  = 0x07, ///< 3.300 V (SW2 RUN on Portenta — carrier +3V3)
  V3_1  = 0x0D, ///< SW3 Table 37 code for 3.100 V (SW3 is OTP-loaded, read-only; SW1/SW2 decode 0x0D as 3.30 V)
};

/// @brief Common LDO output voltage codes used on Portenta H7 (Table 41).
enum class LdoVoltageCode : uint8_t {
  V1_0 = 0x05, ///< 1.000 V (LDO1)
  V1_2 = 0x09, ///< 1.200 V (LDO3)
  V1_8 = 0x00, ///< 1.800 V (LDO2 — code 0)
};

/**
 * @brief Bit positions within `SWn_CTRL` (e.g. 0x35 / 0x3B / 0x41).
 *
 * Used so callers can build a control byte without magic numbers:
 *
 * @code{.cpp}
 * uint8_t ctrl = pf1550::SwCtrlBits::kEn |
 *                pf1550::SwCtrlBits::kStbyEn |
 *                pf1550::SwCtrlBits::kOMode |
 *                pf1550::SwCtrlBits::kLpwr;  // 0x0F — full carrier-friendly enable.
 * @endcode
 */
struct SwCtrlBits {
  static constexpr uint8_t kEn         = 1u << 0; ///< SW_EN — enable in RUN.
  static constexpr uint8_t kStbyEn     = 1u << 1; ///< SW_STBY_EN — enable in STANDBY.
  static constexpr uint8_t kOMode      = 1u << 2; ///< SW_OMODE — enable in SLEEP.
  static constexpr uint8_t kLpwr       = 1u << 3; ///< SW_LPWR — low-power during STBY/SLP.
  static constexpr uint8_t kDvsSpeed   = 1u << 4; ///< SW_DVSSPEED — slow DVS slew.
  static constexpr uint8_t kFpwmInDvs  = 1u << 5; ///< SW_FPWM_IN_DVS — CCM during DVS-down.
  static constexpr uint8_t kFpwm       = 1u << 6; ///< SW_FPWM — force CCM.
  static constexpr uint8_t kRdisEnb    = 1u << 7; ///< SW_RDIS_ENB — disable discharge resistor.
};

/**
 * @brief Bit positions within `LDOn_CTRL` (0x4D / 0x50 / 0x53).
 */
struct LdoCtrlBits {
  static constexpr uint8_t kEn       = 1u << 0; ///< VLDO_EN — enable in RUN.
  static constexpr uint8_t kStbyEn   = 1u << 1; ///< VLDO_STBY_EN — enable in STANDBY.
  static constexpr uint8_t kOMode    = 1u << 2; ///< VLDO_OMODE — enable in SLEEP.
  static constexpr uint8_t kLpwr     = 1u << 3; ///< VLDO_LPWR — low-power in STBY/SLP.
  static constexpr uint8_t kLsEn     = 1u << 4; ///< LDOn_LS_EN — load-switch mode (irreversible 0→1).
};

/// @brief OTP indirect-read unlock sequence (Section 12.x KEY1 / KEY2 / KEY3).
struct OtpUnlockKeys {
  static constexpr uint8_t kKey1Reg = static_cast<uint8_t>(Register::OtpKey1);
  static constexpr uint8_t kKey1Val = 0x15;
  static constexpr uint8_t kKey2Reg = static_cast<uint8_t>(Register::OtpKey2);
  static constexpr uint8_t kKey2Val = 0x50;
  static constexpr uint8_t kKey3Reg = static_cast<uint8_t>(Register::TestRegKey3);
  static constexpr uint8_t kKey3Val = 0xAB;
};

/// @brief OTP memory indirect address range (via FMRADDR/FMRDATA).
struct OtpRegion {
  static constexpr uint8_t kStart        = 0x1C; ///< First OTP byte address.
  static constexpr uint8_t kEndExclusive = 0x37; ///< One past the last OTP byte address.
};

} // namespace pf1550
