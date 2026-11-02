# Hardware setup — ESP32-C6 examples

The ESP32-C6 examples exercise the **driver API over I2C**. They do not require a PF1550 on the bench for CI builds.

## Default example wiring

| Signal | ESP32-C6 pin | Notes |
|--------|--------------|-------|
| I2C SDA | GPIO4 | Internal pull-ups enabled in bus code |
| I2C SCL | GPIO5 | 400 kHz default |
| PMIC I2C addr | 0x08 | 7-bit |

Connect to a PF1550 eval board or Portenta PMIC tap for live reads.

For **external provisioning before MCU bring-up** (I2C test pads on custom
boards), see [ESP32-C6 provisioning](esp32-provisioning.md).

## Expected probe output

With PF1550 present:

```text
DEVICE_ID=0x7C (expected 0x7C)
PMIC_STATUS (0x67)=0x..
```

Without hardware:

```text
No PF1550 at 0x08 ...
```

## Portenta H7 (production target)

| Signal | STM32 pin |
|--------|-----------|
| I2C1 SCL | PB6 |
| I2C1 SDA | PB7 |
| PMIC_STANDBY | PJ0 (LOW = RUN) |
| USB_VBUS_EN | PJ4 |
| USB_OTG_EN | PJ6 |
| PMIC_INT | PK0 |

See [Portenta profile](portenta-profile.md).

**Next:** [Examples →](examples.md)
