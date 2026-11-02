# Troubleshooting

## DEVICE_ID read fails / NACK

- Confirm **7-bit address 0x08** (not 0x10 shifted)
- Check I2C pull-ups and bus speed (400 kHz–1 MHz on Portenta)
- On Portenta: ensure **I2C1 on CM7** is initialized (not only CM4)

## DEVICE_ID != 0x7C

Wrong device on bus or bus fault. Expected PF1550 family byte **0x7C**.

## Profile writes have no effect

**PMIC state persists across MCU reset.** Many voltage changes only apply after **full power cycle**.

On OTP-programmed parts, **SW3** and other rails may ignore I2C writes — see datasheet OTP section.

## Register 0x50 (LDO2_CTRL) breaks I2C

Historical Portenta note: write **0x50 last** in the init sequence. The driver profile enforces this.

## USB dead after boot

Assert strap GPIOs **before** USB init:

- STANDBY = LOW (RUN)
- USB_VBUS_EN = HIGH
- USB_OTG_EN = HIGH

Apply **`portenta_h7_carrier`** (or manufacturing `PwPmic_ApplyCarrierProfile`) on
**cold boot** before PLL. CubeMX default GPIO may drive USB rails LOW.

## Carrier +3V3 / JTAG VTref = 0

SW2 (BUCK2) powers carrier +3V3. MCU may run on SW3 (+3V1 VCORE) while SW2 is
off. Use carrier profile with `SW2_CTRL=0x0F` and verify `[pmic]` on UART7.

## LDO outputs dead but MCU runs

LDO inputs are wired to +3V1SW (SW1). Verify SW1 before LDO1/2/3.

## ESP32 example: no device

Expected without hardware — probe logs warning and exits idle loop. CI builds do not require PMIC.

## Hardware validation (human operator)

Software CI cannot replace:

- Rail voltage measurement (SW1/2/3, LDO1/2/3)
- USB-C VBUS / OTG enumeration
- Scope on I2C during WiFi / high-load events

Use `./scripts/verify_driver_scaffold.sh` for software readiness only.
