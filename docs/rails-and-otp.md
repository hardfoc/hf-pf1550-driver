# Rails and OTP

How the PF1550 encodes rail voltages, enables and limits, which registers OTP controls, and what that means for
the values the driver writes.

All tables come from the [PF1550 data sheet](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf), Rev. 7; the
driver's decoders in [`pf1550_voltage_tables.hpp`](../inc/pf1550_voltage_tables.hpp) implement them and check
themselves at compile time.

## OTP decides first

At start-up (REGS_DISABLE to RUN) the PF1550 copies its OTP into the regulator registers: set points, enables,
sequence, current limits, charger parameters. Register writes over I2C then change the running configuration,
not OTP. The driver reads OTP (`ReadOtpByte()`, `ReadOtpRegion()`), but never programs it.

Three consequences for a host:

- **An MCU reset does not reset the PF1550.** The registers keep what you wrote while the PMIC stays powered.
  To start again from OTP values, power the PMIC down (remove VBUS and the battery) or send it through
  REGS_DISABLE.
- **An A0EP part starts with nothing useful in OTP.** OTP_FLAVOR (0x01) reads 0x00 and the host must configure
  every rail it needs, which is what the [Portenta H7 profiles](portenta-h7.md) do.
- **Some registers are read-only by design.** The next two sections say which.

## SW1 and SW2: two code tables

`OTP_SWx_DVS_SEL` selects one of two encodings for SW1 and for SW2 (Table 31). It lives in OTP, so the driver
cannot read it with an ordinary register access: you declare it with `SetSwDvsEnabled()` and the snapshot decodes
`SWx_VOLT` accordingly. The default is DVS disabled.

| Code (`SWx_VOLT[5:0]`) | DVS enabled (`OTP_SWx_DVS_SEL = 0`) | DVS disabled (`OTP_SWx_DVS_SEL = 1`) |
| --- | --- | --- |
| 0 | 0.6000 V | 1.10 V |
| 1 | 0.6125 V | 1.20 V |
| 2 | 0.6250 V | 1.35 V |
| 3 | 0.6375 V | 1.50 V |
| 4 | 0.6500 V | 1.80 V |
| 5 | 0.6625 V | 2.50 V |
| 6 | 0.6750 V | 3.00 V |
| 7 to 63 | 0.6875 V + 12.5 mV per code, up to 1.3875 V | 3.30 V |

With DVS enabled, `SWx_VOLT`, `SWx_STBY_VOLT` and `SWx_SLP_VOLT` can be changed after start-up. With DVS
disabled they are read-only, and the data sheet says they must not be written.

!!! danger "`SwVoltageCode` values are DVS-disabled codes"
    `SwVoltageCode::V3_3` is code 0x07. On an SW1 or SW2 that runs with DVS enabled, the same code is 0.6875 V.
    Before calling `SetSwVoltage(1, ...)` or `SetSwVoltage(2, ...)`, know which table the part uses; for a DVS
    rail, write a DVS code with `WriteRegister(Register::Sw1Volt, code)` instead.

## SW3

SW3 has one table (Table 37): 1.80 V + 100 mV per code, 0 to 15, so 1.80 V to 3.30 V. SW3 does not support DVS:
its three voltage registers are loaded from fuses and read-only. `Sw3CodeToMillivolts()` decodes them.

## LDO1, LDO2, LDO3

The LDO voltage registers are loaded from OTP and writable.

| Rail | Codes | Output |
| --- | --- | --- |
| LDO1, LDO3 (Table 41) | 0 to 15 | 0.75 V to 1.50 V in 50 mV steps |
| LDO1, LDO3 (Table 41) | 16 to 31 | 1.80 V to 3.30 V in 100 mV steps |
| LDO2 (Table 43) | 0 to 15 | 1.80 V to 3.30 V in 100 mV steps |

`LdoVoltageCode` names the three codes the Portenta H7 uses: `V1_0` (0x05, LDO1), `V1_2` (0x09, LDO3) and
`V1_8` (0x00, LDO2). `LdoCodeToMillivolts(code, is_ldo2)` decodes any code.

## Enables

The control registers share one layout for the bucks (`SWx_CTRL`, 0x35 / 0x3B / 0x41) and the LDOs (`LDOx_CTRL`,
0x4D / 0x50 / 0x53) in their low four bits:

| Bit | `SwCtrlBits` / `LdoCtrlBits` | Set means |
| --- | --- | --- |
| 0 | `kEn` | On in RUN |
| 1 | `kStbyEn` | On in STANDBY |
| 2 | `kOMode` | On in SLEEP |
| 3 | `kLpwr` | Low-power mode in STANDBY and SLEEP |
| 4 | `kDvsSpeed` (SW) / `kLsEn` (LDO) | Slow DVS slew / load-switch mode (LDO1, LDO3; can only be set, not cleared) |
| 5 to 7 | `kFpwmInDvs`, `kFpwm`, `kRdisEnb` (SW only) | Forced PWM during DVS, forced PWM, discharge resistor off |

So `0x0F` keeps a rail on in RUN, STANDBY and SLEEP with low-power mode, and `0x01` turns it off as soon as the
PMIC enters STANDBY.

## Current limits

| `SWx_ILIM[1:0]` (`SWx_CTRL1` bits 1:0) | Typical limit |
| --- | --- |
| `00` | 1.0 A |
| `01` | 1.2 A |
| `10` | 1.5 A |
| `11` | 2.0 A |

`SwCurrentLimitCodeToMilliamps()` decodes the field.

## VBUS input current limit

`VBUS_LIN_ILIM` is bits 7:3 of VBUS_INLIM_CNFG (absolute address 0x94); the power-on value is code 13, 400 mA
(Table 58).

| Codes | Limit |
| --- | --- |
| 0 to 8 | 10 mA to 50 mA in 5 mA steps |
| 9 to 15 | 100, 150, 200, 300, 400, 500, 600 mA |
| 16 to 20 | 700, 800, 900, 1000, 1500 mA |
| 21 to 31 | Reserved |

`VbusLimitRegToMilliamps()` decodes the register byte and returns 0 for a reserved code; the snapshot's
`vbus_in_limit_ma` uses it. `SetVbusCurrentLimitMa()` accepts 10, 100, 500, 1000 and 1500 mA and rejects anything else with
`Error::InvalidParam`. For another value, write the byte yourself:
`WriteRegister(Register::VbusInCurrentLimit, code << 3)`.

## Reading OTP

OTP bytes 0x1C to 0x36 (`OtpRegion`) are read through FMRADDR / FMRDATA after three unlock writes; the sequence
is on the [Hardware](hardware.md#bus-settings) page. Each byte costs five I2C transfers and a 2 ms wait, so a full
region read takes about 60 ms. Use it at bring-up to see what a programmed part will do at power-up, not in a
control loop.

Next: [Register map](registers.md).
