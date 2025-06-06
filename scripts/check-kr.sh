#!/usr/bin/env bash
set -euo pipefail

if [ -z "${LITES_SRC_DIR:-}" ]; then
  echo "LITES_SRC_DIR must be set" >&2
  exit 1
fi
SRC_DIR="$LITES_SRC_DIR"

found=0
while IFS= read -r file; do
  while IFS=: read -r num line; do
    if [[ "$line" =~ \)$ ]] && [[ ! "$line" =~ ';' ]] && [[ ! "$line" =~ '{' ]]; then
      next=$(sed -n "$((num + 1))p" "$file")
      if echo "$next" | grep -qE '^[[:space:]]*[A-Za-z_].*;'; then
        echo "$file:$num:$line"
        found=1
      fi
    fi
  done < <(grep -nE '^[A-Za-z_].*\)\s*$' "$file")
done < <(find "$SRC_DIR/xkernel/util" -type f -name '*.c' -not -path '*/gmake-3.66/*' 2>/dev/null)

exit $found
