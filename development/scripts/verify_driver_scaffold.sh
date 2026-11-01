#!/usr/bin/env bash
# Software-only scaffold verification for hf-* external drivers.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT}"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
FAIL=0

check() {
  if [[ -e "$1" ]]; then
    echo -e "${GREEN}OK${NC}  $1"
  else
    echo -e "${RED}MISSING${NC}  $1"
    FAIL=1
  fi
}

echo "=== hf-pf1550-driver scaffold verification ==="

# Core layout
for f in \
  CMakeLists.txt \
  LICENSE \
  README.md \
  .gitmodules \
  cmake/hf_pf1550_build_settings.cmake \
  cmake/hf_pf1550Config.cmake.in \
  inc/pf1550.hpp \
  inc/pf1550_i2c_interface.hpp \
  inc/pf1550_registers.hpp \
  inc/pf1550_profiles.hpp \
  inc/pf1550_version.h.in \
  src/pf1550.ipp \
  examples/esp32/app_config.yml \
  examples/esp32/CMakeLists.txt \
  .github/workflows/esp32-examples-build-ci.yml \
  _config/.clang-format \
  _config/lychee.toml \
  docs/index.md \
  docs/datasheet/README.md \
  docs/datasheet/PF1550-register-map.md \
  docs/datasheet/PF1550-i2c-register-reference.md \
  scripts/fetch_datasheet.sh \
  scripts/extract_register_reference.sh \
  scripts/verify_driver_scaffold.sh
do
  check "${f}"
done

echo "--- CMake configure ---"
BUILD="${ROOT}/build-verify"
rm -rf "${BUILD}"
mkdir -p "${BUILD}"
if cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=Debug >/dev/null; then
  echo -e "${GREEN}OK${NC}  cmake configure"
  test -f "${BUILD}/hf_pf1550_generated/pf1550_version.h" || { echo -e "${RED}FAIL${NC} version header"; FAIL=1; }
else
  echo -e "${RED}FAIL${NC}  cmake configure"
  FAIL=1
fi

echo "--- Profile array sizes ---"
python3 - <<'PY'
import re, sys
from pathlib import Path
text = Path("inc/pf1550_profiles.hpp").read_text()
fail = False
for name in text.split("inline constexpr std::array<RegisterWrite,"):
    if "kPortentaH7" not in name:
        continue
    block = "inline constexpr std::array<RegisterWrite," + name.split("};", 1)[0]
    declared = re.search(r"RegisterWrite,\s*(\d+)>", block)
    var = re.search(r"kPortentaH7\w+", block)
    entries = len(re.findall(r"\{0x[0-9A-Fa-f]+,", block))
    if not declared or int(declared.group(1)) != entries:
        print(f"FAIL {var.group(0) if var else '?'}: declared={declared.group(1) if declared else '?'} entries={entries}")
        fail = True
    else:
        print(f"OK  {var.group(0) if var else '?'} entries={entries}")
if fail:
    sys.exit(1)
PY

if [[ -f "_local_reference/datasheet/PF1550.pdf" ]]; then
  echo -e "${GREEN}OK${NC}  local PDF present"
else
  echo "WARN  run ./scripts/fetch_datasheet.sh for local PDF (optional)"
fi

if [[ ${FAIL} -eq 0 ]]; then
  echo -e "\n${GREEN}All software checks passed.${NC}"
  echo "Hardware integration tests: human operator (rails, USB, scope)."
  exit 0
fi
echo -e "\n${RED}Scaffold verification failed.${NC}"
exit 1
