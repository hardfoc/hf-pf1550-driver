---
layout: default
title: "⚙️ Configuration"
nav_order: 6
parent: "📚 Documentation"
permalink: /docs/configuration/
---

# Configuration

The PF1550 driver exposes board-level configuration through three layers:

1. **Compile-time constants** in `pf1550_registers.hpp` and the
   `pf1550::profiles::*` tables.
2. **Profile selection** at runtime via `ApplyPortentaH7DefaultProfile()`
   or `ApplyPortentaH7CarrierProfile()` (or a custom
   `std::span<const RegisterWrite>` you build yourself).
3. **Per-deployment tuning** of voltage codes, VBUS limit, charger LED,
   STANDBY polarity, and interrupt masks via the `SetXxx` and
   `WriteRegister` API.

---

## 1. Choosing a profile

| Profile | Use when |
|---------|----------|
| `kPortentaH7Default` | You boot the stock Portenta H7 with no carrier, replicate the Arduino bootloader, and don't care that SW2 turns off in STANDBY. |
| **`kPortentaH7Carrier`** | **Recommended for pw-controller and any custom carrier that draws +3V3 from the module.** Sequences SW1 (LDO input rail) first, sets `SW2_CTRL=0x0F` so the carrier 3V3 survives STANDBY transitions, and disables the orange CHGB LED. |

The two tables only differ in **a few register writes**; see
`pf1550_profiles.hpp` for the exact diff.

---

## 2. Building a custom profile

{% raw %}
```cpp
#include "pf1550_profiles.hpp"

constexpr std::array<pf1550::profiles::RegisterWrite, 4> kMyMinimalProfile = {{
    {0x32, 0x06,    0},  // SW1 RUN = 3.0 V
    {0x35, 0x0F, 5000},  // SW1 enable; 5 ms settling
    {0x38, 0x07,    0},  // SW2 RUN = 3.3 V
    {0x3B, 0x0F,    0},  // SW2 enable in RUN/STBY/SLEEP/LPWR
}};

pmic.ApplyProfile(kMyMinimalProfile);
```
{% endraw %}

Each `RegisterWrite` entry takes a target register, the byte to write,
and an optional `delay_us` to wait *after* the write. The driver
applies them strictly in order so dependencies (`SW1` before LDOs on
Portenta) are honoured.

---

## 3. Per-rail tuning at runtime

| Setting | API |
|---------|-----|
| Change SW1 RUN voltage | `SetSwVoltage(1, SwVoltageCode::V3_0)` |
| Change LDO2 voltage | `SetLdoVoltage(2, LdoVoltageCode::V1_8)` |
| VBUS input current limit | `SetVbusCurrentLimitMa(1500)` |
| SW3 current limit (Portenta) | `WriteRegister(Register::Sw3Ctrl1, 0x02)` |
| Disable charger LED | `WriteRegister(Register::ChargerLedCtrl, 0x20)` |

**Important — cold boot rule.** PF1550 state survives MCU reset. After
writing voltage registers, observe the new value only after a **full
power cycle** of the module (USB + battery removed). See
[`docs/troubleshooting.md`](troubleshooting.md).

---

## 4. Interrupt masking

```cpp
pmic.SetInterruptMaskAll(true);   // mask everything (powers up this way)
pmic.SetInterruptMaskAll(false);  // unmask all categories
```

Granular unmask via `WriteRegister(Register::SwIntMask0, 0x..)` etc.
The mapping is documented in
[`docs/datasheet/PF1550-i2c-register-reference.md`](datasheet/PF1550-i2c-register-reference.md).

---

## 5. Diagnostic-snapshot tuning

`DiagnosticSnapshot` is fixed-layout POD — there is no compile-time
toggle to drop fields. To reduce I²C traffic in tight monitor loops,
call `ReadInterruptCategory()` (single byte) and only call
`ReadLatchedFaults()` when the summary byte is non-zero.

---

## 6. CMake options

| Option | Default | Effect |
|--------|---------|--------|
| `HF_PF1550_ENABLE_WARNINGS` | `OFF` | Adds `-Wall -Wextra -Wpedantic` to the driver target. |
| `HF_CORE_ENABLE_PF1550` (host project) | `OFF` | Builds the `Pf1550Handler` in `hf-core`. |
| `HF_PF1550_VERSION_*` | semver from `hf_pf1550_build_settings.cmake` | Surfaces via `GetDriverVersion()`. |

ESP-IDF component usage is described in
[`cmake_integration.md`](cmake_integration.md).
