"""The three layers of the driver: hardware-agnostic core, communication interface, platform port.

Reads: From top to bottom. Your application talks to one PF1550<Bus> object per PMIC. Its public operations
(listed from inc/pf1550.hpp) read and write the PF1550's registers: identification, named register sequences
(profiles), rail set points, strap pins, OTP, interrupts and diagnostics. The communication interface
(pf1550::BusInterface in inc/pf1550_i2c_interface.hpp) is a CRTP base: four methods are required, DelayUs() has a
default. The platform port is the only code you write. Below it is the chip.
"""

import re
from pathlib import Path

from hardfoc_mkdocs_theme.figkit import Fig

ROOT = Path(__file__).resolve().parents[3]
DRIVER = "inc/pf1550.hpp"
COMM = "inc/pf1550_i2c_interface.hpp"


def data():
    drv = (ROOT / DRIVER).read_text()
    public = drv.split("public:", 1)[1].split("private:", 1)[0]
    ops = list(dict.fromkeys(re.findall(r"^  bool (\w+)\(", public, re.M)))
    comm = (ROOT / COMM).read_text()
    # A method whose body dispatches to Derived is required; one with a body of its own has a default.
    methods = re.findall(r"^  (?:bool|void) (\w+)\([^{;]*?\) noexcept \{\n(.*?)^  \}", comm, re.M | re.S)
    required = [n for n, body in methods if "static_cast<Derived*>" in body]
    optional = [n for n, body in methods if "static_cast<Derived*>" not in body]
    return {
        "operations": ops,
        "required": required,
        "optional": optional,
        "sources": [f"{DRIVER}:PF1550 public members", f"{COMM}:BusInterface"],
    }


def build(d):
    f = Fig(
        "driver-layers",
        "Three layers, one of them yours",
        alt="Layer diagram: the application calls the hardware-agnostic PF1550 driver, whose operations read and "
        "write the PMIC's registers, which uses the CRTP bus interface, which calls the platform port you write, "
        "which drives the I2C bus and the STANDBY and USB strap pins of the PF1550.",
        width=1200,
        kicker="Architecture",
        subtitle="The driver and the interface are the same on every MCU. The port is a small class with four "
        "required methods.",
        profile="overview",
        legend=[("brand", "this library"), ("teal", "you write"), ("slate", "hardware")],
    )
    W = f.body_w
    app = f.card(0, 0, W, "Your application", ["boot sequence, power monitor task, provisioning jig ..."],
                 tone="slate")
    top = app.b + 40
    chips_h = f.chips(16, f.region_inner_top(top), W - 32, d["operations"], tone="brand")
    reg = f.region(0, top, W, f.region_inner_top(top) - top + chips_h + 18, "Hardware-agnostic core", tone="brand",
                   sub="PF1550<Bus>, inc/pf1550.hpp")
    comm = f.card(0, reg.b + 40, W, "Communication interface",
                  ["pf1550::BusInterface<Self>: CRTP, no virtual calls; one-byte register reads and writes,",
                   "strap pins by name (STANDBY, USB_VBUS_EN, USB_OTG_EN) and the profile delay hook"],
                  tone="brand", accent=True)
    port = f.card(0, comm.b + 40, W, "Platform port (your class)",
                  ["required:  " + "  ".join(n + "()" for n in d["required"]),
                   "optional:  " + "  ".join(n + "()" for n in d["optional"])],
                  tone="teal", accent=True, mono=True)
    hw = f.card(0, port.b + 40, W, "PF1550",
                ["I2C up to 400 kHz, address 0x08  /  STANDBY in, INTB and RESETBMCU open drain"], tone="slate")
    for a, b in ((app, reg), (reg, comm), (comm, port), (port, hw)):
        f.connect(a, b, "bottom", "top", tone="rail")
    return f
