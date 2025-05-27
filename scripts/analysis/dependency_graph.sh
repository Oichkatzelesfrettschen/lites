#!/usr/bin/env bash
# Generate function definitions and call graph via ctags and cscope.
# Phase 3 helper script.
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
OUT_DIR="$ROOT/analysis"
mkdir -p "$OUT_DIR"

FILES="$OUT_DIR/cscope.files"
find "$ROOT" -type f \( -name '*.c' -o -name '*.h' \) > "$FILES"

ctags -R --fields=+n --extras=+q --c-kinds=fp -f "$OUT_DIR/tags"
ctags -R -x --c-kinds=f > "$OUT_DIR/function_definitions.txt"

cscope -bkq -i "$FILES" -f "$OUT_DIR/cscope.out"

: > "$OUT_DIR/function_calls.txt"
awk '{print $1}' "$OUT_DIR/function_definitions.txt" | sort -u | while read -r func; do
  echo "$func:" >> "$OUT_DIR/function_calls.txt"
  cscope -dL -1 "$func" >> "$OUT_DIR/function_calls.txt" 2>/dev/null || true
  echo >> "$OUT_DIR/function_calls.txt"
done

echo "CTags and cscope results stored in $OUT_DIR"
