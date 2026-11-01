#!/usr/bin/env python3
"""Extract PF1550 datasheet Section 12 register map to markdown."""
from __future__ import annotations

import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: extract_register_reference.py INPUT.txt OUTPUT.md", file=sys.stderr)
        return 2

    raw = Path(sys.argv[1]).read_text(encoding="utf-8", errors="replace").splitlines()
    out = Path(sys.argv[2])

    start = next((i for i, line in enumerate(raw) if line.strip() == "12 Register map"), None)
    if start is None:
        print("Section 12 not found in extract", file=sys.stderr)
        return 1

    # Stop before revision history / legal boilerplate after register tables.
    end = len(raw)
    for i in range(start + 1, len(raw)):
        if raw[i].strip().startswith("13 ") or raw[i].strip() == "13 Revision history":
            end = i
            break

    body = "\n".join(raw[start:end]).strip()
    # Collapse excessive blank lines for readability.
    while "\n\n\n" in body:
        body = body.replace("\n\n\n", "\n\n")

    content = f"""---
layout: default
title: "PF1550 I2C register reference (extract)"
description: "Auto-generated readable extract from NXP PF1550 Rev.7 Section 12"
nav_order: 12
parent: "Datasheet & links"
permalink: /docs/datasheet/PF1550-i2c-register-reference/
---

# PF1550 I2C register reference (readable extract)

> **Auto-generated** from `_local_reference/datasheet/PF1550.pdf` via
> `scripts/extract_register_reference.sh`. Do not hand-edit — regenerate after
> datasheet updates. Authoritative source: [NXP PF1550.pdf](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf).

For the **driver-maintained subset** used in code, see [PF1550-register-map.md](PF1550-register-map.md).

## Section 12 — Register map (plain text)

```
{body}
```
"""
    out.write_text(content, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
