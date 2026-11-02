# Datasheet and manufacturer resources

## Official source

| Resource | Link |
|----------|------|
| PF1550 product page | [nxp.com PF1550](https://www.nxp.com/products/power-management/pmics/power-management-ics-pmic-for-high-performance-applications:PF1550) |
| PF1550 data sheet (Rev. 7) | [PF1550.pdf](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf) |

NXP PDFs are **not** committed to git. Download locally:

```bash
./scripts/fetch_datasheet.sh
```

PDF lands in `_local_reference/datasheet/PF1550.pdf` (gitignored).

## Readable extracts (in git)

| File | Purpose |
|------|---------|
| [PF1550-register-map.md](PF1550-register-map.md) | Curated register subset used by the driver |
| [PF1550-i2c-register-reference.md](PF1550-i2c-register-reference.md) | Auto-generated Section 12 extract |

Regenerate the Section 12 extract after fetching a new PDF:

```bash
./scripts/extract_register_reference.sh
```

## What to read first

1. **Section 12 — Register map** — I2C addresses, bit fields, OTP vs runtime writable
2. **Section on OTP** — which rails are OTP-locked on programmed parts (MC34PF1550 vs A0EP)
3. **Power modes** — RUN, STANDBY, SLEEP; interaction with STANDBY strap pin
4. **DEVICE_ID @ 0x00** — expect **0x7C** for PF1550 family on the bus

## Part numbers

| Part | Notes |
|------|-------|
| MC34PF1550A0EP | Unprogrammed OTP — full I2C profile writes (Portenta H7) |
| MC34PF1550* | Factory OTP — some SW/LDO settings may ignore runtime writes |

See also [Chip reference](../chip-reference.md) and [Portenta profile](../portenta-profile.md).
