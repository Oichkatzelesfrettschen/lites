#!/usr/bin/env bash
# See LICENSE_MAP for license details
set -euo pipefail

OUT_FILE="LICENSE_MAP"
: > "$OUT_FILE"

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

find . -type f \( -name '*.c' -o -name '*.h' -o -name '*.S' -o -name '*.s' -o -name '*.asm' \) | sort | while read -r file; do
  header=$(sed -n '1,20p' "$file")
  license="UNKNOWN"
  if echo "$header" | grep -qi "GNU GENERAL PUBLIC LICENSE"; then
    license="GPL"
  elif echo "$header" | grep -qi "Carnegie Mellon"; then
    license="CMU"
  elif echo "$header" | grep -qi "University of California"; then
    license="BSD"
  elif echo "$header" | grep -qi "University of Utah"; then
    license="UTAH"
  elif echo "$header" | grep -qi "IBM"; then
    license="IBM"
  fi

  origin=$(echo "$header" | grep -i "Copyright" | head -n 1 | sed 's/^\s*[#* ]*//')
  timestamp=$(git log -1 --format=%cI -- "$file" 2>/dev/null || echo "unknown")
  printf "%s\t%s\t%s\t%s\n" "$file" "$license" "$origin" "$timestamp" >> "$OUT_FILE"
done
