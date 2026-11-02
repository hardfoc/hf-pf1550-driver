---
hide:
  - navigation
  - toc
---

<div class="hf-hero" markdown>
<span class="hf-kicker">HardFOC driver</span>

# PF1550 Driver

<p>A header-only C++20 driver for the NXP PF1550 power management IC. It identifies the chip, applies board power
profiles, sets rail voltages and the USB input limit, reads OTP, and reports rail state and faults, on any MCU with
I2C.</p>

[Quickstart](quickstart.md){ .md-button .md-button--primary }
[API guide](api-guide.md){ .md-button }
[Source on GitHub](https://github.com/hardfoc/hf-pf1550-driver){ .md-button }
</div>

The driver has no platform code. You give it a small class that reads and writes one register over I2C and sets
three strap pins; it does the rest: identification, ordered register sequences, code tables, the OTP unlock
sequence and fault decoding.

<div class="hf-chip" markdown>
<div class="hf-chip__head">
<span class="hf-chip__kicker">PMIC for low-power application processors</span>
<span class="hf-chip__name">PF1550</span>
<span class="hf-chip__vendor">NXP Semiconductors</span>
</div>

Regulators
:   SW1, SW2, SW3 buck converters (1.0 A each); LDO1 and LDO3 (300 mA), LDO2 (400 mA); VSNVS 3.0 V; VREFDDR

Charger
:   Single-cell Li-ion / Li-polymer linear charger; VBUSIN 4.1 V to 6.0 V, input limit 10 mA to 1500 mA

Host interface
:   I2C, fast mode up to 400 kHz, 7-bit address 0x08 (OTP-selectable 0x08 to 0x0F)

Control pins
:   STANDBY, PWRON, ONKEY, WDI (inputs); INTB, RESETBMCU (open-drain outputs)

Configuration
:   OTP: start-up sequence, set points, charger parameters; A0EP parts ship unprogrammed

Package
:   40-pin HVQFN, 5 mm × 5 mm, exposed pad; −40 °C to 105 °C (MC34) or 85 °C (MC32)

[Data sheet (Rev. 7)](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf) ·
[Product page](https://www.nxp.com/products/PF1550) ·
[Register map](registers.md)
</div>

## What you get

- **One object per PMIC.** `PF1550<YourBus>` exposes identification, register access, profiles, rail set points,
  strap pins, OTP, interrupts and diagnostics as plain member functions. See [API guide](api-guide.md).
- **Board profiles as data.** A profile is a table of register writes with optional waits, applied in order; the
  Portenta H7 default and carrier profiles are included. See [Portenta H7 profiles](portenta-h7.md).
- **Datasheet code tables.** SW, LDO and VBUS-limit codes convert to millivolts and milliamps from the data
  sheet's tables, checked at compile time. See [Rails and OTP](rails-and-otp.md).
- **A diagnostic snapshot.** One call reads identity, state, every rail's set point and enables, the VBUS limit
  and latched faults, for a monitor task. See [API guide](api-guide.md#read-a-diagnostic-snapshot).
- **Zero-overhead portability.** The port is a CRTP class: no virtual calls, no heap, no RTOS dependency.
  See [Porting](porting.md).
- **Tested without hardware.** Host unit tests run the driver against a register model of the chip. See
  [Testing and CI](testing.md).

## Where to go next

<div class="grid cards" markdown>

-   :material-rocket-launch-outline: **[Quickstart](quickstart.md)**

    ---

    Wire the bus, write the port, read the rails in about 15 lines.

-   :material-chip: **[Hardware](hardware.md)**

    ---

    Pins, bus settings, supplies, rails and the parts.

-   :material-layers-outline: **[Architecture](architecture.md)**

    ---

    The three layers and what each one owns.

-   :material-code-braces: **[API guide](api-guide.md)**

    ---

    Identify, configure, read, diagnose and handle faults, task by task.

-   :material-swap-horizontal: **[Porting](porting.md)**

    ---

    Implement the bus interface for your MCU.

-   :material-book-open-variant: **[API reference](api-reference.md)**

    ---

    Every class and member, generated from the headers.

</div>
