---
layout: default
title: "📚 Documentation"
description: "Complete documentation for the HardFOC PF1550 PMIC driver"
nav_order: 2
parent: "HardFOC PF1550 Driver"
permalink: /docs/
has_children: true
---

# HF-PF1550 documentation

Welcome. This site mirrors the [`docs/`](https://github.com/hardfoc/hf-pf1550-driver/tree/main/docs) folder and documents the **NXP PF1550** PMIC family driver used on Portenta H7 and Synapse MCU power domains.

> **Browse on GitHub:** [repository home](https://github.com/hardfoc/hf-pf1550-driver) · [Issues](https://github.com/hardfoc/hf-pf1550-driver/issues)

## Documentation structure

### Getting started

- **[Installation](installation.md)** — Toolchain, CMake, generated headers
- **[Quick start](quickstart.md)** — Minimal I2C bus adapter + device probe
- **[Chip reference](chip-reference.md)** — Parts, DEVICE_ID, family scope

### Hardware & integration

- **[Hardware setup — ESP32-C6](hardware_setup.md)** — Lab I2C wiring for examples
- **[ESP32-C6 provisioning](esp32-provisioning.md)** — External I2C PMIC setup before MCU bring-up
- **[Portenta H7 profile](portenta-profile.md)** — Default + `carrier` profiles, rails, straps
- **[Configuration](configuration.md)** — Profiles, per-rail tuning, interrupt masks
- **[CMake integration](cmake_integration.md)** — `hf::pf1550`, build settings
- **[Platform integration](platform_integration.md)** — hf-core `Pf1550Handler`

### Reference & examples

- **[API reference](api_reference.md)** — `PF1550<BusType>`, diagnostics, errors
- **[Examples](examples.md)** — ESP32-C6 probe, diagnostics, provisioning
- **[Troubleshooting](troubleshooting.md)** — OTP locks, I2C, power-cycle behavior

### Manufacturer

- **[Datasheet & links](datasheet/README.md)** — PDF fetch, readable extracts

## Recommended reading order

1. [Installation](installation.md)
2. [Chip reference](chip-reference.md) — confirm **0x7C** @ 0x08
3. [Quick start](quickstart.md) — implement `BusInterface`
4. [Portenta profile](portenta-profile.md) — if targeting Portenta / Synapse
5. [Examples](examples.md) — ESP32-C6 CI-safe probe

## Verification (software-only)

Run the scaffold verifier before opening a PR:

```bash
./scripts/verify_driver_scaffold.sh
```

Hardware validation (scope, rail voltages, USB bring-up) remains a **human operator** step.
