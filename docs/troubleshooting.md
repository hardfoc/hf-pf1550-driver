# Troubleshooting

Symptoms you may see while bringing up a PF1550, their causes and the fix for each, cheapest check first.

Start with `GetErrorFlags()` after a failing call: `DeviceNotFound` or `I2cRead` / `I2cWrite` point at the bus,
`WrongDeviceId` at the device, `InvalidParam` at the arguments, `ProfileApply` at a write the chip refused.

## The chip does not answer

| Symptom | Cause | Fix |
| --- | --- | --- |
| `EnsureInitialized()` fails with `DeviceNotFound` | No ACK at 0x08 | Check the pull-ups to VDDIO, SDA and SCL not swapped, a common ground |
| Same, and the SDK logs an address of 0x10 | 8-bit address passed to a 7-bit API | Pass 0x08; shift only inside your port if the SDK wants it |
| Same, on a programmed part | OTP selects another address (0x09 to 0x0F) | Scan the bus and pass the address to the constructor |
| `WrongDeviceId` | Another device answers at 0x08, or DEVICE_ID is not 0x7C | Read `ReadDeviceId()`; a PF1500 reads 0x78 |
| Errors at high bus speed | Clock above 400 kHz, or edges faster than 20 ns | Run at 400 kHz or less; lower the master's drive strength |
| The ESP32 example reports no device | Nothing wired, or the wrong pins | Expected without hardware; otherwise check Kconfig `PF1550_I2C_SDA_GPIO` / `SCL_GPIO` |

## Writes do not change the rails

| Symptom | Cause | Fix |
| --- | --- | --- |
| `SetSwVoltage()` / `SetSwVoltageMv()` return `false` | SW3, or an SW1 / SW2 declared DVS-disabled: those voltage registers are read-only. Or the voltage is off the DVS grid | Change the set point in OTP, or declare DVS with `SetSwDvsEnabled()` if the part has it ([Rails and OTP](rails-and-otp.md)) |
| A rail reads far below the expected voltage | A DVS-disabled code written with `WriteRegister()` to a DVS-enabled SW1 / SW2 (0x07 is 0.6875 V there) | Use `SetSwVoltageMv()`; declare the mode with `SetSwDvsEnabled()` |
| A profile change has no effect after a reset | The PMIC kept the previous values: an MCU reset does not reset it | Power the PMIC down completely, then apply the profile again |
| The snapshot shows the old values after a profile | The profile stopped at a failed write | Check the return value and `ProfileApply`; the writes before it did happen |
| `SetVbusCurrentLimitMa()` returns `false` | The value is not 10, 100, 500, 1000 or 1500 mA | Write `VBUS_INLIM_CNFG` with `code << 3` for other Table 58 values |

## Board symptoms (Portenta H7 and carriers)

| Symptom | Cause | Fix |
| --- | --- | --- |
| Carrier +3V3 or JTAG VTref is 0 V while the processor runs | SW2 is off; with the default profile it also turns off in STANDBY | Apply the carrier profile (`SW2_CTRL = 0x0F`) |
| LDO outputs are dead but the processor runs | The LDO inputs hang off SW1, which is not up yet | Use the carrier profile: SW1 first, then a 5 ms wait |
| USB does not enumerate after boot | USB_VBUS_EN / USB_OTG_EN low, or STANDBY high | `SetPowerMode(PowerMode::Run)` and `SetUsbRails(true, true)` before USB starts |
| The rails drop when the board idles | STANDBY asserted, and the rail's `STBY_EN` bit is clear | Drive STANDBY low, or set `kStbyEn` in the rail's control register |

## Faults and self-test

| Symptom | Cause | Fix |
| --- | --- | --- |
| `RunPowerSelfTest()` fails on battery | No VBUS sets `kVbusInval`, ranked `kWarning` | Expected without USB; rank `result.faults` for your board |
| A fault comes back right after `ClearLatchedFaults()` | The cause is still there | Read the SENSE register for the live state |
| INTB never goes low | Every interrupt is masked (the reset value) | `SetInterruptMaskAll(false)`, or unmask one group |

## What software cannot check

Measure the rails, VBUS / OTG behaviour on USB-C and the I2C waveforms under load with instruments; the snapshot
reports set points and status bits, not voltages.
