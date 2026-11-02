# API guide

How to do the common jobs with the driver: identify the chip, apply a profile, set rails, read status, take a
diagnostic snapshot, handle faults and read OTP.

Every snippet on this page is compiled by the host CI from
[`tests/docs/api_tasks.cpp`](../tests/docs/api_tasks.cpp), with `Driver` standing for `pf1550::PF1550<MyI2c>`. The
[API reference](api-reference.md) lists every member.

## The shape of the API

One `PF1550<Bus>` object per PMIC, constructed from a pointer to your port and the 7-bit address (default
`kDefaultI2cAddress`, 0x08). Every member returns `bool` (or fills an out-parameter) and never throws. A `false`
also sets a bit in the sticky error flags, which stay set until you clear them.

| Task | Members |
| --- | --- |
| Identify | `EnsureInitialized()`, `IsInitialized()`, `ReadDeviceId()`, `VerifyDevice()`, `GetI2cAddress()` |
| Profiles | `ApplyProfile(span)`, `ApplyPortentaH7DefaultProfile()`, `ApplyPortentaH7CarrierProfile()` |
| Rails and limits | `SetSwVoltageMv()`, `SetSwVoltage()`, `SetLdoVoltage()`, `SetVbusCurrentLimitMa()`, `SetSwDvsEnabled()` |
| Strap pins | `SetPowerMode()`, `SetUsbRails()` |
| Status | `ReadPmicStatus()`, `ReadPmicState()`, `ReadChargerState()`, `ReadInterruptCategory()` |
| Faults and interrupts | `ReadLatchedFaults()`, `ClearLatchedFaults()`, `SetInterruptMaskAll()` |
| Diagnostics | `ReadDiagnosticSnapshot()`, `RefreshStatusSnapshot()`, `RunPowerSelfTest()` |
| OTP and raw access | `ReadOtpByte()`, `ReadOtpRegion()`, `ReadRegister()`, `WriteRegister()` |
| Errors | `GetErrorFlags()`, `ClearErrorFlags(mask)`, `GetDriverVersion()` |

## Identify the chip

```cpp
--8<-- "tests/docs/api_tasks.cpp:init"
```

`EnsureInitialized()` brings your bus up, reads DEVICE_ID and accepts only 0x7C. After the first success it
returns `true` without touching the bus. The other members do not require it, but call it first so a wiring
problem shows up as `DeviceNotFound` rather than as a failed rail write.

## Apply a profile

```cpp
--8<-- "tests/docs/api_tasks.cpp:profile"
```

A profile is applied in table order, waiting each entry's `delay_us` through your port's `DelayUs()`. It stops at
the first write that fails and sets `Error::ProfileApply`; the writes before it have already taken effect.
[Portenta H7 profiles](portenta-h7.md) lists every write of the built-in tables; [Configuration](configuration.md)
shows how to write your own.

## Set rails and limits

```cpp
--8<-- "tests/docs/api_tasks.cpp:rails"
```

`SetLdoVoltage(n, code)` writes the RUN set point of LDO `n` (1 to 3). `SetSwVoltageMv(n, mv)` and
`SetSwVoltage(n, code)` set the RUN voltage of SW1 or SW2, and only when that buck runs with DVS enabled (declared
with `SetSwDvsEnabled()`): they write the DVS code for the voltage. They refuse with `Error::InvalidParam`, writing
nothing, for SW3 and for a DVS-disabled SW1 / SW2, whose voltage registers are read-only, and for a voltage that
is not on the DVS grid (0.6 V to 1.3875 V in 12.5 mV steps). Any other index is rejected the same way. Enables have no helper: build the control byte from `SwCtrlBits` or
`LdoCtrlBits` and use `WriteRegister()`.

