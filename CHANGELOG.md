# Changelog

All notable changes to this driver are listed here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [Unreleased]

### Fixed

- **Breaking (behaviour):** the fault decoder reads `SW_INT_STAT0/1` (and their SENSE registers),
  `TEMP_INT_STAT0` and `MISC_INT_STAT0` at the data sheet's bit positions (Tables 90, 93, 102, 108). A RUN to
  STANDBY transition no longer reports `kVsysLow` / `kMcuKill`; SW2, SW3 and the 125 °C flag now come from the
  right bits. `SwIntStat0Bits`, `SwIntStat1Bits`, `TempIntStat0Bits::kTempShdn` and `MiscIntStat0Bits` changed
  value; `MiscIntStat0Bits::kWdiAssert` / `kResetBmcu` are deprecated and 0 (no such bits), and
  `FaultFlags::kWdiAssert` / `kResetBmcu` are never set.
- **Breaking (behaviour):** `SetSwVoltage()` writes only a DVS-enabled SW1 / SW2 (declared with
  `SetSwDvsEnabled()`), converting the code's voltage to the DVS encoding (Table 31); it refuses SW3 and
  DVS-disabled bucks, whose voltage registers are read-only (Tables 31, 37), and voltages above 1.3875 V.
- **Breaking (behaviour):** the Portenta H7 profiles no longer write the read-only `SWx_VOLT`, `SWx_STBY_VOLT` and
  `SWx_SLP_VOLT` registers; each table has 14 writes (the array type changed from 23 to 14 entries).
- `VbusLimitRegToMilliamps()` returns 0 for the reserved codes 21 to 31 (Table 58) instead of 1600 to 1800 mA.
- The diagnostic snapshot fills the VSNVS entry (VSNVS_CTRL byte, 3000 mV, always on; Table 127).
- `RunPowerSelfTest()` passes on battery: `kVbusInval` ranks `kInfo` in `FaultSeverityPortentaH7()`.
- `cmake --install` installs `src/pf1550.ipp`; headers now go to `include/hf_pf1550/inc/` (the exported target
  points there).
- The ESP32 example's `PF1550_I2C_FREQ_HZ` is limited to 400 kHz (fast mode).

### Added

- `SetSwVoltageMv()` and `SwDvsMillivoltsToCode()` for DVS-enabled SW1 / SW2; `TempIntStat0Bits::kTherm110` /
  `kTherm125`, `MiscIntStat0Bits::kPwrUp`, `kPwrDn`, `kLowSysWarn`, `kSysOvlo`; `kVsnvsMillivolts`.
- Host tests for the data sheet conformance fixes and an install check.

- Host unit tests (`-DHF_PF1550_BUILD_TESTS=ON`) against a register model of the PF1550, and a CI workflow that
  runs them with GCC and Clang.

### Changed

- Documentation moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, architecture, rails and
  OTP, register map, Portenta H7 profiles, API guide, porting, external provisioning, testing), code snippets
  compiled by the host build from `tests/docs/`, Doxygen under `/api/`, versioned with mike. The Jekyll site files
  and the doxygen-awesome submodule are gone. The curated register list moved from `docs/datasheet/` to the
  Register map page.

## 0.1.0 (version in the build settings; not tagged)

- PF1550 driver over a CRTP bus interface: identification, register access, Portenta H7 default and carrier
  profiles, rail set points, VBUS input limit, strap pins, indirect OTP reads, interrupt masking, diagnostic
  snapshot and boot self-test; the ESP32-C6 probe, register dump, diagnostics and provisioning examples.
