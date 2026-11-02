# Local reference (not synced to Git)

Everything in this directory **except this file** is listed in the repo root `.gitignore` and will **never** be committed or pushed.

Use it for:

- NXP PF1550 datasheet PDF and application notes
- Board-specific captures (scope shots, I2C traces)
- Scratch notes during bring-up

Suggested layout:

```text
_local_reference/
  README.md          (this file — tracked)
  datasheet/
    PF1550.pdf       (ignored — run scripts/fetch_datasheet.sh)
```

Readable register extracts for agents and developers live in tracked paths:

- `docs/registers.md` — the registers the driver names (Register map page)
- `docs/datasheet/PF1550-i2c-register-reference.md` — auto-generated from PDF (Section 12)

Fetch the official PDF:

```bash
./scripts/fetch_datasheet.sh
./scripts/extract_register_reference.sh
```