!!! note "`SwVoltageCode` names a voltage"
    `SwVoltageCode` values are DVS-disabled codes. `SetSwVoltage()` uses the voltage the code stands for
    (`V1_1` is 1.1 V) and converts it to the DVS encoding, so codes above 1.3875 V are refused. [Rails and
    OTP](rails-and-otp.md#sw1-and-sw2-two-code-tables) has both tables.

## Read status

```cpp
--8<-- "tests/docs/api_tasks.cpp:status"
```

`ReadPmicState()` decodes STATE_INFO (0x67) into `PmicState`: `Wait`, `Run`, `Standby`, `Sleep`, `RegsDisable`
or `Unknown`. `ReadChargerState()` decodes CHG_SNS (0x87) into `ChargerState` (pre-charge, fast charge CC / CV,
end of charge, done, faults, off) and can return the raw byte too. `PmicStateName()`, `ChargerStateName()` and
`RailName()` give log-friendly strings.

## Read a diagnostic snapshot

```cpp
--8<-- "tests/docs/api_tasks.cpp:diagnostics"
```

`DiagnosticSnapshot` holds identity (DEVICE_ID, OTP_FLAVOR, SILICON_REV), INT_CATEGORY, the decoded state and
charger state, VBUS sense, the decoded VBUS input limit, the aggregated `FaultFlags`, and one `RailDiagnostics` per
rail: raw control and voltage bytes, set point in mV, RUN and STANDBY enables, latched and live fault marks. The
VSNVS entry is not filled in. When a read fails, `read_ok` is `false` and `Error::DiagnosticRead` is set, but every
field that was read is kept for logging.

`RefreshStatusSnapshot()` re-reads only what changes without a register write (interrupts, state, charger, VBUS,
faults), at about half the bus time of a full read. A monitor polling at 20 Hz can do a full read once a second
and refreshes in between.

## Handle errors and faults

```cpp
--8<-- "tests/docs/api_tasks.cpp:faults"
```

| Signal | Meaning | Where to look |
| --- | --- | --- |
| A call returns `false` | NACK, bad argument or a failed read | `GetErrorFlags()`: `I2cWrite`, `I2cRead`, `InvalidParam`, `DeviceNotFound`, `WrongDeviceId`, `NotInitialized`, `ProfileApply`, `DiagnosticRead`, `SelfTestFailed` |
| INT_CATEGORY non-zero | A group has a latched interrupt | `ReadLatchedFaults()`, or the STAT register with `ReadRegister()` |
| INTB low | An unmasked interrupt is pending | Masks power up set: `SetInterruptMaskAll(false)` unmasks every group |
| `kVbusInval` in `FaultFlags` | VBUS_VALID_SNS is clear: no valid USB or adapter input | Expected on battery |

STAT bits are write-1-to-clear: `ClearLatchedFaults()` writes 0xFF to every STAT register. Clear after you have
handled the cause; a bit that comes back at once means the cause is still there. The sticky driver flags are
separate: `ClearErrorFlags()` clears them.

`ReadLatchedFaults()` decodes the STAT registers at the data sheet's bit positions ([Register
map](registers.md#interrupt-registers)). `PWRUP_I` and `PWRDN_I` (a finished RUN / STANDBY / SLEEP transition) are
events, not faults, and have no `FaultFlags` bit; read `MISC_INT_STAT0` if you need them. `kWdiAssert` and
`kResetBmcu` are never set: the PF1550 has no such interrupt bits.

## Run the boot self-test

```cpp
--8<-- "tests/docs/api_tasks.cpp:selftest"
```

`RunPowerSelfTest()` takes a full snapshot and passes only when DEVICE_ID is 0x7C, the PMIC is in RUN, every SW
and LDO rail is enabled in RUN and no latched fault ranks above `kInfo`. The ranking is
`FaultSeverityPortentaH7()`: it assumes the Portenta H7's wiring (MCU on SW3, LDO1 and LDO3 critical). On another
board, rank `result.faults` yourself.

## Read OTP and raw registers

```cpp
--8<-- "tests/docs/api_tasks.cpp:otp"
```

`ReadOtpByte()` writes the three unlock keys, the OTP address to FMRADDR, waits 2 ms with `DelayUs()` and reads
FMRDATA. `ReadOtpRegion()` repeats it and refuses addresses past 0x36. `ReadRegister()` and `WriteRegister()` take
any `pf1550::Register`; the [Register map](registers.md) lists them.
