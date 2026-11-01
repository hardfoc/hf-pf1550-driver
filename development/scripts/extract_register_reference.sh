#!/usr/bin/env bash
# Extract Section 12 register map from local PF1550 PDF to tracked markdown.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PDF="${ROOT}/_local_reference/datasheet/PF1550.pdf"
RAW="${ROOT}/docs/datasheet/.PF1550-extract.txt"
OUT="${ROOT}/docs/datasheet/PF1550-i2c-register-reference.md"

if [[ ! -f "${PDF}" ]]; then
  echo "Missing ${PDF}. Run scripts/fetch_datasheet.sh first." >&2
  exit 1
fi

pdftotext "${PDF}" "${RAW}"
python3 "${ROOT}/scripts/extract_register_reference.py" "${RAW}" "${OUT}"
rm -f "${RAW}"
echo "Wrote ${OUT}"
