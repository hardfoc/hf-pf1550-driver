---
layout: default
title: "ESP32-C6 external provisioning"
nav_order: 6
parent: "📚 Documentation"
permalink: /docs/esp32-provisioning/
---

# ESP32-C6 external PMIC provisioning

Use an **ESP32-C6 dev kit** to configure the PF1550 over exposed I2C test pads
**before** the host MCU (STM32H747) is powered or released from reset. This is
the same workflow the Arduino team will use on custom boards where PMIC I2C is
brought out for factory programming.

## When to use

| Scenario | Use ESP32 provision? |
|----------|---------------------|
| First power-on, bare module, no STM32 firmware | **Yes** |
| Carrier +3V3 / JTAG VTref = 0 V, MCU runs on SW3 | **Yes** — apply carrier profile |
| Manufacturing CM7 already flashed with early PMIC init | No — firmware handles it |
| OTP-programmed PMIC, rails already correct | Probe/diagnostics only |

## Hardware connections

### Minimum (I2C only)

| Signal | ESP32-C6 | PF1550 / module |
|--------|----------|-----------------|
| SDA | GPIO4 (default) | I2C SDA @ **0x08** |
| SCL | GPIO5 (default) | I2C SCL |
| GND | GND | GND |

Power the module from USB or bench supply. The PMIC I2C domain is active once
VSYS is up — the STM32 does **not** need to run.

### Optional strap GPIOs

If STANDBY or USB enable pins are wired to test points, enable strap drive in
menuconfig:

```text
Component config → PF1550 Example Configuration
  → Drive PMIC strap GPIOs from ESP32-C6
```

| Strap | Portenta pin | Boot value |
|-------|--------------|------------|
| STANDBY | PJ0 | LOW = RUN |
| USB_VBUS_EN | PJ4 | HIGH |
| USB_OTG_EN | PJ6 | HIGH |

On a minimal jig, straps may already be pulled correctly on the PCB — I2C-only
provisioning still works if STANDBY is pulled low and USB rails are strapped.

## Software workflow

```bash
git clone --recursive https://github.com/hardfoc/hf-pf1550-driver.git
cd hf-pf1550-driver/examples/esp32
git submodule update --init --recursive

# Build the provisioning app
./scripts/build_app.sh pf1550_esp32c6_provision Debug

# Flash and monitor (115200 baud)
./scripts/flash_app.sh pf1550_esp32c6_provision Debug
./scripts/idf_flash_monitor.sh pf1550_esp32c6_provision Debug
```

### Expected serial output (success)

```text
I (xxx) pf1550_prov: PF1550 external provision — hf-pf1550 v...
I (xxx) pf1550_prov: Applying profile 'portenta_h7_carrier'
I (xxx) pf1550_prov: Self-test: PASS device_id=1 state_run=1 rails=1 worst=Info
I (xxx) pf1550_prov: --- Bench checklist (measure at module) ---
I (xxx) pf1550_prov:   SW2 +3V3:    3300 mV ±3% ...
I (xxx) pf1550_prov: Provisioning complete — safe to remove ESP32 and bring up host MCU
```

### After provisioning

1. **Disconnect ESP32** from I2C (avoid bus contention with STM32).
2. **Cold power cycle** if this is the first profile apply (PMIC state survives
   soft reset).
3. Flash manufacturing firmware or release MCU reset.
4. Verify UART7 banner: `[pmic] carrier profile OK ...` (see
   [Portenta bring-up doc](https://github.com/N3b3x/pw-controller-sw/blob/main/docs/hardware/pmic-bringup-portenta-h7.md)).

## App matrix

| App | Purpose | CI-safe? |
|-----|---------|----------|
| `pf1550_esp32c6_probe` | DEVICE_ID + optional profile | Yes (NACK OK) |
| `pf1550_esp32c6_register_dump` | Raw register dump | Yes |
| `pf1550_esp32c6_diagnostics` | Snapshot + self-test loop | Yes |
| **`pf1550_esp32c6_provision`** | **Full carrier profile + checklist** | Yes |

## Kconfig reference

| Option | Default | Notes |
|--------|---------|-------|
| `PF1550_I2C_SDA_GPIO` | 4 | Override for custom jig |
| `PF1550_I2C_SCL_GPIO` | 5 | |
| `PF1550_I2C_FREQ_HZ` | 400000 | Up to 1 MHz on Portenta |
| `PF1550_PROFILE_CARRIER` | on | SW1-first, SW2_CTRL=0x0F |
| `PF1550_GPIO_STRAPS_ENABLE` | off | Enable for strap wiring |

Run `./scripts/build_app.sh pf1550_esp32c6_provision Debug menuconfig` to edit.

## Safety notes

- Apply profiles only on **lab / unprogrammed** PMIC parts unless you understand
  OTP lock behaviour on programmed devices.
- Do **not** connect ESP32 and STM32 to the same I2C bus simultaneously.
- A full **power cycle** (remove USB ≥10 s) is required after the first profile
  apply — soft reset alone may not change rail voltages.

**Next:** [Hardware setup →](hardware_setup.md) · [Portenta profile →](portenta-profile.md)
