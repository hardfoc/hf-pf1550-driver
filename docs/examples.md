# Examples

Complete ESP32-C6 applications that exercise the driver over ESP-IDF's I2C master, from a bus probe to external
provisioning.

They run on an ESP32-C6 DevKit wired to a PF1550 (SDA on GPIO4, SCL on GPIO5 by default); the port is
[`esp32_pf1550_bus.hpp`](../examples/esp32/main/esp32_pf1550_bus.hpp) and the pins and options are in
[`Kconfig.projbuild`](../examples/esp32/main/Kconfig.projbuild). None of them needs a PF1550 to build, and without
one each logs that it found no device and stops. CI builds every app below on each pull request.

## The apps

<!-- generated: examples -->
| App | What it shows | Source | CI |
| --- | --- | --- | --- |
| `pf1550_esp32c6_probe` :material-star:{ title="featured" } | Probe PF1550 at 0x08, read DEVICE_ID and PMIC status over I2C | [`pf1550_esp32c6_probe.cpp`](../examples/esp32/main/pf1550_esp32c6_probe.cpp) | yes |
| `pf1550_esp32c6_register_dump` | Dump key PF1550 regulator and status registers | [`pf1550_esp32c6_register_dump.cpp`](../examples/esp32/main/pf1550_esp32c6_register_dump.cpp) | yes |
| `pf1550_esp32c6_diagnostics` | Full diagnostic snapshot + boot self-test + fault classification | [`pf1550_esp32c6_diagnostics.cpp`](../examples/esp32/main/pf1550_esp32c6_diagnostics.cpp) | yes |
| `pf1550_esp32c6_provision` :material-star:{ title="featured" } | External PMIC provisioning before host MCU bring-up (I2C test pads) | [`pf1550_esp32c6_provision.cpp`](../examples/esp32/main/pf1550_esp32c6_provision.cpp) | yes |

Build and flash one (from `examples/esp32`, ESP-IDF environment loaded):

```bash
./scripts/build_app.sh pf1550_esp32c6_probe Release
./scripts/flash_app.sh flash_monitor pf1550_esp32c6_probe Release
```

Source of this table: [`examples/esp32/app_config.yml`](../examples/esp32/app_config.yml).
<!-- /generated -->

## Set up once

```bash
git submodule update --init examples/esp32/scripts   # the HardFOC ESP-IDF build scripts
cd examples/esp32
. $IDF_PATH/export.sh                                 # ESP-IDF v5.5
./scripts/build_app.sh list                           # every app and build type
```

The scripts read [`app_config.yml`](../examples/esp32/app_config.yml) (the same file this page's table comes
from) and build each app as its own firmware image.

## What to read in an example

| You want to | Read |
| --- | --- |
| See the smallest bring-up: bus, DEVICE_ID, STATE_INFO | `pf1550_esp32c6_probe.cpp` |
| Dump the regulator and status registers in hex | `pf1550_esp32c6_register_dump.cpp` |
| Run a monitor loop: snapshot, self-test, fault classification and clearing | `pf1550_esp32c6_diagnostics.cpp` |
| Configure a board from outside before its processor runs | `pf1550_esp32c6_provision.cpp` ([External provisioning](provisioning.md)) |

!!! warning "The probe and provision apps can write rails"
    `pf1550_esp32c6_provision` always applies the selected profile, and `pf1550_esp32c6_probe` does when
    `CONFIG_PF1550_APPLY_PORTENTA_PROFILE=y`. Both profiles are written for the Portenta H7: on another board,
    check [Portenta H7 profiles](portenta-h7.md) against your schematic first.
