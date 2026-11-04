# Configuration

What you can configure, where: board profiles, run-time rail settings, the SW1 / SW2 code table, interrupt masks
and the build.

The snippets are compiled by the host CI from [`tests/docs/configuration.cpp`](../tests/docs/configuration.cpp).

## Where configuration lives

| Layer | Where | Changes |
| --- | --- | --- |
| OTP | Inside the PF1550 | Start-up sequence and set points; fixed on programmed parts, empty on A0EP |
| Profile | A `profiles::RegisterWrite` table | Everything your board needs after power-up, in order |
| Run time | `Set*()` and `WriteRegister()` calls | One register at a time |
| Driver | `SetSwDvsEnabled()`, the constructor address | How the driver decodes and addresses, not the chip |

## Write your own profile

```cpp
--8<-- "tests/docs/configuration.cpp:custom-profile"
```

Each entry is `{reg, value, delay_us}`: the register address, the byte, and how long to wait after writing it
(0 for none). The driver writes them in order and stops at the first failure, so put a rail's set point before its
enable and a supply before the rails it feeds. The waits go through your port's `DelayUs()`, which every port must
define (the driver does not compile without it).

Rules worth following in a profile:

- Write a rail's voltage register before its control register.
- Leave out SW voltage registers the part treats as read-only: SW3's always, SW1's and SW2's when DVS is disabled
  ([Rails and OTP](rails-and-otp.md)).
- Setting `LDOx_LS_EN` (bit 4 of `LDO1_CTRL` / `LDO3_CTRL`) turns the LDO into a load switch; the data sheet
  does not allow clearing it again.

## Run-time settings

| Setting | Call | Register |
| --- | --- | --- |
| SW1 / SW2 RUN set point (DVS-enabled bucks only) | `SetSwVoltageMv(n, mv)`, `SetSwVoltage(n, SwVoltageCode)` | `SWn_VOLT` |
| LDO1 / LDO2 / LDO3 set point | `SetLdoVoltage(n, LdoVoltageCode)` | `LDOn_VOLT` |
| VBUS input current limit | `SetVbusCurrentLimitMa(10 / 100 / 500 / 1000 / 1500)` | `VBUS_INLIM_CNFG` (0x94) |
| Rail enables per mode | `WriteRegister(Register::Sw2Ctrl, SwCtrlBits::kEn \| ...)` | `SWn_CTRL`, `LDOn_CTRL` |
| Buck current limit | `WriteRegister(Register::Sw3Ctrl1, 0..3)` | `SWn_CTRL1` bits 1:0 |
| STANDBY polarity and delay | `WriteRegister(Register::PwrCtrl0, ...)` | `PWRCTRL0` (0x58) |
| Interrupt masks | `SetInterruptMaskAll(bool)`, or `WriteRegister(Register::LdoIntMask0, ...)` per group | `*_INT_MASK*` |

## Declare the SW1 / SW2 code table

```cpp
--8<-- "tests/docs/configuration.cpp:dvs"
```

`SetSwDvsEnabled(n, enabled)` decides how the snapshot turns `SW1_VOLT` and `SW2_VOLT` into millivolts and whether
`SetSwVoltage()` / `SetSwVoltageMv()` may write them; it writes nothing itself. Set it to what the part's
`OTP_SWx_DVS_SEL` says. The default (both disabled) matches the Portenta H7.

## Address

The constructor's second argument is the 7-bit address, 0x08 by default. A part whose OTP selects another address
(0x09 to 0x0F) needs it there; `I2C_ADDR` (0x68) shows which one the part uses.

## Build options

| Option | Default | Effect |
| --- | --- | --- |
| `HF_PF1550_ENABLE_WARNINGS` | `OFF` | `-Wall -Wextra -Wpedantic` on targets that link the driver |
| `HF_PF1550_BUILD_TESTS` | `OFF` | Host unit tests and the documentation snippets ([Testing and CI](testing.md)) |
| `HF_PF1550_VERSION_MAJOR` / `MINOR` / `PATCH` | in `cmake/hf_pf1550_build_settings.cmake` | Written into `pf1550_version.h`; `GetDriverVersion()` returns it |

The ESP32 examples add their own Kconfig options (I2C pins and speed, profile choice, strap GPIOs); they are listed
on [External provisioning](provisioning.md#kconfig-options).
