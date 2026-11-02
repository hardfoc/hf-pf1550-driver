"""``<!-- generated: quickstart-code -->``: the Quickstart example, copied from the compiled snippet file.

The README cannot include files the way the site does, so this region carries a copy of the
``quickstart`` section of ``tests/docs/quickstart.cpp`` (built by the host CI). ``hardfoc-docs gen --check`` fails
when the copy and the compiled file differ.
"""

from __future__ import annotations

import re

from hardfoc_mkdocs_theme.paths import repo_root


def _section(path: str, name: str) -> str:
    text = (repo_root() / path).read_text()
    m = re.search(rf"--8<-- \[start:{name}\]\n(.*?)\n[^\n]*--8<-- \[end:{name}\]", text, re.S)
    if not m:
        raise SystemExit(f"[gen] no section {name!r} in {path}")
    return m.group(1)


def quickstart_code() -> str:
    return "```cpp\n" + _section("tests/docs/quickstart.cpp", "quickstart") + "\n```\n"


REGIONS = {"quickstart-code": quickstart_code}


def register_map() -> str:
    """``<!-- generated: register-map -->``: the ``pf1550::Register`` enum as a table, from the header."""
    text = (repo_root() / "inc/pf1550_registers.hpp").read_text()
    body = re.search(r"enum class Register : uint8_t \{(.*?)\n\};", text, re.S).group(1)
    entries = re.findall(r"^\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+),\s*///<\s*(.*)$", body, re.M)
    rows = ["| Address, `Register::` | Data sheet name and contents |", "| --- | --- |"]
    for name, addr, doc in sorted(entries, key=lambda e: int(e[1], 16)):
        doc = doc.replace("|", "/").replace("**", "").strip()
        rows.append(f"| `0x{int(addr, 16):02X}` `{name}` | {doc} |")
    src = "Source of this table: [`inc/pf1550_registers.hpp`](../inc/pf1550_registers.hpp)."
    return "\n".join(rows) + "\n\n" + src + "\n"


REGIONS["register-map"] = register_map
