#!/usr/bin/env bash
set -euo pipefail

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
done < <(find src-lites-1.1-2025/xkernel/util -type f -name '*.c' -not -path '*/gmake-3.66/*')

exit $found
