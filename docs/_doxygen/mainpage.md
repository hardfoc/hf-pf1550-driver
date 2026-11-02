# PF1550 Driver API reference {#mainpage}

The classes, members and registers of the HardFOC driver for the NXP PF1550 power management IC (three buck
converters, three LDOs, a Li-ion charger), generated from the headers.

Start with pf1550::PF1550, the driver, and pf1550::BusInterface, the interface your platform port implements. The
register map is pf1550::Register; the rail-setting codes are pf1550::SwVoltageCode and pf1550::LdoVoltageCode; the
diagnostic types are pf1550::DiagnosticSnapshot and pf1550::FaultFlags; the named register sequences are in
pf1550::profiles.

The guides, with examples that compile, are on the documentation site: <a href="../">PF1550 Driver
documentation</a>.
