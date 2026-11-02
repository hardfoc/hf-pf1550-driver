# Portenta H7 profiles

What the two built-in profiles write to an unprogrammed PF1550 on an Arduino Portenta H7 module, in which order,
and when to use each.

Both profiles are tables in [`pf1550_profiles.hpp`](../inc/pf1550_profiles.hpp), applied in order by
`ApplyProfile()`. They are written for the MC34PF1550A0EP (unprogrammed OTP) on the Portenta H7. On any other
board, write your own table ([Configuration](configuration.md#write-your-own-profile)).

## Which one

| Profile | Use it when |
| --- | --- |
| Default: `ApplyPortentaH7DefaultProfile()` | You want the order of the Portenta's original firmware; SW1 and SW2 turn off in STANDBY |
| Carrier: `ApplyPortentaH7CarrierProfile()` | A carrier board draws +3V3 from the module, or the LDO inputs hang off SW1 |

Their names for logs are `profiles::kPortentaH7DefaultName` (`portenta_h7_default`) and
`profiles::kPortentaH7CarrierName` (`portenta_h7_carrier`).

The carrier profile differs from the default in three writes:

1. **SW1 comes first.** `SW1_VOLT = 0x06` and `SW1_CTRL = 0x0F`, then a 5 ms wait, before any LDO register: on
   the Portenta the LDO inputs are fed from SW1.
2. **SW1 stays on in STANDBY.** `SW1_CTRL = 0x0F` instead of `0x01` (on in RUN only).
3. **SW2 stays on in STANDBY.** `SW2_CTRL = 0x0F` instead of `0x01`, so the carrier's +3V3 survives a STANDBY
   transition.

The default writes `SW1_VOLT = 0x07` where the carrier writes `0x06`.

## Rails on the Portenta H7

Rail use from the Portenta H7 module schematic. The middle column is what each code in the carrier profile means
(SW1 and SW2 in the DVS-disabled table); the warning below explains why a buck can still run at its OTP value.

| Rail (schematic) | Carrier profile | Feeds |
| --- | --- | --- |
| SW1 (BUCK1, +3V1SW) | 3.0 V, code 0x06 | USB / ULPI, SDRAM, Ethernet; the LDO inputs |
| SW2 (BUCK2, +VOUT) | 3.3 V, code 0x07 | Carrier +3V3, JTAG VTref |
| SW3 (BUCK3) | 3.3 V, code 0x0F (OTP value on the modules read so far) | STM32H747, QSPI flash |
| LDO1 | 1.0 V, code 0x05 | MIPI bridge |
| LDO2 | 1.8 V, code 0x00 | MIPI, USB / ULPI |
| LDO3 | 1.2 V, code 0x09 | STM32 DSI, Ethernet PHY |

!!! warning "Writes the data sheet calls read-only"
    Both profiles write `SWx_VOLT`, `SWx_STBY_VOLT` and `SWx_SLP_VOLT` for all three bucks. SW3's registers are
    read-only (no DVS), and so are SW1's and SW2's when their DVS is disabled, which is the case for 3.0 V and
    3.3 V set points. The part ignores those writes, so the rails run at their OTP values. Read the snapshot after
    a profile to see what the bucks really use.

## The carrier profile, write by write

The writes, in table order (two rows here cover two registers each):

| Register | Value | Effect |
| --- | --- | --- |
| `SW1_VOLT` (0x32) | `0x06` | 3.0 V in RUN |
| `SW1_STBY_VOLT`, `SW1_SLP_VOLT` | `0x05` | 2.5 V in STANDBY and SLEEP |
| `SW1_CTRL` (0x35) | `0x0F` | On in RUN, STANDBY, SLEEP, low power; then wait 5 ms |
| `LDO2_VOLT` (0x4F) | `0x00` | 1.8 V |
| `LDO1_VOLT` (0x4C) | `0x05` | 1.0 V |
| `LDO1_CTRL` (0x4D) | `0x0F` | On in every mode, low power |
| `LDO3_VOLT` (0x52) | `0x09` | 1.2 V |
| `LDO3_CTRL` (0x53) | `0x0F` | On in every mode, low power |
| `PWRCTRL0` (0x58) | `0x03` | `STANDBYDLY = 3`, `STANDBYINV = 0`: STANDBY high means standby |
| `LED_PWM` (0x9C) | `0x80` | LED enabled at 0/32 duty; wait 1 ms |
| `LED_CNFG` (0x9E) | `0x20` | `LEDOVRD`: software owns the LED, so it stays off; wait 1 ms |
| `SW3_CTRL1` (0x42) | `0x02` | SW3 current limit 1.5 A typical; wait 1 ms |
| `VBUS_INLIM_CNFG` (0x94) | `0xA0` | VBUS input limit 1500 mA |
| `SW2_VOLT` (0x38) | `0x07` | 3.3 V in RUN |
| `SW2_STBY_VOLT`, `SW2_SLP_VOLT` | `0x05` | 2.5 V |
| `SW2_CTRL` (0x3B) | `0x0F` | On in every mode, low power |
| `SW3_VOLT` (0x3E) | `0x0F` | 3.3 V in RUN |
| `SW3_STBY_VOLT`, `SW3_SLP_VOLT` | `0x0E` | 3.2 V |
| `SW3_CTRL` (0x41) | `0x0F` | On in every mode, low power |
| `LDO2_CTRL` (0x50) | `0x0F` | LDO2 on; always the last write |

`LDO2_CTRL` is last in both profiles. That order comes from the Portenta's original start-up code, and earlier
bring-up notes report I2C trouble when it was written sooner; the data sheet gives no ordering rule for it.

## Strap pins

| Signal (STM32H747 pin) | Normal level | Driver call |
| --- | --- | --- |
| PMIC STANDBY (PJ0) | Low: RUN | `SetPowerMode(PowerMode::Run)` |
| USB_VBUS_EN (PJ4) | High | `SetUsbRails(true, ...)` |
| USB_OTG_EN (PJ6) | High | `SetUsbRails(..., true)` |
| PMIC INT (PK0) | Input | Read by your code |

The PF1550 sits on I2C1 (PB6 SCL, PB7 SDA) at 0x08.

## When to apply

Apply the profile early, after the strap pins and before USB or Ethernet start. The PF1550 keeps its registers
across an MCU reset, so a profile that was applied once stays in force until the PMIC is powered down; after
changing a profile, power the module down completely (USB and battery off) before you judge the result.

Next: [API guide](api-guide.md).
