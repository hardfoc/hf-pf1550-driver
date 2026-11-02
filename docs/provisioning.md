# External provisioning

Configure a PF1550 from an ESP32-C6 over the board's I2C test pads before the board's own processor runs, and
check the result.

The `pf1550_esp32c6_provision` example does it: it applies a Portenta H7 profile, runs the boot self-test, prints a
bench checklist and then monitors the rails. Use it on a new board with an unprogrammed (A0EP) PMIC, or to bring
up a carrier whose +3V3 depends on SW2.

!!! warning "Two masters, one PMIC"
    Do not connect the ESP32-C6 while the board's own processor drives the same I2C bus. Hold that processor in
    reset or leave it unprogrammed, and disconnect the ESP32-C6 before you release it.

## When to use it

| Situation | Provision from the ESP32-C6? |
| --- | --- |
| First power-up, bare module, no firmware on the processor | Yes |
| Carrier +3V3 or JTAG VTref reads 0 V while the processor runs from SW3 | Yes, with the carrier profile |
| Production firmware already applies a profile early in boot | No |
| OTP-programmed PMIC whose rails are already right | Probe and diagnostics only |

## Wiring

| ESP32-C6 | Board | Notes |
| --- | --- | --- |
| GPIO4 | PF1550 SDA | Kconfig `PF1550_I2C_SDA_GPIO` |
| GPIO5 | PF1550 SCL | Kconfig `PF1550_I2C_SCL_GPIO` |
| GND | GND | |
| any GPIO (optional) | STANDBY, USB_VBUS_EN, USB_OTG_EN test points | Enable `PF1550_GPIO_STRAPS_ENABLE` and set the three GPIO numbers |

Power the board from USB or a bench supply. The PMIC answers once VSYS and VDDIO are up; the board's processor
does not need to run. Without strap GPIOs, the board's own pull resistors must hold STANDBY low.

## Build, flash, watch

From `examples/esp32`, with ESP-IDF v5.5 loaded and the scripts submodule initialised:

```bash
./scripts/build_app.sh pf1550_esp32c6_provision Release
./scripts/flash_app.sh flash_monitor pf1550_esp32c6_provision Release
```

A successful run logs the profile name, `Self-test: PASS`, a checklist of the voltages to measure at the module
(SW1, SW2, SW3, LDO1 to LDO3) with the set points read back, and `Provisioning complete`. It then prints the PMIC
state, the SW set points and the fault flags every 10 s.

## Afterwards

1. Disconnect the ESP32-C6 from the I2C bus.
2. Power the board down completely (USB and battery off) if this was the first profile on the part, then power it
   up again, and measure the rails.
3. Release the board's processor or flash its firmware.

The PF1550 keeps its registers only while it stays powered: an unprogrammed part forgets the profile at the next
full power-down. Production firmware has to apply the profile at every boot.

## Kconfig options

Edit them with `./scripts/build_app.sh pf1550_esp32c6_provision Release menuconfig`, under **PF1550 Example
Configuration**.

| Option | Default | Meaning |
| --- | --- | --- |
| `PF1550_I2C_SDA_GPIO` | 4 | SDA pin |
| `PF1550_I2C_SCL_GPIO` | 5 | SCL pin |
| `PF1550_I2C_FREQ_HZ` | 400000 | I2C clock; the PF1550 supports up to 400 kHz |
| `PF1550_PROFILE_SELECT` | `PF1550_PROFILE_CARRIER` | Profile the provision app applies; or `PF1550_PROFILE_DEFAULT` |
| `PF1550_GPIO_STRAPS_ENABLE` | off | Drive the strap pins from the ESP32-C6 |
| `PF1550_GPIO_STANDBY`, `PF1550_GPIO_USB_VBUS_EN`, `PF1550_GPIO_USB_OTG_EN` | −1 (not wired) | Strap GPIO numbers |
| `PF1550_APPLY_PORTENTA_PROFILE` | off | Lets the probe example apply the profile too |

Next: [Troubleshooting](troubleshooting.md).
