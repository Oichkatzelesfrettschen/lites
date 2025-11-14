#!/usr/bin/env bash
# Gather files from exokernel, microkernel and libOS directories
# into a single flattened tree for easier analysis.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/flattened_tree"

mkdir -p "$DEST"

copy_file() {
    local src="$1"
    local rel="$2"
    local base target enc
    base=$(basename "$rel")
    target="$DEST/$base"
    if [ -e "$target" ]; then
        enc="${rel//\//_}"
        target="$DEST/$enc"
    fi
    cp "$src" "$target"
}

for dir in exokernel microkernel libos; do
    SRC="$ROOT/$dir"
    [ -d "$SRC" ] || continue
    find "$SRC" -type f -print0 | while IFS= read -r -d '' file; do
        rel="${file#$ROOT/}"
        copy_file "$file" "$rel"
    done
done

echo "Files copied to $DEST"
