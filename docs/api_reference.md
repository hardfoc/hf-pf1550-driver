---
layout: default
title: "📖 API reference"
nav_order: 8
parent: "📚 Documentation"
permalink: /docs/api_reference/
---

# API reference

Complete reference for the `pf1550::` namespace. All symbols are exposed
through `pf1550.hpp`; the implementation lives in `src/pf1550.ipp`.

> Doxygen browsing: a full annotated API is published at
> [hardfoc.github.io/hf-pf1550-driver](https://hardfoc.github.io/hf-pf1550-driver/)
> alongside this Jekyll site.

## Headers

| File | Purpose |
|------|---------|
| [`inc/pf1550.hpp`](../inc/pf1550.hpp) | Templated driver class. |
| [`inc/pf1550_i2c_interface.hpp`](../inc/pf1550_i2c_interface.hpp) | CRTP `BusInterface` + strap-pin enum. |
| [`inc/pf1550_registers.hpp`](../inc/pf1550_registers.hpp) | All §12.1 / §12.2 datasheet registers + bit-field constants. |
| [`inc/pf1550_diagnostics.hpp`](../inc/pf1550_diagnostics.hpp) | `RailId`, `FaultFlags`, `FaultSeverity`, snapshot, self-test types. |
| [`inc/pf1550_voltage_tables.hpp`](../inc/pf1550_voltage_tables.hpp) | Code ↔ mV / code ↔ mA `constexpr` helpers. |
| [`inc/pf1550_profiles.hpp`](../inc/pf1550_profiles.hpp) | `portenta_h7_default`, `portenta_h7_carrier` register tables. |

## Class `pf1550::PF1550<BusType>`

`BusType` must inherit `pf1550::BusInterface<BusType>` (CRTP).

### Construction & lifecycle

| Method | Returns | Description |
|--------|---------|-------------|
| `PF1550(BusType*, uint8_t = kDefaultI2cAddress)` | — | Bind to a CRTP bus adapter. |
| `EnsureInitialized()` | `bool` | Idempotent — verifies `DEVICE_ID == 0x7C`. |
| `IsInitialized()` | `bool` | Cached init state. |
| `GetI2cAddress()` | `uint8_t` | 7-bit address. |
| `GetDriverVersion()` | `const char*` | CMake-generated semantic version. |

### Identification

| Method | Description |
|--------|-------------|
| `ReadDeviceId(uint8_t& id)` | Read register `0x00`. |
| `VerifyDevice()` | Read + assert `id == kExpectedDeviceId`. |

### Raw register access

| Method | Description |
|--------|-------------|
| `ReadRegister(Register reg, uint8_t& value)` | Single-byte read. |
| `WriteRegister(Register reg, uint8_t value)` | Single-byte write. |

### Profile application

| Method | Description |
|--------|-------------|
| `ApplyProfile(std::span<const RegisterWrite>)` | Apply an arbitrary write-table. |
| `ApplyPortentaH7DefaultProfile()` | Legacy VFR profile. |
| `ApplyPortentaH7CarrierProfile()` | **Recommended** — SW1-first, `SW2_CTRL=0x0F`. |

### Power mode and strap GPIOs

| Method | Description |
|--------|-------------|
| `SetPowerMode(PowerMode)` | Drive STANDBY strap. |
| `SetUsbRails(bool vbus_en, bool otg_en)` | Drive USB_VBUS_EN / USB_OTG_EN. |

### Status, charger, OTP

| Method | Description |
|--------|-------------|
| `ReadPmicStatus(uint8_t&)` | Read `STATE_INFO` raw byte. |
| `ReadPmicState(PmicState&)` | Decoded state (Wait/Run/Standby/Sleep/RegsDisable). |
| `ReadChargerState(ChargerState&, uint8_t* raw=nullptr)` | Decode `CHG_SNS`. |
| `ReadOtpByte(uint8_t addr, uint8_t&)` | Indirect OTP read via KEY1/KEY2/KEY3 + FMRADDR/FMRDATA. |
| `ReadOtpRegion(uint8_t*, size_t, uint8_t start)` | Bulk OTP read. |

### Voltage / current configuration

| Method | Description |
|--------|-------------|
| `SetSwVoltage(uint8_t index, SwVoltageCode)` | Write `SWn_VOLT` (n=1..3). |
| `SetLdoVoltage(uint8_t index, LdoVoltageCode)` | Write `LDOn_VOLT` (n=1..3). |
| `SetVbusCurrentLimitMa(uint16_t mA)` | Encode + write `VBUS_IN_LIM_CNFG`. |

### Interrupt management

| Method | Description |
|--------|-------------|
| `ReadInterruptCategory(uint8_t&)` | `INT_CATEGORY` (0x06) summary. |
| `ReadLatchedFaults(FaultFlags&)` | Aggregate every per-category `*_INT_STAT*` into a 32-bit bitmask. |
| `ClearLatchedFaults()` | Write `0xFF` to every RW1C status register. |
| `SetInterruptMaskAll(bool masked)` | Mask / unmask all categories at once. |

### Diagnostics and self-test

| Method | Description |
|--------|-------------|
| `ReadDiagnosticSnapshot(DiagnosticSnapshot&)` | One-shot read of identity, state, rails, faults, charger. |
| `RefreshStatusSnapshot(DiagnosticSnapshot&)` | Status only (state, charger, VBUS, latched + live faults: ≈14 reads); identity and rail configuration kept from the last full read. For a periodic monitor: status every tick, full read now and then. |
| `RunPowerSelfTest(SelfTestResult&)` | Boot-time classifier producing `FaultSeverity` (Info/Warning/Critical/McuKill). |

### Error tracking

`Error` is a bitmask enum (`I2cWrite`, `I2cRead`, `WrongDeviceId`,
`ProfileApply`, `DiagnosticRead`, `SelfTestFailed`, …). Use
`GetErrorFlags()` and `ClearErrorFlags(mask)` to introspect.

## Free helpers (`pf1550_voltage_tables.hpp`)

| Function | Purpose |
|----------|---------|
| `SwCodeToMillivolts(uint8_t code)` | SW1/SW2, DVS disabled (Table 31 right column; every code ≥ 7 = 3.30 V). |
| `SwDvsCodeToMillivolts(uint8_t code)` | SW1/SW2, DVS enabled (0.6 V + 12.5 mV × code). |
| `Sw3CodeToMillivolts(uint8_t code)` | SW3 (Table 37: 1.8 V + 100 mV × code; OTP-loaded, read-only). |
| `SetSwDvsEnabled(sw, enabled)` | Declare the OTP DVS selection for SW1/SW2 (default disabled). |
| `SwMillivoltsToCode(uint16_t mv)` | Reverse. `0xFF` if out of grid. |
| `LdoCodeToMillivolts(uint8_t code, bool is_ldo2)` | Decode LDO1/3 (group A) or LDO2 (group B). |
| `VbusLimitRegToMilliamps(uint8_t)` / `VbusLimitMilliampsToReg(uint16_t)` | VBUS LUT. |
| `SwCurrentLimitCodeToMilliamps(uint8_t)` | Decode `SWn_CTRL1[1:0]`. |
| `DecodeStateInfo(uint8_t)` / `DecodeChargerSense(uint8_t)` | Typed enum decoders. |

## Diagnostic types (`pf1550_diagnostics.hpp`)

| Type | Purpose |
|------|---------|
| `RailId` | `Sw1..Sw3`, `Ldo1..Ldo3`, `Vsnvs`. |
| `FaultSeverity` | `kInfo`, `kWarning`, `kCritical`, `kMcuKill`. |
| `FaultFlags` | Bitmask covering SW HS/LS, LDO faults, temp, MISC, charger, driver I²C. |
| `RailDiagnostics` | Per-rail: ctrl byte, voltage code, decoded mV, enable bits, latched/live. |
| `DiagnosticSnapshot` | Full PMIC state — used by HAL handler + manager. |
| `SelfTestResult` | Output of `RunPowerSelfTest`. |

The free `FaultSeverityPortentaH7(uint32_t flag)` and
`WorstSeverityPortentaH7(FaultFlags)` map each fault to its
Portenta-H7-specific severity (the rationale is documented in
`docs/quality/diovv-pmic.md` of the private `pw-controller-sw` firmware
repository).

## Constants

| Name | Value |
|------|-------|
| `kDefaultI2cAddress` | `0x08` |
| `kExpectedDeviceId` | `0x7C` |
| `kChargerPageBase` | `0x80` |

## Threading

The driver itself is **not** thread-safe; the HAL handler
`Pf1550Handler` provides RTOS-mutex-protected wrappers for use from
multiple threads.
