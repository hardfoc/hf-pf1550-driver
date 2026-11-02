# Portenta H7 PMIC profiles

## Rail assignment (MCU domain — module schematic)

| PF1550 | Schematic | Voltage | Role |
|--------|-----------|---------|------|
| SW1 | BUCK1 | 3.0 V (+3V1SW) | USB/ULPI, SDRAM, ETH; **LDO1/2/3 inputs** |
| SW2 | BUCK2 | 3.3 V (+VOUT) | Carrier HDC / JTAG VTref |
| SW3 | BUCK3 | 3.3 V on the modules read so far (OTP code 0x0F, Table 37; older notes said 3.1 V) | STM32, QSPI, MIPI |
| LDO1 | — | 1.0 V | MIPI (input = +3V1SW) |
| LDO2 | — | 1.8 V | MIPI, USB/ULPI |
| LDO3 | — | 1.2 V | STM32 DSI, ETH PHY |

VSYS is a **2 A LDO** from VIN (~4.5 V, or ~4.1 V USB-only). Bucks switch from VSYS.

## Profiles

| Name | API | Use when |
|------|-----|----------|
| `portenta_h7_default` | `ApplyPortentaH7DefaultProfile()` | VFR / legacy order |
| **`portenta_h7_carrier`** | **`ApplyPortentaH7CarrierProfile()`** | **Carrier bring-up: SW1-first, SW2_CTRL=0x0F** |

Carrier profile details:

- SW1 enabled **before** LDO registers (LDO inputs wired to +3V1SW).
- SW2_CTRL = `0x0F` keeps carrier +3V3 up across STANDBY transitions.
- Charger LED disabled (`0x9C`, `0x9E`).

Manufacturing CM7 uses the same table in `pw_pmic_early_init.c` (C, no C++).

## GPIO straps (STM32H7 CM7)

| Signal | Pin | Boot value |
|--------|-----|------------|
| PMIC_STANDBY | PJ0 | LOW = RUN |
| USB_VBUS_EN | PJ4 | HIGH for USB-C VBUS |
| USB_OTG_EN | PJ6 | HIGH for OTG HS |
| PMIC_INT | PK0 | Input (optional) |

## I2C

- **Bus:** I2C1, PB6/PB7
- **Address:** 0x08
- **Speed:** up to 1 MHz (Fast Mode Plus) on Portenta

## Init sequence

Use `ApplyPortentaH7DefaultProfile()` or **`ApplyPortentaH7CarrierProfile()`** for
carrier boards (recommended when LDO inputs = +3V1SW).

Call **early in CM7 `main()`**, before USB/Ethernet, after GPIO straps
(STANDBY LOW, USB rails HIGH).

Register `0x50` (LDO2_CTRL commit) is written **last**. SW3's voltage registers are OTP-loaded and read-only (no DVS on SW3); with SW1/SW2 DVS disabled their voltage registers are read-only too (datasheet §6.2.5) — profile writes to them are ignored by the part.

## Important behavior

1. **PMIC state survives MCU reset** — only full power cycle resets PF1550.
2. **First init after power-up matters** — voltage changes may not take effect on subsequent soft resets.
3. **CubeMX does not configure the PMIC** — bare-metal firmware must call profile init explicitly.

## VBUS / charger extras in profile

- VBUS input limit: 1500 mA (reg 0x94 = 0xA0)
- SW3 current limit: 2 A (reg 0x42 = 0x02)
- Charger LED disabled (0x9C, 0x9E)
