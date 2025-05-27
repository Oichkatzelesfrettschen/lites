#!/usr/bin/env bash
# See LICENSE_MAP for license details
set -euo pipefail

OUT_FILE="LICENSE_MAP"
: > "$OUT_FILE"

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

find . -type f \( -name '*.c' -o -name '*.h' -o -name '*.S' -o -name '*.s' -o -name '*.asm' \) \
  | sort | while read -r file; do
  header=$(sed -n '1,20p' "$file")
  excerpt=$(echo "$header" | head -n 3 | tr '\n' ' ' | sed 's/[[:space:]]\+/ /g')
  origin=$(echo "$header" | grep -i "Copyright" | head -n 1 | sed 's/^\s*[#* ]*//')
  timestamp=$(git log -1 --format=%cI -- "$file" 2>/dev/null || echo "unknown")
  printf "%s\t%s\t%s\t%s\n" "$file" "$excerpt" "$origin" "$timestamp" >> "$OUT_FILE"
done
