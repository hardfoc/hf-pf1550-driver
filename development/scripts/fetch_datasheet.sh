#!/usr/bin/env bash
# Download NXP PF1550 datasheet into _local_reference/ (gitignored).
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="${ROOT}/_local_reference/datasheet/PF1550.pdf"
URL="https://www.nxp.com/docs/en/data-sheet/PF1550.pdf"
mkdir -p "$(dirname "${OUT}")"
echo "Fetching ${URL}"
curl -fsSL -o "${OUT}" "${URL}"
ls -lh "${OUT}"
echo "Done. Run scripts/extract_register_reference.sh to refresh markdown extracts."
