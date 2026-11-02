# Quickstart

Talk to a PF1550 from your MCU: wire the I2C bus, write a small port class, identify the chip and read its rails.

!!! warning "Reading is safe; writing changes the board's supplies"
    This example only reads. The PF1550 powers the board it sits on, often including the MCU that talks to it:
    a wrong rail write can brown out the processor or over-volt a peripheral. Read the
    [Portenta H7 profiles](portenta-h7.md) page before you apply a profile.

## 1. Wire it

| MCU | PF1550 | Notes |
| --- | --- | --- |
| I2C SDA | SDA (pin 2) | Pull up to VDDIO; up to 400 kHz |
| I2C SCL | SCL (pin 3) | Pull up to VDDIO |
| GPIO out (optional) | STANDBY (pin 7) | High moves RUN to STANDBY (default polarity) |
| GPIO in (optional) | INTB (pin 9) | Open drain, active low: pull it up |
| GND | GND | Common ground |

The PF1550 answers on I2C once it has VSYS and its I/O supply VDDIO, so the host does not have to be the processor
the PMIC supplies: an [external jig](provisioning.md) can configure it first. [Hardware](hardware.md) has the full
pin list and the electrical limits.

## 2. Add the driver

Add the repository as a submodule and link the `hf::pf1550` CMake target (or add `inc/` and the generated
`pf1550_version.h` to your include path and build with C++20). [Installation](installation.md) covers CMake and
ESP-IDF.

```bash
git submodule add https://github.com/hardfoc/hf-pf1550-driver.git external/hf-pf1550-driver
```

## 3. Write the port

The port is the only platform code. It reads and writes one register, brings the bus up and sets the strap pins.
Replace the `platform_*` calls with your SDK's.

```cpp title="my_i2c.hpp"
--8<-- "tests/docs/my_i2c.hpp:port"
```

[Porting](porting.md) explains each method and the strap-pin levels.

## 4. Read the rails

```cpp title="main.cpp"
--8<-- "tests/docs/quickstart.cpp:quickstart"
```

What each step does:

1. `EnsureInitialized()` calls your port's `EnsureInitialized()`, reads DEVICE_ID (register 0x00) and checks it
   is 0x7C. Later calls return at once.
2. `ReadPmicState()` decodes STATE_INFO (0x67). `Run` means the start-up sequence has finished and the
   OTP-enabled regulators are on.
3. `ReadDiagnosticSnapshot()` reads about 30 registers: identity, state, charger and VBUS sense, every rail's set
   point and enables, and the latched faults.

This example is built by the host CI, so it compiles against the current headers.

## If it does not work

| Symptom | First thing to check |
| --- | --- |
| `EnsureInitialized()` returns `false`, `DeviceNotFound` set | Pull-ups, SDA / SCL swapped, the 7-bit address (0x08, not 0x10) |
| `WrongDeviceId` set | Another device answers at 0x08, or the part's OTP moved its address |
| Rails read 0 mV | The snapshot read failed part-way: check `read_ok` and `GetErrorFlags()` |
| A rail's set point does not match its measured voltage | SW1 / SW2 DVS mode ([Rails and OTP](rails-and-otp.md#sw1-and-sw2-two-code-tables)) |

## Next

- [API guide](api-guide.md): profiles, rail settings, status, faults and OTP.
- [Portenta H7 profiles](portenta-h7.md): what the two built-in profiles write and in which order.
- [Examples](examples.md): ESP32-C6 applications for probing, register dumps, diagnostics and provisioning.
