# Register map

The PF1550 registers the driver names in `pf1550::Register`, with their data-sheet names, and the interrupt bit
layout the fault reader depends on.

Addresses are absolute: the PMIC page is 0x00 to 0x7F and the charger page starts at 0x80 (`kChargerPageBase`), so
the data sheet's charger register CHG_SNS, offset 0x07, is 0x87 here. Table numbers refer to the
[PF1550 data sheet](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf), Rev. 7, section 12. Any register can be
read or written with `ReadRegister()` / `WriteRegister()`.

## Registers

<!-- generated: register-map -->
| Address, `Register::` | Data sheet name and contents |
| --- | --- |
| `0x00` `DeviceId` | DEVICE_ID — Table 86. Bits[2:0] = family, [7:3] = `01111` for PF1550. |
| `0x01` `OtpFlavor` | OTP_FLAVOR — Table 87. 0x00 = OTP not burned (A0EP). |
| `0x02` `SiliconRev` | SILICON_REV — Table 88. Metal/full-layer/fab-fin identifiers. |
| `0x06` `IntCategory` | INT_CATEGORY — Table 89. CHG/SW1/SW2/SW3/LDO/ONKEY/TEMP/MISC summary. |
| `0x08` `SwIntStat0` | SW_INT_STAT0 — low-side current limits (SW1/2/3). |
| `0x09` `SwIntMask0` | SW_INT_MASK0 |
| `0x0A` `SwIntSense0` | SW_INT_SENSE0 |
| `0x0B` `SwIntStat1` | SW_INT_STAT1 — high-side current limits. |
| `0x0C` `SwIntMask1` | SW_INT_MASK1 |
| `0x0D` `SwIntSense1` | SW_INT_SENSE1 |
| `0x0E` `SwIntStat2` | SW_INT_STAT2 — DVS done. |
| `0x0F` `SwIntMask2` | SW_INT_MASK2 |
| `0x10` `SwIntSense2` | SW_INT_SENSE2 |
| `0x18` `LdoIntStat0` | LDO_INT_STAT0 — Table 99. LDO1/2/3 current-limit faults. |
| `0x19` `LdoIntMask0` | LDO_INT_MASK0 |
| `0x1A` `LdoIntSense0` | LDO_INT_SENSE0 |
| `0x20` `TempIntStat0` | TEMP_INT_STAT0 — Table 102. Junction-temperature events. |
| `0x21` `TempIntMask0` | TEMP_INT_MASK0 |
| `0x22` `TempIntSense0` | TEMP_INT_SENSE0 |
| `0x24` `OnKeyIntStat0` | ONKEY_INT_STAT0 — Table 105. Push / 1s/2s/3s/4s/8s long-press. |
| `0x25` `OnKeyIntMask0` | ONKEY_INT_MASK0 |
| `0x26` `OnKeyIntSense0` | ONKEY_INT_SENSE0 |
| `0x28` `MiscIntStat0` | MISC_INT_STAT0 — Table 108. PWRON / VSYS / RESETBMCU events. |
| `0x29` `MiscIntMask0` | MISC_INT_MASK0 |
| `0x2A` `MiscIntSense0` | MISC_INT_SENSE0 |
| `0x30` `CoinCellCtrl` | COINCELL_CONTROL — Table 111. LICELL charging configuration. |
| `0x32` `Sw1Volt` | SW1_VOLT — RUN-mode voltage code (Table 31). |
| `0x33` `Sw1StbyVolt` | SW1_STBY_VOLT — STANDBY-mode voltage. |
| `0x34` `Sw1SlpVolt` | SW1_SLP_VOLT — SLEEP-mode voltage. |
| `0x35` `Sw1Ctrl` | SW1_CTRL — EN/STBY_EN/OMODE/LPWR/DVSSPEED/FPWM/RDIS_ENB. |
| `0x36` `Sw1Ctrl1` | SW1_CTRL1 — SW1_ILIM[1:0] current limit + TMODE_SEL. |
| `0x38` `Sw2Volt` | SW2_VOLT — Table 117. |
| `0x39` `Sw2StbyVolt` | SW2_STBY_VOLT |
| `0x3A` `Sw2SlpVolt` | SW2_SLP_VOLT |
| `0x3B` `Sw2Ctrl` | SW2_CTRL — Table 120. |
| `0x3C` `Sw2Ctrl1` | SW2_CTRL1 — SW2_ILIM[1:0]. |
| `0x3E` `Sw3Volt` | SW3_VOLT — Table 122. |
| `0x3F` `Sw3StbyVolt` | SW3_STBY_VOLT |
| `0x40` `Sw3SlpVolt` | SW3_SLP_VOLT |
| `0x41` `Sw3Ctrl` | SW3_CTRL |
| `0x42` `Sw3Ctrl1` | SW3_CTRL1 — Table 126. SW3_ILIM[1:0] + SW3_TMODE_SEL (Portenta writes `0x02` = 1.5 A typical). |
| `0x48` `VsnvsCtrl` | VSNVS_CTRL — Table 127. RTC/SNVS backup LDO control. |
| `0x4A` `VrefDdrCtrl` | VREFDDR_CTRL — Table 128. DDR reference (unused on Portenta). |
| `0x4C` `Ldo1Volt` | LDO1_VOLT |
| `0x4D` `Ldo1Ctrl` | LDO1_CTRL |
| `0x4F` `Ldo2Volt` | LDO2_VOLT |
| `0x50` `Ldo2Ctrl` | LDO2_CTRL — must be written last in Portenta profile. |
| `0x52` `Ldo3Volt` | LDO3_VOLT |
| `0x53` `Ldo3Ctrl` | LDO3_CTRL |
| `0x58` `PwrCtrl0` | PWRCTRL0 — STANDBYDLY, STANDBYINV, POR_DLY, TGRESET. |
| `0x59` `PwrCtrl1` | PWRCTRL1 — PWRONDBNC, ONKEYDBNC, PWRONRSTEN, RESTARTEN, REGSCPEN. |
| `0x5A` `PwrCtrl2` | PWRCTRL2 — WDI_EN, WDI_POL etc. |
| `0x5B` `PwrCtrl3` | PWRCTRL3 — sequence delay etc. |
| `0x5F` `Sw1PwrDnSeq` | SW1_PWRDN_SEQ |
| `0x60` `Sw2PwrDnSeq` | SW2_PWRDN_SEQ |
| `0x61` `Sw3PwrDnSeq` | SW3_PWRDN_SEQ |
| `0x62` `Ldo1PwrDnSeq` | LDO1_PWRDN_SEQ |
| `0x63` `Ldo2PwrDnSeq` | LDO2_PWRDN_SEQ |
| `0x64` `Ldo3PwrDnSeq` | LDO3_PWRDN_SEQ |
| `0x65` `VrefDdrPwrDnSeq` | VREFDDR_PWRDN_SEQ |
| `0x67` `StateInfo` | STATE_INFO — Table 146. STATE[5:0] = Wait/RUN/STANDBY/SLEEP/REGS_DISABLE. |
| `0x68` `I2cAddrReg` | I2C_ADDR — Table 147 (read-only, OTP-driven). |
| `0x6B` `Rc16Mhz` | RC_16MHZ — 16 MHz clock force, analog core overrides. |
| `0x6F` `OtpKey1` | KEY1 unlock (write `0x15`). |
| `0x80` `ChgInt` | CHG_INT — Table 150. SUP_I/BAT2SOC_I/CHGREV_I/CHG_I. |
| `0x82` `ChgIntMask` | CHG_INT_MASK |
| `0x84` `ChgIntOk` | CHG_INT_OK — Table 152. CHG_OK, BATT_OK, VBUS_OK. |
| `0x86` `VbusSns` | VBUS_SNS — Table 153. VBUS_VALID, DPM_SNS, OVLO, UVLO. |
| `0x87` `ChgSense` | CHG_SNS — Table 154. Charger mode (precharge, fast-cc, fast-cv, eoc …). |
| `0x94` `VbusInCurrentLimit` | VBUS input current limit ((mA/50) << 3 — Portenta uses 1500 mA → `0xA0`). |
| `0x9C` `ChargerLedDuty` | LED_PWM — Table 170. LED_EN, LED_RAMP, LED_PWM[5:0] duty (Portenta writes `0x80`). |
| `0x9E` `ChargerLedCtrl` | LED_CNFG — Table 172. LEDOVRD, LED_CFG, current, frequency (Portenta writes `0x20`). |
| `0x9F` `OtpKey2` | KEY2 unlock (write `0x50`). |
| `0xC4` `FmrAddr` | FMRADDR — OTP indirect address. |
| `0xC5` `FmrData` | FRMDATA — OTP indirect data. |
| `0xDF` `TestRegKey3` | TEST_REG_KEY3 — third OTP unlock key (write `0xAB`). |

