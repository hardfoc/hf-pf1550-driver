---
layout: default
title: "💡 Examples"
nav_order: 9
parent: "📚 Documentation"
permalink: /docs/examples/
---

# Examples

## ESP32-C6 (ESP-IDF)

| App | Source | Description |
|-----|--------|-------------|
| `pf1550_esp32c6_probe` | `pf1550_esp32c6_probe.cpp` | DEVICE_ID + PMIC_STATUS |
| `pf1550_esp32c6_register_dump` | `pf1550_esp32c6_register_dump.cpp` | Key regulator registers |
| `pf1550_esp32c6_diagnostics` | `pf1550_esp32c6_diagnostics.cpp` | Snapshot + self-test loop |
| **`pf1550_esp32c6_provision`** | `pf1550_esp32c6_provision.cpp` | **External I2C provisioning (pre-MCU bring-up)** |

Clone **with submodules**:

```bash
git clone --recursive https://github.com/hardfoc/hf-pf1550-driver.git
cd hf-pf1550-driver/examples/esp32
git submodule update --init --recursive
./scripts/build_app.sh list
./scripts/build_app.sh pf1550_esp32c6_probe Debug
./scripts/flash_app.sh pf1550_esp32c6_probe Debug
```

Configuration matrix: `examples/esp32/app_config.yml`.

For **external PMIC provisioning** before MCU bring-up, see
[ESP32-C6 provisioning](esp32-provisioning.md).

## Optional profile apply

Enable in `sdkconfig` when hardware is wired:

```
CONFIG_PF1550_APPLY_PORTENTA_PROFILE=y
```

**Warning:** only use on lab / unprogrammed PMIC parts.

See [examples/esp32/README.md](https://github.com/hardfoc/hf-pf1550-driver/blob/main/examples/esp32/README.md).
