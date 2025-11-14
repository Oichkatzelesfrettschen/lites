#!/usr/bin/env bash
# Generate a simple include dependency map in Graphviz format.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/include_graph.dot"

printf 'digraph includes {\n' > "$OUT"

find "$ROOT" \( -name '*.c' -o -name '*.h' \) -print0 | while IFS= read -r -d '' file; do
    rel="${file#$ROOT/}"
    while IFS= read -r line; do
        header=$(echo "$line" | sed -E 's/^\s*#include[ \t]*[<\"]([^>\"]+)[>\"].*/\1/' )
        [ -n "$header" ] && printf '"%s" -> "%s";\n' "$rel" "$header" >> "$OUT"
    done < <(grep -E '^\s*#include[ \t]*[<\"]([^>\"]+)[>\"]' "$file" || true)
done

printf '}\n' >> "$OUT"

echo "Graph written to $OUT"
