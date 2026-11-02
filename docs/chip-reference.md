# Chip reference

## Family

The **PF1550** is an NXP integrated PMIC with:

- 3 buck converters (SW1, SW2, SW3)
- 3 LDOs (LDO1, LDO2, LDO3)
- Li-ion charger, USB VBUS management, OTP configuration

## Parts covered by this driver

| Part | Notes |
|------|-------|
| MC34PF1550A0EP | Portenta H7 / Synapse — **unprogrammed OTP** (A0EP) |
| MC34PF1550* | OTP-programmed variants — profile writes may be ignored for OTP-locked rails |
| MC32PF1550* | Same register map, different packaging / OTP |

## Identification

- **I2C 7-bit address:** `0x08`
- **Register 0x00 (DEVICE_ID):** expect `0x7C` for PF1550 family

## Datasheet

- Official: [PF1550.pdf](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf) (NXP, ~150 pages)
- Product page: [PF1550 PMIC](https://www.nxp.com/products/power-management/pmics/power-management-ics-pmic-for-high-performance-applications:PF1550)

## Internal project references

- VFR reference: `design/software/reference/Stm32Projects/PortentaH7_VFR_CubeIDE/CM7/Core/Src/I2C_PMIC.c`
- Synapse rail map: `pw-controller-synapse/docs/design/power/power-management.md`
