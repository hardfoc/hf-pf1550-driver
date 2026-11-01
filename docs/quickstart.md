---
layout: default
title: "⚡ Quick start"
nav_order: 4
parent: "📚 Documentation"
permalink: /docs/quickstart/
---

# Quick start

## 1. Implement the bus adapter (CRTP)

Your platform type must inherit `pf1550::BusInterface<YourBus>`:

| Method | Contract |
|--------|----------|
| `bool EnsureInitialized() noexcept` | I2C bus ready |
| `bool Write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) noexcept` | Register write |
| `bool Read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) noexcept` | Register read |
| `void GpioSet(CtrlPin, GpioSignal) noexcept` | Optional strap pins (STANDBY, USB rails) |
| `void DelayUs(uint32_t us) noexcept` | Optional — profile pacing |

## 2. Construct and probe

```cpp
#include "pf1550.hpp"

YourBus bus;
pf1550::PF1550<YourBus> pmic(&bus, pf1550::kDefaultI2cAddress);

if (!pmic.EnsureInitialized()) {
  // I2C failure or DEVICE_ID != 0x7C
}

uint8_t status{};
pmic.ReadPmicStatus(status);
```

## 3. Apply board profile (Portenta / Synapse)

```cpp
// After power-up, before USB — PMIC state persists across MCU reset
pmic.SetPowerMode(pf1550::PowerMode::Run);
pmic.SetUsbRails(true, true);
pmic.ApplyPortentaH7DefaultProfile();
```

## 4. Custom profile

{% raw %}
```cpp
std::array<pf1550::profiles::RegisterWrite, 2> custom = {{
    {0x4F, 0x00, 0},
    {0x50, 0x0F, 0},
}};
pmic.ApplyProfile(custom);
```
{% endraw %}

**Next:** [Hardware setup →](hardware_setup.md) · [API reference →](api_reference.md)
