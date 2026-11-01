# PF1550 register map (driver subset)

Extracted for software bring-up. See [PF1550.pdf](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf) for full bit definitions.

## Device ID

| Reg | Name | Expected |
|-----|------|----------|
| 0x00 | DEVICE_ID | 0x7C |

## Switch regulators

| Reg | Name |
|-----|------|
| 0x32 | SW1_VOLT |
| 0x33 | SW1_VOLT_DVS |
| 0x34 | SW1_VOLT2 |
| 0x35 | SW1_CTRL |
| 0x38 | SW2_VOLT |
| 0x39 | SW2_VOLT_DVS |
| 0x3A | SW2_VOLT2 |
| 0x3B | SW2_CTRL |
| 0x3E | SW3_VOLT |
| 0x3F | SW3_VOLT_DVS |
| 0x40 | SW3_VOLT2 |
| 0x41 | SW3_CTRL |
| 0x42 | SW3_CTRL1 (current limit) |

### Common SW voltage codes (RUN)

| Code | Voltage |
|------|---------|
| 0x05 | 2.5 V |
| 0x06 | 3.0 V |
| 0x07 | 3.3 V |
| 0x0D | 3.1 V |

## LDO regulators

| Reg | Name |
|-----|------|
| 0x4C | LDO1_VOLT |
| 0x4D | LDO1_CTRL |
| 0x4F | LDO2_VOLT |
| 0x50 | LDO2_CTRL (commit) |
| 0x52 | LDO3_VOLT |
| 0x53 | LDO3_CTRL |
| 0x58 | LDO_MISC |

### Portenta LDO voltage codes

| Rail | Code | Voltage |
|------|------|---------|
| LDO1 | 0x05 | 1.0 V |
| LDO3 | 0x09 | 1.2 V |
| LDO2 | 0x00 | 1.8 V |

## Status / USB

| Reg | Name |
|-----|------|
| 0x67 | PMIC_STATUS |
| 0x94 | VBUS_IN_CURR_LIM |

VBUS limit encoding: `(mA / 50) << 3` (1500 mA → 0xA0).

## OTP indirect access

| Reg | Name | Value |
|-----|------|-------|
| 0x6F | KEY1 | 0x15 |
| 0x9F | KEY2 | 0x50 |
| 0xDF | TEST_REG_KEY3 | 0xAB |
| 0xC4 | FMRADDR | OTP address |
| 0xC5 | FRMDATA | OTP data |

OTP address range: 0x1C … 0x36.

## Undocumented Portenta bootloader registers

| Reg | Portenta value |
|-----|----------------|
| 0x9C | 0x80 |
| 0x9E | 0x20 |

Used in Arduino bootloader sequence; included in `portenta_h7_default` profile.
