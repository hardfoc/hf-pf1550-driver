---
layout: default
title: "PF1550 I2C register reference (extract)"
description: "Auto-generated readable extract from NXP PF1550 Rev.7 Section 12"
nav_order: 12
parent: "Datasheet & links"
permalink: /docs/datasheet/PF1550-i2c-register-reference/
---

# PF1550 I2C register reference (readable extract)

> **Auto-generated** from `_local_reference/datasheet/PF1550.pdf` via
> `scripts/extract_register_reference.sh`. Do not hand-edit — regenerate after
> datasheet updates. Authoritative source: [NXP PF1550.pdf](https://www.nxp.com/docs/en/data-sheet/PF1550.pdf).

For the **driver-maintained subset** used in code, see [PF1550-register-map.md](PF1550-register-map.md).

## Section 12 — Register map (plain text)

```text
12 Register map
12.1 Specific PMIC Registers (Offset is 0x00)
The following pages contain description of the various registers in the PF1550.

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

80 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 86. Register DEVICE_ID - ADDR 0x00
Name

Bit

R/W

Default

DEVICE_ID

2 to 0

R

100

Description
Loaded from fuses
000 — Devices with "00" at the end of the part number, such as
"PF1500"
001 — Future use
010 — Future use
011 — Future use
100 — Devices with "50" at the end of the part number, such as
"PF1550"
101 — Future use
110 — Future use
111 — Future use

FAMILY

7 to 3

R

01111

Identifies PMIC
01111 — 0b0_1111 for "15" used to denote the "PF1550"

Table 87. Register OTP_FLAVOR - ADDR 0x01
Name

Bit

R/W

Default

Description

UNUSED

7 to 0

R

0x00

Blown by ATE to indicate flavor of OTP used
0x00 — OTP not burned
0x01 — A1
0x02 — A2
0x03 — A3
continues...

Description

Table 88. Register SILICON_REV - ADDR 0x02
Name

Bit

R/W

Default

METAL_LAYER_REV

2 to 0

R

001

Unused

FULL_LAYER_REV

5 to 3

R

010

Unused

FAB_FIN

7 to 6

R

00

Unused

Table 89. Register INT_CATEGORY - ADDR 0x06
Name

Bit

R/W

Default

CHG_INT

0

R

0

This bit is set high if any of the charger interrupt status bits are set
0 — No charger interrupt bit is set, cleared, or did not occur
1 — "OR" function of all charger interrupt status bit

SW1_INT

1

R

0

This bit is set high if any of the Buck 1 interrupt status bits are set
0 — SW1 interrupts cleared or did not occur
1 — Any of the SW1 interrupt status bits are set

SW2_INT

2

R

0

This bit is set high if any of the Buck 2 interrupt status bits are set
0 — SW2 interrupts cleared or did not occur
1 — Any of the SW2 interrupt status bits are set

SW3_INT

3

R

0

This bit is set high if any of the Buck 3 interrupt status bits are set
0 — SW3 interrupts cleared or did not occur
1 — any of the SW3 interrupt status bits are set

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

81 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 89. Register INT_CATEGORY - ADDR 0x06...continued
Name

Bit

R/W

Default

LDO_INT

4

R

0

Description
This bit is set high if any of the LDO interrupt status bits are set.
This includes LDO1, LDO2, and LDO3.
0 — LDO interrupts cleared or did not occur
1 — Any of the LDO interrupt status bits are set

ONKEY_INT

5

R

0

This bit is set high if any of the interrupts associated with ONKEY
push-button are set.
0 — ONKEY related interrupts cleared or did not occur
1 — Any of the ONKEY interrupt status bits are set

TEMP_INT

6

R

0

This bit is set if any of the interrupts associated with the die
temperature monitor are set
0 — PMIC junction temperature related interrupts cleared or did
not occur
1 — any of the PMIC junction temperature interrupts status bits
are set

MISC_INT

7

R

0

This bit is set if interrupts not covered by the above mentioned
categories occur
0 — Other interrupts (not covered by categories above) cleared,
or did not occur
1 — Status bit of other interrupts (not covered by categories
above) is set

Table 90. Register SW_INT_STAT0 - ADDR 0x08
Name

Bit

R/W

SW1_LS_I

0

RW1C

[1]

0

SW1 low-side current limit interrupt status. This bit is set if the
current limit fault persists for longer than the debounce time.
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

SW2_LS_I

1

RW1C

0

SW2 low-side current limit interrupt status. This bit is set if the
current limit fault persists for longer than the debounce time.
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

SW3_LS_I

2

RW1C

0

SW3 low-side current limit interrupt status. This bit is set if the
current limit fault persists for longer than the debounce time.
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

UNUSED

7 to 3

—

—

Unused

[1]

Default

Description

Read or Write 1 to clear the bit

Table 91. Register SW_INT_MASK0 - ADDR 0x09
Name

Bit

R/W

Default

SW1_LS_M

0

RW

1

PF1550

Product data sheet

Description
SW1 low-side current limit interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

82 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 91. Register SW_INT_MASK0 - ADDR 0x09...continued
Name

Bit

R/W

Default

SW2_LS_M

1

RW

1

Description
SW2 low-side current limit interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

SW3_LS_M

2

RW

1

SW3 low-side current limit interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

UNUSED

7 to 3

—

—

Unused

Table 92. Register SW_INT_SENSE0 - ADDR 0x0A
Name

Bit

R/W

Default

Description

SW1_LS_S

0

R

0

SW1 low-side current limit interrupt sense. Sense is high as long
as fault persists (post-debounce).
0 — Fault removed
1 — Fault exists

SW2_LS_S

1

R

0

SW2 low-side current limit interrupt sense. Sense is high as long
as fault persists (post-debounce).
0 — Fault removed
1 — Fault exists

SW3_LS_S

2

R

0

SW3 low-side current limit interrupt sense. Sense is high as long
as fault persists (post-debounce)
0 — Fault removed
1 — Fault exists

UNUSED

7 to 3

—

—

Unused

Table 93. Register SW_INT_STAT1 - ADDR 0x0B
Name

Bit

R/W

SW1_HS_I

0

RW1C

SW2_HS_I

1

SW3_HS_I

UNUSED
[1]

Default
[1]

Description

0

SW1 high-side current limit interrupt
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

RW1C

0

SW2 high-side current limit interrupt
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

2

RW1C

0

SW3 high-side current limit interrupt
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

7 to 3

—

—

Unused

Read or Write 1 to clear the bit

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

83 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 94. Register SW_INT_MASK1 - ADDR 0x0C
Name

Bit

R/W

Default

SW1_HS_M

0

RW

1

Description
SW1 high-side current limit interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

SW2_HS_M

1

RW

1

SW2 high-side current limit interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

SW3_HS_M

2

RW

1

SW3 high-side current limit interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

UNUSED

7 to 3

—

—

Unused

Table 95. Register SW_INT_SENSE1 - ADDR 0x0D
Name

Bit

R/W

Default

Description

SW1_HS_S

0

R

0

SW1 high-side current limit interrupt sense. This bit should not
toggle within a switching cycle (at buck switching frequency), but
report the sense status within the switching cycle.
0 — Fault removed
1 — Fault exists

SW2_HS_S

1

R

0

SW2 high-side current limit interrupt sense. This bit should not
toggle within a switching cycle (at buck switching frequency), but
report the sense status within the switching cycle.
0 — Fault removed
1 — Fault exists

SW3_HS_S

2

R

0

SW3 high-side current limit interrupt sense. This bit should not
toggle within a switching cycle (at buck switching frequency), but
report the sense status within the switching cycle.
0 — Fault removed
1 — Fault exists

UNUSED

7 to 3

—

—

Unused

Table 96. Register SW_INT_STAT2 - ADDR 0x0E
Name

Bit

R/W

SW1_DVS_DONE_I

0

RW1C

[1]

0

Interrupt to indicate SW1 DVS complete. This interrupt should
occur every time regulator output voltage is changed (either via
2
I C within a given state, or if there is change in voltage when
transitioning states, Run to Standby, for example).
0 — DVS not complete and/or bit cleared
1 — DVS complete

SW2_DVS_DONE_I

1

RW1C

0

Interrupt to indicate SW2 DVS complete. This interrupt should
occur every time regulator output voltage is changed (either via
2
I C within a given state, or if there is change in voltage when
transitioning states, Run to Standby, for example).
0 — DVS not complete and/or bit cleared
1 — DVS complete

PF1550

Product data sheet

Default

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

84 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 96. Register SW_INT_STAT2 - ADDR 0x0E...continued
Name

Bit

R/W

Default

UNUSED

7 to 2

—

—

[1]

Description
Unused

Read or Write 1 to clear the bit

Table 97. Register SW_INT_MASK2 - ADDR 0x0F
Name

Bit

R/W

Default

SW1_DVS_DONE_M

0

RW

1

Mask for interrupt that indicates SW1 DVS complete
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

SW2_DVS_DONE_M

1

RW

1

Mask for interrupt that indicates SW2 DVS complete
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

7 to 2

—

—

Unused

UNUSED

Description

Table 98. Register SW_INT_SENSE2 - ADDR 0x10
Name

Bit

R/W

Default

SW1_DVS_S

0

R

0

Indicates DVS in progress for SW1
0 — DVS not in progress
1 — DVS in progress

SW2_DVS_S

1

R

0

Indicates DVS in progress for SW2
0 — DVS not in progress
1 — DVS in progress

7 to 2

—

—

Unused

UNUSED

Description

Table 99. Register LDO_INT_STAT0 - ADDR 0x18
Name

Bit

R/W

Default

Description

[1]

0

LDO1 current limit interrupt
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

LDO1_FAULTI

0

RW1C

LDO2_FAULTI

1

RW1C

0

LDO2 current limit interrupt
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

LDO3_FAULTI

2

RW1C

0

LDO3 current limit interrupt
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

7 to 3

—

—

Unused

UNUSED
[1]

Read or Write 1 to clear the bit

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

85 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 100. Register LDO_INT_MASK0 - ADDR 0x19
Name

Bit

R/W

Default

LDO1_FAULTM

0

RW

1

LDO1 current limit fault interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

LDO2_FAULTM

1

RW

1

LDO2 current limit fault interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

LDO3_FAULTM

2

RW

1

LDO3 current limit fault interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

7 to 3

—

—

Unused

UNUSED

Description

Table 101. Register LDO_INT_SENSE0 - ADDR 0x1A
Name

Bit

R/W

Default

LDO1_FAULTS

0

R

0

LDO1 fault interrupt sense
0 — Fault removed
1 — Fault exists

LDO2_FAULTS

1

R

0

LDO2 fault interrupt sense
0 — Fault removed
1 — Fault exists

LDO3_FAULTS

2

R

0

LDO3 fault interrupt sense
0 — Fault removed
1 — Fault exists

7 to 3

—

—

Unused

UNUSED

Description

Table 102. Register TEMP_INT_STAT0 - ADDR 0x20
Name

Bit

R/W

THERM110I

0

RW1C

UNUSED

1

THERM125I

UNUSED
[1]

Default
[1]

Description

0

Die temperature crosses 110 °C interrupt. Bidirectional interrupt.
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

—

—

Unused

2

RW1C

0

Die temperature crosses 125 °C interrupt. Bidirectional interrupt.
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

7 to 3

—

—

Unused

Read or Write 1 to clear the bit

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

86 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 103. Register TEMP_INT_MASK0 - ADDR 0x21
Name

Bit

R/W

Default

THERM110M

0

RW

1

Die temperature crosses 110 °C interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

UNUSED

1

—

—

Unused

THERM125M

2

RW

1

Die temperature crosses 125 °C interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

7 to 3

—

—

Unused

UNUSED

Description

Table 104. Register TEMP_INT_SENSE0 - ADDR 0x22
Name

Bit

R/W

Default

THERM110S

0

R

0

110 °C interrupt sense
0 — Die temperature below 110 °C
1 — Die temperature above 110 °C

UNUSED

1

—

—

Unused

THERM125S

2

R

0

125 °C interrupt sense
0 — Die temperature below 125 °C
1 — Die temperature above 125 °C

7 to 3

—

—

Unused

UNUSED

Description

Table 105. Register ONKEY_INT_STAT0 - ADDR 0x24
Name

Bit

R/W

ONKEY_PUSHI

0

RW1C

ONKEY_1SI

1

ONKEY_2SI

ONKEY_3SI

PF1550

Product data sheet

Default
[1]

Description

0

Interrupt to indicate a push of the ONKEY button. Goes high after
debounce.
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared. Interrupt occurs
whenever ONKEY button is pushed low for longer than the falling
edge debounce setting.
Interrupt also occurs whenever ONKEY button is released high
for longer than the rising edge debounce setting, provided it went
past the falling edge debounce time. In other words, this interrupt
occurs whenever a change in status of the ONKEY_PUSHS
sense bit occurs.

RW1C

0

Interrupt after ONKEY pressed for > 1 s
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

2

RW1C

0

Interrupt after ONKEY pressed for > 2 s
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

3

RW1C

0

Interrupt after ONKEY pressed for > 3 s
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

87 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 105. Register ONKEY_INT_STAT0 - ADDR 0x24...continued
Name

Bit

R/W

Default

ONKEY_4SI

4

RW1C

0

Interrupt after ONKEY pressed for > 4 s
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

ONKEY_8SI

5

RW1C

0

Interrupt after ONKEY pressed for > 8 s
0 — Interrupt cleared or did not occur
1 — Interrupt occurred and/or not cleared

UNUSED

7 to 6

—

—

Unused

[1]

Description

Read or Write 1 to clear the bit

Table 106. Register ONKEY_INT_MASK0 - ADDR 0x25
Name

Bit

R/W

Default

ONKEY_PUSHM

0

RW

1

Interrupt mask for ONKEY_PUSH_I
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

ONKEY_1SM

1

RW

1

Interrupt mask for ONKEY_1SI
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

ONKEY_2SM

2

RW

1

Interrupt mask for ONKEY_2SI
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is NOT pulled low if corresponding
interrupt status bit is set.

ONKEY_3SM

3

RW

1

Interrupt mask for ONKEY_3SI
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

ONKEY_4SM

4

RW

1

Interrupt mask for ONKEY_4SI
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

ONKEY_8SM

5

RW

1

Interrupt mask for ONKEY_8SI
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

7 to 6

—

—

Unused

UNUSED

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

88 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 107. Register ONKEY_INT_SENSE0 - ADDR 0x26
Name

Bit

R/W

Default

ONKEY_PUSHS

0

R

0

Push interrupt sense
0 — ONKEY not pushed low. This bit follows debounced version
of the ONKEY button being released.
1 — ONKEY pushed low. This follows the ONKEY button after the
debounce circuit (debounce is programmable).

ONKEY_1SS

1

R

0

1 s interrupt sense or cleared after ONKEY button is released
0 — ONKEY not pushed low for >1 s or cleared after ONKEY
button is released.
1 — ONKEY pushed and being held low > 1 s. This bit is cleared
when ONKEY_PUSHS goes back to 0 when the push-button is
released.

ONKEY_2SS

2

R

0

2 s interrupt sense or cleared after ONKEY button is released
0 — ONKEY not pushed low for >1 s or cleared after ONKEY
button is released.
1 — ONKEY pushed and being held low > 1 s. This bit is cleared
when ONKEY_PUSHS goes back to 0 when the push-button is
released.

ONKEY_3SS

3

R

0

3 s interrupt sense or cleared after ONKEY button is released
0 — ONKEY not pushed low for >1 s or cleared after ONKEY
button is released
1 — ONKEY pushed and being held low > 1 s. This bit is cleared
when ONKEY_PUSHS goes back to 0 when the push-button is
released.

ONKEY_4SS

4

R

0

4 s interrupt sense or cleared after ONKEY button is released
0 — ONKEY not pushed low for >1 s or cleared after ONKEY
button is released.
1 — ONKEY pushed and being held low > 1 s. This bit is cleared
when ONKEY_PUSHS goes back to 0 when the push-button is
released.

ONKEY_8SS

5

R

0

8 s interrupt sense or cleared after ONKEY button is released
0 — ONKEY not pushed low for >1 s or cleared after ONKEY
button is released.
1 — ONKEY pushed and being held low > 1 s. This bit is cleared
when ONKEY_PUSHS goes back to 0 when the push-button is
released.

7 to 6

—

—

Unused

UNUSED

Description

Table 108. Register MISC_INT_STAT0 - ADDR 0x28
Name

Bit

R/W

Default

Description

[1]

0

Interrupt to indicate completion of transition from STANDBY to
RUN and from SLEEP to RUN
0 — Interrupt cleared or has not occurred
1 — Interrupt has occurred

PWRUP_I

0

RW1C

PWRDN_I

1

RW1C

0

Interrupt to indicate completion of transition from RUN to
STANDBY and from RUN to SLEEP
0 — Interrupt cleared or has not occurred
1 — Interrupt has occurred

PWRON_I

2

RW1C

0

Power on button event interrupt
0 — Interrupt cleared or has not occurred
1 — Interrupt has occurred

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

89 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 108. Register MISC_INT_STAT0 - ADDR 0x28...continued
Name

Bit

R/W

Default

LOW_SYS_WARN_I

3

RW1C

0

LOW_SYS_WARN threshold crossed interrupt
0 — Interrupt cleared or has not occurred
1 — Interrupt has occurred

SYS_OVLO_I

4

RW1C

0

SYS_OVLO threshold crossed interrupt
0 — Interrupt cleared or has not occurred
1 — Interrupt has occurred

7 to 5

—

—

Unused

UNUSED
[1]

Description

Read or Write 1 to clear the bit

Table 109. Register MISC_INT_MASK0- ADDR 0x29
Name

Bit

R/W

PWRUP_M

0

[1]

RW

1

Mask for Interrupt to indicate completion on transition from
STANDBY to RUN and from SLEEP to RUN
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

PWRDN_M

1

RW

1

Mask for Interrupt to indicate completion on transition from RUN
to STANDBY and from RUN to SLEEP
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

PWRON_M

2

RW

1

Power on button event interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

LOW_SYS_WARN_M

3

RW

1

LOW_SYS_WARN threshold crossed interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

SYS_OVLO_M

4

RW

1

SYS_OVLO threshold crossed interrupt mask
0 — Mask removed. INTB pin is pulled low if corresponding
interrupt status bit is set.
1 — Mask enabled. INTB pin is not pulled low if corresponding
interrupt status bit is set.

7 to 5

—

—

Unused

UNUSED
[1]

Default

Description

Asynchronous Set, Read, and Write

Table 110. Register MISC_INT_SENSE0 - ADDR 0x2A
Name

Bit

R/W

Default

PWRUP_S

0

R

0

PF1550

Product data sheet

Description
Sense for interrupt to indicate completion on transition from
STANDBY to RUN and from SLEEP to RUN
0 — Transition not in progress
1 — Transition in progress

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

90 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 110. Register MISC_INT_SENSE0 - ADDR 0x2A...continued
Name

Bit

R/W

Default

PWRDN_S

1

R

0

Interrupt to indicate completion on transition from RUN to
STANDBY and from RUN to SLEEP
0 — Transition not in progress
1 — Transition in progress

PWRON_S

2

R

0

Power on button event interrupt sense
0 — PWRON low
1 — PWRON high

LOW_SYS_WARN_S

3

R

0

LOW_SYS_WARN threshold crossed interrupt sense
0 — SYS > LOW_SYS_WARN
1 — SYS < LOW_SYS_WARN

SYS_OVLO_S

4

R

0

SYS_OVLO threshold crossed interrupt sense
0 — SYS < SYS_OVLO
1 — SYS > SYS_OVLO

7 to 5

—

—

Unused

UNUSED

Description

Table 111. Register COINCELL_CONTROL - ADDR 0x30
Name

Bit

R/W

Default

Description

VCOIN

3 to 0

RW

0000

Coin cell charger charging voltage
0000 — 1.8 V
0111 — 3.3 V (goes up in 100 mV step per LSB)

COINCHEN

4

RW

0

Coin cell charger enable
0 — Charger disabled
1 — Charger enabled

UNUSED

7 to 5

—

—

Unused

Table 112. Register SW1_VOLT - ADDR 0x32
Name

Bit

R/W

Default
[1]

Description

SW1_VOLT

5 to 0

RW1S

—

SW1 voltage setting register (Run mode)
000000 — See Table 31 for voltage settings
111111 — See Table 31 for voltage settings
Reset condition — POR

UNUSED

7 to 5

—

—

Unused

[1]

Load from OTP fuse, Read, and Write

Table 113. Register SW1_STBY_VOLT - ADDR 0x33
Name

Bit

R/W

Default
[1]

Description

SW1_STBY_VOLT

5 to 0

RW1S

—

SW1 output voltage setting register (Standby mode). The default
value here should be identical to SW1_VOLT[5:0] register.
000000 — See Table 31 for voltage settings
111111 — See Table 31 for voltage settings

UNUSED

7 to 6

—

—

Unused

[1]

Load from OTP fuse, Read, and Write

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

91 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 114. Register SW1_SLP_VOLT - ADDR 0x34
Name

Bit

R/W

SW1_SLP_VOLT

5 to 0

RW1S

—

SW1 output voltage setting register (Sleep mode). The default
value here should be identical to SW1_VOLT[5:0] register.
000000 — See Table 31 for voltage settings
111111 — See Table 31 for voltage settings

UNUSED

7 to 6

—

—

Unused

[1]

Default
[1]

Description

Load from OTP fuse, Read, and Write

Table 115. Register SW1_CTRL - ADDR 0x35
Name

Bit

R/W

Default
[1]

Description

SW1_EN

0

RW1S

0

Enables buck regulator. Loaded from OTP based on the
sequence settings. User can turn off regulator by clearing this bit.
0 — Regulator disabled in Run mode
1 — Regulator enabled in Run mode

SW1_STBY_EN

1

RW1S

0

Enables buck regulator in Standby mode. User can turn off
regulator by clearing this bit. The default value of this bit should
be equal to the SW1_EN bit (based on OTP).
0 — Regulator disabled in Standby mode
1 — Regulator enabled in Standby mode

SW1_OMODE

2

RW

[2]

0

Enables buck regulator in Sleep mode. User can turn off regulator
by clearing this bit.
0 — Regulator disabled in Sleep mode
1 — Regulator enabled in Sleep mode

SW1_LPWR

3

RW

0

Enables the buck to enter Low-power mode during Standby and
Sleep
0 — Regulator not in Low-power mode
1 — Regulator in Low-power mode during Standby or Sleep
modes

SW1_DVSSPEED

4

RW1S

0

Controls slew rate of DVS transitions. Loaded from OTP and
changeable by user after boot up. Not used when OTP_SW1_
DVS_SEL = 1.
0 — DVS rate at 12.5 mV/2 μs
1 — DVS rate at 12.5 mV/4 μs

SW1_FPWM_IN_DVS

5

RW

0

Enables CCM operation during DVS down
0 — does not force FPWM during DVS
1 — forces regulator to track the DVS reference while it is falling
rather than relying on the load current to pull the voltage low

SW1_FPWM

6

RW

0

Forces buck to go into CCM mode
0 — Not in FPWM mode
1 — Forced in PWM mode irrespective of load current

SW1_RDIS_ENB

7

RW1S

0

Controls discharge resistor on output when regulator disabled
0 — Enables discharge resistor on output when regulator
disabled. Resistor connected at FB pin when regulator disabled to
force capacitor discharge.
1 — Disables discharge resistor on output when regulator
disabled. Resistor not connected at FB pin when regulator
disabled. Relies on leakage/residue load to discharge output
capacitor.

[1]
[2]

Load from OTP fuse, Read, and Write
Asynchronous Set, Read, and Write

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

92 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 116. Register SW1_SLP_VOLT - ADDR 0x36
Name

Bit

R/W

SW1_ILIM

1 to 0

RW1S

00

Sets current limit of SW1 regulator
00 — Typical current limit of 1.0 A
01 — Typical current limit of 1.2 A
10 — Typical current limit of 1.5 A
11 — Typical current limit of 2.0 A

UNUSED

3 to 2

—

—

Unused

4

RW

0

0 — TON control
1 — TOFF control

7 to 5

—

—

Unused

SW1_TMODE_SEL
UNUSED
[1]

Default
[1]

Description

Load from OTP fuse, Read, and Write

Table 117. Register SW2_VOLT - ADDR 0x38
Name

Bit

R/W

SW2_VOLT

5 to 0

RW1S

—

SW2 voltage setting register (Run mode)
000000 —See Table 31 for voltage settings
111111 — See Table 31 for voltage settings

UNUSED

7 to 6

—

—

Unused

[1]

Default
[1]

Description

Load from OTP fuse, Read, and Write

Table 118. Register SW2_STBY_VOLT - ADDR 0x39
Name

Bit

R/W

Default
[1]

Description

SW2_STBY_VOLT

5 to 0

RW1S

—

SW2 output voltage setting register (Standby mode). The default
value here should be identical to SW2_VOLT[5:0] register.
000000 — See Table 31 for voltage settings
111111 — See Table 31 for voltage settings

UNUSED

7 to 6

—

—

Unused

[1]

Load from OTP fuse, Read, and Write

Table 119. Register SW2_SLP_VOLT - ADDR 0x3A
Name

Bit

R/W

SW2_SLP_VOLT

5 to 0

RW1S

—

SW2 output voltage setting register (Sleep mode). The default
value here should be identical to SW2_VOLT[5:0] register.
000000 — See Table 31 for voltage settings
111111 — See Table 31 for voltage settings

UNUSED

7 to 6

—

—

Unused

[1]

Default
[1]

Description

Load from OTP fuse, Read, and Write

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

93 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 120. Register SW2_CTRL - ADDR 0x3B
Name

Bit

R/W

SW2_EN

0

RW1S

[1]

0

Enables buck regulator. Loaded from OTP based on the
sequence settings. User can turn off regulator by clearing this bit.
0 — Regulator disabled in Run mode
1 — Regulator enabled in Run mode

SW2_STBY_EN

1

RW1S

0

Enables buck regulator in Standby mode. User can turn off
regulator by clearing this bit. The default value of this bit should
be equal to the SW1_EN bit (based on OTP).
0 — Regulator disabled in Standby mode
1 — Regulator enabled in Standby mode

SW2_OMODE

2

RW

0

Enables buck regulator in Sleep mode. User can turn off regulator
by clearing this bit.
0 — Regulator disabled in Sleep mode
1 — Regulator enabled in Sleep mode

SW2_LPWR

3

RW

0

Enables the buck to enter Low-power mode during Standby and
Sleep modes
0 — Regulator not in Low-power mode
1 — Regulator in Low-power mode during Standby or Sleep

SW2_DVSSPEED

4

RW1S

0

Controls slew rate of DVS transitions. Loaded from OTP and
changeable by user after boot up. Not used when OTP_SW2_
DVS_SEL = 1.
0 — DVS rate at 12.5 mV/2 μs
1 — DVS rate at 12.5 mV/4 μs

SW2_FPWM_IN_DVS

5

RW

0

Enables CCM operation during DVS down
0 — does not force FPWM during DVS
1 — forces regulator to track the DVS reference while it is falling
rather than relying on the load current to pull the voltage low

SW2_FPWM

6

RW

0

Forces buck to go into CCM mode
0 — Not in FPWM mode
1 — Forced in PWM mode irrespective of load current.

SW2_RDIS_ENB

7

RW1S

0

Controls discharge resistor on output when regulator disabled
0 — Enables discharge resistor on output when regulator
disabled. Resistor connected at FB pin when regulator disabled to
force capacitor discharge.
1 — Disables discharge resistor on output when regulator
disabled. Resistor not connected at FB pin when regulator
disabled. Relies on leakage/residue load to discharge output
capacitor.

[1]

Default

Description

Load from OTP fuse, Read, and Write

Table 121. Register SW2_CTRL1 - ADDR 0x3C
Name

Bit

R/W

Default

SW2_ILIM

1 to 0

RW1S

00

Sets current limit of SW2 regulator
00 — Typical current limit of 1.0 A
01 — Typical current limit of 1.2 A
10 — Typical current limit of 1.5 A
11 — Typical current limit of 2.0 A

UNUSED

3 to 2

—

—

Unused

4

RW

0

0 — TON control
1 — TOFF control

7 to 5

—

—

Unused

SW2_TMODE_SEL
UNUSED
PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

94 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 122. Register SW3_VOLT - ADDR 0x3E
Name

Bit

R/W

Default

SW3_VOLT

3 to 0

RW1S

—

Description
SW3 voltage setting register (Run mode). Loaded from fuses.
Read only because DVS is not supported in this regulator.
0000 — See Table 37 for voltage settings
1111 — See Table 37 for voltage settings

UNUSED

7 to 4

—

—

Unused

Table 123. Register SW3_STBY_VOLT - ADDR 0x3F
Name

Bit

R/W

Default

Description

SW3_STBY_VOLT

3 to 0

RW1S

—

SW3 voltage setting register (Standby mode). Loaded from fuses.
Read only because DVS is not supported in this regulator.
0000 — See Table 37 for voltage settings
1111 —See Table 37 for voltage settings

UNUSED

7 to 4

—

—

Unused

Table 124. Register SW3_SLP_VOLT - ADDR 0x40
Name

Bit

R/W

Default

Description

SW3_SLP_VOLT

3 to 0

RW1S

—

SW3 voltage setting register (Sleep mode). Loaded from fuses.
Read only because DVS is not supported in this regulator.
0000 — See Table 37 for voltage settings
1111 — See Table 37 for voltage settings

UNUSED

7 to 4

—

—

Unused

Table 125. Register SW3_CTRL - ADDR 0x41
Name

Bit

R/W

Default

SW3_EN

0

RW1S

0

Enables buck regulator. Loaded from OTP based on the
sequence settings. User can turn off regulator by clearing this bit.
0 — Regulator disabled in Run mode
1 — Regulator enabled in Run mode

SW3_STBY_EN

1

RW1S

0

Enables buck regulator in Standby mode. User can turn off
regulator by clearing this bit. The default value of this bit should
be equal to the SW1_EN bit (based on OTP).
0 — Regulator disabled in Standby mode
1 — Regulator enabled in Standby mode

SW3_OMODE

2

RW

0

Enables buck regulator in Sleep mode. User can turn off regulator
by clearing this bit.
0 — Regulator disabled in Sleep mode
1 — Regulator enabled in Sleep mode

SW3_LPWR

3

RW

0

Enables the buck to enter Low-power mode during Standby and
Sleep modes
0 — Regulator not in Low-power mode
1 — Regulator in Low-power mode while in Standby or Sleep

UNUSED

4

—

—

Unused

UNUSED

5

—

—

Unused

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

95 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 125. Register SW3_CTRL - ADDR 0x41...continued
Name

Bit

R/W

Default

SW3_FPWM

6

RW

0

Description
Forces buck to go into CCM mode
0 — Not in FPWM mode
1 — Forced in PWM mode irrespective of load current

SW3_RDIS_ENB

7

RW1S

0

Controls discharge resistor on output when regulator disabled
0 — Enables discharge resistor on output when regulator
disabled. Resistor connected at FB pin when regulator disabled to
force capacitor discharge.
1 — Disables discharge resistor on output when regulator
disabled. Resistor not connected at FB pin when regulator
disabled. Relies on leakage/residue load to discharge output
capacitor.

Table 126. Register SW3_CTRL1 - ADDR 0x42
Name

Bit

R/W

Default

SW3_ILIM

1 to 0

RW1S

00

Sets current limit of SW3 regulator
00 — Typical current limit of 1.0 A
01 — Typical current limit of 1.2 A
10 — Typical current limit of 1.5 A
11 — Typical current limit of 2.0 A

UNUSED

3 to 2

—

—

Unused

4

RW

0

0 — TON control
1 — TOFF control

7 to 5

—

—

Unused

SW3_TMODE_SEL
UNUSED

Description

Table 127. Register VSNVS_CTRL - ADDR 0x48
Name

Bit

R/W

Default

Description

VSNVS_VOLT

2 to 0

RW1S

000

Not used in PF1550. Placeholder for future products.

CLKPULSE

3

RW

0

Optional bit used for evaluation (see IP block)

FORCEBOS

4

RW

0

Optional bit for evaluation
0 — BOS circuit activated only when VSYS < UVDET
1 — Forces best of supply circuit irrespective of UVDET

LIBGDIS

5

RW

0

Use to reduce quiescent current in coin cell mode
0 — VSNVS local band gap enabled in coin cell mode
1 — VSNVS local band gap disabled in coin cell mode to save
quiescent current

UNUSED

7 to 6

—

—

Unused

Table 128. Register VREFDDR_CTRL - ADDR 0x4A
Name

Bit

R/W

Default

VREFDDR_EN

0

RW1S

0

PF1550

Product data sheet

Description
0 — Disables VREFDDR regulator
1 — Enables VREFDDR regulator. This is set by the OTP
sequence.

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

96 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 128. Register VREFDDR_CTRL - ADDR 0x4A...continued
Name

Bit

R/W

Default

VREFDDR_STBY_EN

1

RW1S

0

The default value for this should be same as VREFDDREN
0 — Disables VREFDDR regulator in Standby mode
1 — Enables VREFDDR regulator in Standby mode if
VREFDDREN = 1

VREFDDR_OMODE

2

RW

0

0 — Keeps VREFDDR off in Off mode
1 — Enables VREFDDR in Sleep mode if VREFDDREN = 1

VREFDDR_LPWR

3

RW

0

0 — Disables VREFDDR Low-power mode
1 — Enables VREFDDR Low-power mode

7 to 4

—

—

Unused

UNUSED

Description

Table 129. Register LDO1_VOLT - ADDR 0x4C
Name

Bit

R/W

Default

LDO1_VOLT

4 to 0

RW1S

—

Description
LDO1 output voltage setting register. Loaded from OTP.
00000 — See Table 41 for voltage settings
11111 — See Table 41 for voltage settings

UNUSED

7 to 5

—

—

Unused

Table 130. Register LDO1_CTRL - ADDR 0x4D
Name

Bit

R/W

Default

VLDO1_EN

0

RW1S

0

Enables LDO regulator. Loaded from OTP based on the
sequence settings. User can turn off regulator by clearing this bit.
0 — Disables regulator
1 — Enables regulator

VLDO1_STBY_EN

1

RW1S

0

Enables LDO in Standby mode. Default value of this bit should be
same as VLDO1_EN.
0 — Disables regulator
1 — Enables regulator

VLDO1_OMODE

2

RW

0

Enables LDO in Sleep mode
0 — Disables regulator
1 — Enables regulator

VLDO1_LPWR

3

RW

0

Forces LDO to Low-power mode in Sleep and Standby modes
0 — Not in Low-power mode during Standby and Sleep
1 — Regulator in Low-power mode during Standby and Sleep

LDO1_LS_EN

4

RW1S

0

This is loaded from OTP_LDOy_LS_EN and changeable from 0
to 1 on power-up. Changing from 1 to 0 is not allowed.
0 — Sets LDOy in LDO mode
1 — Sets LDOy to a load switch (fully on) mode

7 to 5

—

—

Unused

UNUSED

Description

Table 131. Register LDO2_VOLT - ADDR 0x4F
Name

Bit

R/W

Default

LDO2_VOLT

3 to 0

RW1S

—

PF1550

Product data sheet

Description
LDO2 output voltage setting register. Loaded from OTP.
0000 — See Table 43 for voltage settings
1111 — See Table 43 for voltage settings

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

97 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 131. Register LDO2_VOLT - ADDR 0x4F...continued
Name

Bit

R/W

Default

UNUSED

7 to 4

—

—

Description
Unused

Table 132. Register LDO2_CTRL - ADDR 0x50
Name

Bit

R/W

Default

VLDO2_EN

0

RW1S

0

Enables LDO regulator. Loaded from OTP based on the
sequence settings. User can turn off regulator by clearing this bit.
0 — Disables regulator
1 — Enables regulator

VLDO2_STBY_EN

1

RW1S

0

Enables LDO in Standby mode. Default value of this bit should be
same as VLDO1_EN.
0 — Disables regulator
1 — Enables regulator

VLDO2_OMODE

2

RW

0

Enables LDO in Sleep mode
0 — Disables regulator
1 — Enables regulator

VLDO2_LPWR

3

RW

0

Forces LDO to Low-power mode in Sleep and Standby modes
0 — Not in Low-power mode during Standby and Sleep
1 — Regulator in Low-power mode during Standby and Sleep

7 to 4

—

—

Unused

UNUSED

Description

Table 133. Register LDO3_VOLT - ADDR 0x52
Name

Bit

R/W

Default

Description

LDO3_VOLT

4 to 0

RW1S

—

LDO3 output voltage setting register. Loaded from OTP.
00000 — See Table 41 for voltage settings
11111 — See Table 41 for voltage settings

UNUSED

7 to 5

—

—

Unused

Table 134. Register LDO3_CTRL - ADDR 0x53
Name

Bit

R/W

Default

VLDO3_EN

0

RW1S

0

Enables LDO regulator. Loaded from OTP based on the
sequence settings. User can turn off regulator by clearing this bit.
0 — Disables regulator
1 — Enables regulator

VLDO3_STBY_EN

1

RW1S

0

Enables LDO in Standby mode. Default value of this bit should be
same as VLDO1_EN.
0 — Disables regulator
1 — Enables regulator

VLDO3_OMODE

2

RW

0

Enables LDO in Sleep mode
0 — Disables regulator
1 — Enables regulator

VLDO3_LPWR

3

RW

0

Forces LDO to Low-power mode in Sleep and Standby modes
0 — Not in Low-power mode during Standby and Sleep
1 — Regulator in Low-power mode during Standby and Sleep

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

98 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 134. Register LDO3_CTRL - ADDR 0x53...continued
Name

Bit

R/W

Default

LDO3_LS_EN

4

RW1S

0

This is loaded from OTP_LDOy_LS_EN and changeable from 0
to 1 on power up. Changing from 1 to 0 is not allowed.
0 — sets LDOy in LDO mode
1 — sets LDOy to a load switch (fully on) mode

7 to 5

—

—

Unused

UNUSED

Description

Table 135. Register PWRCTRL0 - ADDR 0x58
Name

Bit

R/W

Default

Description

STANDBYDLY

1 to 0

RW

01

Controls delay of Standby pin after synchronization
0 — No additional delay
1 — 32 kHz cycle additional delay
2 — 32 kHz cycle additional delay
3 — 32 kHz cycle additional delay

STANDBYINV

2

RW

0

Controls polarity of STANDBY pin
0 — Standby pin input active high
1 — Standby pin input active low

POR_DLY

5 to 3

RW1S

000

Controls delay of RESETBMCU pin after power up (loaded from
OTP)
000 — RESETBMCU goes high 2 ms after last regulator
010 — RESETBMCU goes high 4 ms after last regulator
011 — RESETBMCU goes high 8 ms after last regulator
100 — RESETBMCU goes high 16 ms after last regulator
101 — RESETBMCU goes high 128 ms after last regulator
110 — RESETBMCU goes high 256 ms after last regulator
111 — RESETBMCU goes high 1024 ms after last regulator

TGRESET

7 to 6

RW1S

00

Controls duration for which ONKEY has to be pushed low for a
global reset (part goes to REGS_DISABLE)
00 — 4 s
01 — 8 s
10 — 12 s
11 — 16 s

Table 136. Register PWRCTRL1 - ADDR 0x59
Name

Bit

R/W

Default

PWRONDBNC

1 to 0

RW

00

Controls debounce of PWRON when in push-button mode
(PWRON_CFG = 1)
00 — 31.25 ms falling edge; 31.25 ms rising edge
01 — 31.25 ms falling edge; 31.25 ms rising edge
10 — 125 ms falling edge; 31.25 ms rising edge
11 — 750 ms falling edge; 31.25 ms rising edge

ONKEYDBNC

3 to 2

RW

00

Controls debounce of ONKEY push-button
00 — 31.25 ms falling edge; 31.25 ms rising edge
01 — 31.25 ms falling edge; 31.25 ms rising edge
10 — 125 ms falling edge; 31.25 ms rising edge
11 — 750 ms falling edge; 31.25 ms rising edge

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

99 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 136. Register PWRCTRL1 - ADDR 0x59...continued
Name

Bit

R/W

Default

PWRONRSTEN

4

RW

0

Description
Enables going to REGS_DISABLE or Sleep mode when
PWRON_CFG = 1. See Section 10 "PF1550 state machine" for
details.
0 — Long press on PWRON button does not take state to
REGS_DISABLE or Sleep
1 — Long press on PWRON button takes state to
REGS_DISABLE or Sleep

RESTARTEN

5

RW

0

Enables restart of system when PWRON push-button is held low
for 5 s
0 — No impact
1 — When going to REGS_DISABLE via a long press of PWRON
button, holding it low for 1 more second takes state back to RUN
(Equally, a 5 second push restarts the system)

REGSCPEN

6

RW

0

Shuts down LDO if it enters a current limit fault. Controls LDO1,
LDO2, and LDO3.
0 — LDO does not shut down in the event of a current limit fault.
Continues to current limit
1 — LDO is turned off when it encounters a current limit fault

ONKEY_RST_EN

7

RW

1

Enables turning off of system via ONKEY. See Section 10
"PF1550 state machine" for details.
0 — ONKEY cannot be used to turn off or restart system
1 — ONKEY can be used to turn off or restart system

Table 137. Register PWRCTRL2 - ADDR 0x5A
Name

Bit

R/W

Default

Description

UVDET

1 to 0

RW1S

00

Sets UVDET threshold
00 — Rising 2.65 V; Falling 2.55 V
01 — Rising 2.8 V; Falling 2.7 V
10 — Rising 3.0 V; Falling 2.9 V
11 — Rising 3.1 V; Falling 3.0 V

LOW_SYS_WARN

3 to 2

RW

00

Sets LOW_SYS_WARN threshold
00 — Rising 3.3 V; Falling 3.1 V
01 — Rising 3.5 V; Falling 3.3 V
10 — Rising 3.7 V; Falling 3.5 V
11 — Rising 3.9 V; Falling 3.7 V

UNUSED

7 to 4

—

—

Unused

Table 138. Register PWRCTRL3 - ADDR 0x5B
Name

Bit

R/W

Default

GOTO_SHIP

0

RW

0

Set this bit to go to SHIP mode from any state. See Section 10
"PF1550 state machine" for details.
0 — No impact
1 — PF1550 enters SHIP mode

GOTO_CORE_OFF

1

RW

0

Set this bit to go to CORE_OFF mode once in REGS_DISABLE
state
0 — No impact
1 — PF1550 enters CORE_OFF mode when in REGS_DISABLE
state

7 to 2

—

—

Unused

UNUSED
PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

100 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 139. Register SW1_PWRDN_SEQ - ADDR 0x5F
Name

Bit

R/W

Default

SW1_PWRDN_SEQ

2 to 0

RW1S

000

Description
This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the function opposite
to the power-up sequencer. Each regulator has an associated
register setting (SW1_PWRDN_SEQ[2:0], SW2_PWRDN_
SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_SEQ[2:0],
LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

Table 140. Register SW2_PWRDN_SEQ - ADDR 0x60
Name

Bit

R/W

Default

SW2_PWRDN_SEQ

2 to 0

RW1S

000

This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the function opposite
to the power-up sequencer. Each regulator has an associated
register setting (SW1_PWRDN_SEQ[2:0], SW2_PWRDN_
SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_SEQ[2:0],
LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

101 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 141. Register SW2_PWRDN_SEQ - ADDR 0x61
Name

Bit

R/W

Default

SW3_PWRDN_SEQ

2 to 0

RW1S

000

Description
This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the function opposite
to the power-up sequencer. Each regulator has an associated
register setting (SW1_PWRDN_SEQ[2:0], SW2_PWRDN_
SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_SEQ[2:0],
LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

Table 142. Register LDO1_PWRDN_SEQ - ADDR 0x62
Name

Bit

R/W

Default

LDO1_PWRDN_SEQ

2 to 0

RW1S

000

This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the function opposite
to the power-up sequencer. Each regulator has an associated
register setting (SW1_PWRDN_SEQ[2:0], SW2_PWRDN_
SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_SEQ[2:0],
LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

102 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 143. Register LDO2_PWRDN_SEQ - ADDR 0x63
Name

Bit

R/W

Default

LDO2_PWRDN_SEQ

2 to 0

RW1S

000

Description
This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the functional
opposite to the power-up sequencer. Each regulator has an
associated register setting (SW1_PWRDN_SEQ[2:0], SW2_
PWRDN_SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_
SEQ[2:0], LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

Table 144. Register LDO3_PWRDN_SEQ - ADDR 0x64
Name

Bit

R/W

Default

LDO3_PWRDN_SEQ

2 to 0

RW1S

000

This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the function opposite
to the power-up sequencer. Each regulator has an associated
register setting (SW1_PWRDN_SEQ[2:0], SW2_PWRDN_
SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_SEQ[2:0],
LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

103 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 145. Register VREFDDR_PWRDN_SEQ - ADDR 0x65
Name

Bit

R/W

Default

VREFDDR_PWRDN_S
EQ

2 to 0

RW1S

000

Description
This contains same value as power-up sequence value by
default. Power-up sequence is in mirror registers.
xxx = The power-down sequencer performs the function opposite
to the power-up sequencer. Each regulator has an associated
register setting (SW1_PWRDN_SEQ[2:0], SW2_PWRDN_
SEQ[2:0], SW3_PWRDN_SEQ[2:0], LDO1_PWRDN_SEQ[2:0],
LDO2_PWRDN_SEQ[2:0], LDO3_PWRDN_SEQ[2:0].
VREFDDR_PWRDN_SEQ[2:0]) that sets its power-down
sequence. The default setting of the above registers is equal
to the corresponding power-up sequence setting. For example,
SW1_PWRDN_SEQ[2:0] = OTP_SW1_PWRUP_SEQ[2:0].
When the power-down sequencer is activated, regulators are
turned off one by one in the descending order of the XXX_
PWRDN_SEQ[2:0] setting. This way, by default, power-down
is a mirror of the power-up sequence. In one of the "System
On" states, the processor can change the values of the XXX_
PWRDN_SEQ[2:0] registers. The power-up sequence is fixed by
OTP (or TBB). If all XXX_PWRDN_SEQ[2:0] = 0x00, the powerdown sequencer is bypassed and all the regulators are turned off
at once.

UNUSED

7 to 3

—

—

Unused

Table 146. Register STATE_INFO - ADDR 0x67
Name

Bit

R/W

Default

Description

STATE

5 to 0

R

000000

Indicates machine state
000000 — Wait status
001100 — RUN state
001101 — STANDBY state
001110 — SLEEP/LPSR state
101011 — REGS_DISABLE state
Other bits are reserved

UNUSED

7 to 6

—

—

Unused

Table 147. Register I2C_ADDR - ADDR 0x68
Name

Bit

R/W

Default

I2C_SLAVE_ADDR_L
SBS

2 to 0

R

000

Loaded from fuses. But read only in functional space.
000 — Slave Address: 0x08
001 — Slave Address: 0x09
010 — Slave Address: 0x0A
011 — Slave Address: 0x0B
100 — Slave Address: 0x0C
101 — Slave Address: 0x0D
110 — Slave Address: 0x0E
111 — Slave Address: 0x0F

USE_DEFAULT_ADD R

7

RW

0

DEFAULT ADDR

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

104 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 148. Register RC_16MHZ - ADDR 0x6B
Name

Bit

R/W

Default

REQ_16MHZ

0

RW

0

Enables 16 MHz clock
0 — 16 MHz clock enable controlled by state machine
1 — 16 MHz clock always enabled

REQ_ACORE_ON

1

RW

0

Controls Analog core enable
0 — Analog core enable controlled by state machine
1 — Analog core always on

REQ_ACORE_HIPWR

2

RW

0

Controls Low-power mode of the analog core
0 — Analog core Low-power mode controlled by state machine
1 — Analog core never in Low-power mode

7 to 3

—

—

Unused

UNUSED

Description

Table 149. Register KEY1 - ADDR 0x6B
Name

Bit

R/W

Default

Description

KEY1

7 to 0

RW

0x00

Unused

12.2 Specific Charger Registers (Offset is 0x80)
Table 150. Register CHG_INT - ADDR 0x00
Name

Bit

R/W

Default
[1]

Description

SUP_I

0

RW1S

0

Supplement mode interrupt
0 — The SUP_OK bit interrupt has not occurred or been cleared
1 — The SUP_OK bit interrupt has occurred
Reset condition — VCOREDIG_RSTB

BAT2SOC_I

1

RW1S

0

VBATT to VSYS overcurrent interrupt
0 — The BAT2SOC_OK interrupt has not occurred or been
cleared
1— The BAT2SOC _OK bit interrupt has occurred
Reset condition — VCOREDIG_RSTB

BAT_I

2

RW1S

0

Battery interrupt
0 — The BAT_OK interrupt has not occurred or been cleared
1 — The BAT_OK interrupt has occurred
Reset condition — VCOREDIG_RSTB

CHG_I

3

RW1S

0

Charger interrupt
0 — The CHG_OK interrupt has not occurred or been cleared
1 — The CHG_OK interrupt has occurred
Reset condition — VCOREDIG_RSTB

RSVD4

4

RW1S

0

Unused

VBUS_I

5

RW1S

0

VBUS interrupt
0 — The VBUS_OK interrupt has not occurred or been cleared
1 — The VBUS_OK interrupt has occurred
Reset condition — VCOREDIG_RSTB

VBUS_DPM_I

6

RW1S

0

VBUS_DPM interrupt
0 — The VBUS_DPM _OK interrupt has not occurred or been
cleared
1 — The VBUS_DPM _OK interrupt has occurred
Reset condition — VCOREDIG_RSTB

PF1550

Product data sheet

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

105 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 150. Register CHG_INT - ADDR 0x00...continued
Name

Bit

R/W

Default

THM_I

7

RW1S

0

[1]

Description
THM interrupt. Occurs when Warm/Cool thresholds are crossed
or when thermal foldback is active. After the interrupt has
occurred, THM_OK bit can be read to know the source of the
interrupt.
If THM_OK = 0, warm/cool thresholds are crossed
If THM_OK = 1, thermal foldback is active
0 — THM interrupt has not occurred or has been cleared
1 — THM interrupt has occurred
Reset condition — VCOREDIG_RSTB

Load from OTP fuse, Read, and Write

Table 151. Register CHG_INT_MASK - ADDR 0x02
Name

Bit

R/W

Default

SUP_M

0

RW

1

Supplement mode interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

BAT2SOC_M

1

RW

1

VBATT to VSYS overcurrent interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

BAT_M

2

RW

1

Battery interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

CHG_M

3

RW

1

Charger interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

RSVD4

4

RW

1

Unused

VBUS_M

5

RW

1

VBUS interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

VBUS_DPM_M

6

RW

1

VBUS_DPM interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

THM_M

7

RW

1

THM interrupt mask
0 — Unmasked
1 — Masked
Reset condition — VCOREDIG_RSTB

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

106 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 152. Register CHG_INT_OK - ADDR 0x04
Name

Bit

R/W

Default

SUP_OK

0

R

0

Description
Supplement mode indicator
0 — Supplement mode not detected
1 — Supplement mode detected
Reset condition — VCOREDIG_RSTB

BAT2SOC_OK

1

R

0

Single-bit battery overcurrent indicator
0 — Battery to VSYS has not hit overcurrent limit
1 — Battery to VSYS has hit overcurrent limit (BATTOC_SNS bit
= 1)
Reset condition — VCOREDIG_RSTB

BAT_OK

2

R

1

Single-bit battery status indicator. See BATT_SNS for more
information.
0 — The battery has an issue or the charger has been
suspended, for example, BATT_SNS = 0x02 or 0x05 or 0x06
1 — The battery is okay, for example, BATT_SNS ≠ 0x02 or 0x05
or 0x06
Reset condition — VCOREDIG_RSTB

CHG_OK

3

R

0

Single-bit charger status indicator. See CHG_SNS for more
information.
Reset condition — VCOREDIG_RSTB
0 — The charger is not charging, has suspended charging or
Thermal Reg = 1, for example, CHG_SNS ≠ 0x00 or 0x01 or 0x02
or 0x03
1 — The charger is okay, for example, CHG_SNS = 0x00 or 0x01
or 0x02 or 0x03
Reset condition — VCOREDIG_RSTB

RSVD4

4

R

0

Unused

VBUS_OK

5

R

0

Single-bit VBUS_LIN input status indicator. See VBUS_LIN_SNS
for more information.
0 — The VBUS_LIN input is invalid. For example, VBUS_VALID =
0.
1 — The VBUS_LIN input is valid. For example, VBUS_VALID =
1.
Reset condition — VCOREDIG_RSTB

VBUS_DPM_OK

6

R

0

VBUS_DPM status indicator. This register provides status of input
Dynamic Power Management threshold.
0 — Not in VBUS_DPM mode
1 — VBUS_DPM mode
Reset condition — VCOREDIG_RSTB

THM_OK

7

R

1

Thermistor status indicator. This register provides information on
whether battery temperature is within or outside the thermistor
cool/warm thresholds.
0 — Thermistor outside cool and warm thresholds
1 — Thermistor between cool and warm thresholds
Reset condition — VCOREDIG_RSTB

Table 153. Register VBUS_SNS - ADDR 0x06
Name

Bit

R/W

Default

RSVD0

1 to 0

R

00

Unused

2

R

1

0 — VBUS_ LIN > VBUS_LIN_UVLO
1 — VBUS_ LIN < VBUS_LIN_UVLO or when VBUS is detached

VBUS_UVLO_SNS

PF1550

Product data sheet

Description

All information provided in this document is subject to legal disclaimers.

Rev. 7 — 29 September 2021

© NXP B.V. 2021. All rights reserved.

107 / 150

PF1550

NXP Semiconductors

Power management integrated circuit (PMIC) for low power application processors
Table 153. Register VBUS_SNS - ADDR 0x06...continued
Name

Bit

R/W

Default

VBUS_IN2SYS_SNS

3

R

1

Description
0 — VBUS_ LIN > VBATT + VIN2SYS
1 — VBUS_ LIN < VBATT + VIN2SYS

VBUS_OVLO_SNS

4

R

0

0 — VBUS_ LIN < VBUS_LIN_OVLO
1 — VBUS_ LIN > VBUS_LIN_OVLO

VBUS_VALID

5

R

0

0 — VBUS is not valid
1 — VBUS is valid, VBUS_LIN > VBUS_LIN_UVLO, VBUS_LIN >
VBATT + VIN2SYS, VBUS_LIN < VBUS_LIN_OVLO
Reset condition — VCOREDIG_RSTB

RSVD6

6

R

0

Unused

VBUS_DPM_SNS

7

R

0

VBUS_LIN DPM sense details
0 — VBUS _LIN DPM threshold has not been triggered
1 — VBUS_LIN DPM threshold has been triggered

Table 154. Register CHG_SNS - ADDR 0x07
Name

Bit

R/W

Default

Description

CHG_SNS

3 to 0

R

1000

Charger sense
0 — Charger is in precharge mode, CHG_OK = 1, VBATT <
VPRECHG.LB, TJ < TSHDN
1 — Charger is in fast-charge constant current mode, CHG_OK =
1, VBATT < VBATREG, TJ < TSHDN
2 — Charger is in fast-charge constant voltage mode, CHG_OK =
1, VBATT = VBATREG, TJ < TSHDN
3 — Charger is in end-of-charge mode, CHG_OK = 1, VBATT ≥
VBATREG, IBAT = IEOC, TJ < TSHDN
4 — Charger is in done mode, CHG_OK = 0, VBATT >
VBATREG-VRESTART, TJ < TSHDN
5 — Reserved
6 — Charger is in timer fault mode, CHG_OK = 0, VBATT <
VBATOV, if BATT_SNS = 0b001 then VBATT < VBATPC, TJ <
TSHDN
7 — Charger is in thermistor suspend mode, CHG_OK =
0, VBATT < VBATOV, if BATT_SNS = 0b001 then VBATT <
VPRECHG.LB, TJ < TSHDN
8 — Charger is off, input invalid and/or charger is disabled,
CHG_OK = 0
9 — Battery overvoltage condition
10 — Charger is off and TJ > TSHDN, CHG_OK = 0
11 — Reserved
12 — Charger block is in Linear only mode, not charging,
CHG_OK = 0
```