Source of this table: [`inc/pf1550_registers.hpp`](../inc/pf1550_registers.hpp).
<!-- /generated -->

## Interrupt registers

Each interrupt source has three registers: STAT (latched, write 1 to clear), MASK (1 = masked, the reset value)
and SENSE (the live state). INT_CATEGORY (0x06) has one bit per group and is the cheap first read.

<div class="hf-registers" markdown>

| Register | Bits, from bit 0 | Meaning |
| --- | --- | --- |
| `INT_CATEGORY` (0x06) | `CHG_INT`, `SW1_INT`, `SW2_INT`, `SW3_INT`, `LDO_INT`, `ONKEY_INT`, `TEMP_INT`, `MISC_INT` | A group has a pending interrupt |
| `SW_INT_STAT0` (0x08) | `SW1_LS_I`, `SW2_LS_I`, `SW3_LS_I` | Low-side current limit |
| `SW_INT_STAT1` (0x0B) | `SW1_HS_I`, `SW2_HS_I`, `SW3_HS_I` | High-side current limit |
| `SW_INT_STAT2` (0x0E) | `SW1_DVS_DONE_I`, `SW2_DVS_DONE_I` | DVS transition finished |
| `LDO_INT_STAT0` (0x18) | `LDO1_FAULTI`, `LDO2_FAULTI`, `LDO3_FAULTI` | LDO fault |
| `TEMP_INT_STAT0` (0x20) | `THERM110I`, unused, `THERM125I` | Die temperature crossed 110 °C / 125 °C |
| `MISC_INT_STAT0` (0x28) | `PWRUP_I`, `PWRDN_I`, `PWRON_I`, `LOW_SYS_WARN_I`, `SYS_OVLO_I` | Back in RUN, entered STANDBY / SLEEP, PWRON button, VSYS below LOW_SYS_WARN, VSYS over SYS_OVLO |

</div>

`ReadLatchedFaults()` maps these bits into `FaultFlags`: `SW1/2/3_LS_I` and `_HS_I` to `kSwNLs` / `kSwNHs`,
`LDOn_FAULTI` to `kLdoNFault`, `THERM110I` to `kTempWarn`, `THERM125I` to `kTempShdn`, `PWRON_I` to `kPwrOn`,
`LOW_SYS_WARN_I` to `kVsysLow` and `SYS_OVLO_I` to `kVsysOv`. `PWRUP_I` and `PWRDN_I` are mode-transition events and
are not mapped. The SENSE registers use the same layout as their STAT registers.

## The plain-text extract

[`docs/datasheet/PF1550-i2c-register-reference.md`](https://github.com/hardfoc/hf-pf1550-driver/blob/main/docs/datasheet/PF1550-i2c-register-reference.md)
is section 12 of the data sheet as text, regenerated from a local copy of the PDF with
`scripts/extract_register_reference.sh`. It is kept in the repository for searching and is not part of this site.

Next: [Portenta H7 profiles](portenta-h7.md).
