#!/usr/bin/env bash
# Parse #include lines to produce a simple dependency map.
# Each line of output has the form "<source> -> <header>".

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT_FILE="${1:-$ROOT/include_graph.txt}"

> "$OUT_FILE"

find "$ROOT" -name '*.c' -o -name '*.h' 2>/dev/null | while read -r file; do
    rel="${file#$ROOT/}"
    while IFS= read -r line; do
        if [[ $line =~ ^[[:space:]]*#include[[:space:]]+[<\"]([^\">]+)[\">] ]]; then
            inc="${BASH_REMATCH[1]}"
            echo "$rel -> $inc" >> "$OUT_FILE"
        fi
    done < "$file"
done

echo "Include graph written to $OUT_FILE"
