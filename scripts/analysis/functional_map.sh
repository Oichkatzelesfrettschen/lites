#!/usr/bin/env bash
# Extract include directives and function prototypes from source files.
# Phase 2 helper script.
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
OUT_DIR="$ROOT/analysis"
mkdir -p "$OUT_DIR"

INCLUDES="$OUT_DIR/includes.txt"
PROTOS="$OUT_DIR/function_prototypes.txt"
: > "$INCLUDES"
: > "$PROTOS"

find "$ROOT" -type f \( -name '*.c' -o -name '*.h' \) -print0 \
  | while IFS= read -r -d '' file; do
    rel="${file#$ROOT/}"
    grep -nE '^[[:space:]]*#include[[:space:]]*[<"]' "$file" \
      | sed "s|^|$rel:|" >> "$INCLUDES" || true
    grep -nE '^[[:space:]]*[[:alnum:]_][[:alnum:]_[:space:]*]*\([^)]*\)[[:space:]]*;' "$file" \
      | sed "s|^|$rel:|" >> "$PROTOS" || true
  done

echo "Include list saved to $INCLUDES"
echo "Function prototypes saved to $PROTOS"
