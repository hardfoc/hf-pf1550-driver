# ESP32 PF1550 examples

Build and flash via the shared ESP-IDF project tools submodule:

```bash
git submodule update --init --recursive
./scripts/build_app.sh pf1550_esp32c6_probe Debug
./scripts/flash_app.sh pf1550_esp32c6_probe Debug
```

## Wiring (lab / eval)

| Signal | ESP32-C6 default |
|--------|------------------|
| SDA | GPIO4 |
| SCL | GPIO5 |
| PMIC I2C | 0x08 |

Without a PF1550 on the bus the probe app logs a warning and idles — suitable for CI.

## Apps

| App | Purpose |
|-----|---------|
| `pf1550_esp32c6_probe` | Read DEVICE_ID, optional profile apply |
| `pf1550_esp32c6_register_dump` | Dump regulator / status registers |
| `pf1550_esp32c6_diagnostics` | Full snapshot + self-test every 5 s |
| **`pf1550_esp32c6_provision`** | **Apply carrier profile before MCU bring-up** |

### External provisioning (custom boards)

When PMIC I2C is on test pads and the host MCU is held in reset:

```bash
./scripts/build_app.sh pf1550_esp32c6_provision Debug
./scripts/flash_app.sh pf1550_esp32c6_provision Debug
./scripts/idf_flash_monitor.sh pf1550_esp32c6_provision Debug
```

See [docs/esp32-provisioning.md](../../docs/esp32-provisioning.md) for strap GPIO
Kconfig and bench verification checklist.

## menuconfig

```bash
./scripts/build_app.sh pf1550_esp32c6_provision Debug menuconfig
```

Under **PF1550 Example Configuration**: I2C pins, profile selection, optional
strap GPIO drive.
