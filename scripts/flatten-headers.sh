#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/include"

mkdir -p "$DEST"

copy_header() {
    local src="$1"
    local rel="$2"
    local base
    base=$(basename "$rel")
    local target="$DEST/$base"
    if [ -e "$target" ]; then
        # avoid collisions by encoding the relative path
        local enc
        enc="${rel//\//_}"
        target="$DEST/$enc"
    fi
    cp "$src" "$target"
}

if [ -f "$ROOT/headers_inventory.csv" ]; then
    tail -n +2 "$ROOT/headers_inventory.csv" | while IFS= read -r path; do
        file="$ROOT/${path#./}"
        [ -f "$file" ] || continue
        copy_header "$file" "${path#./}"
    done
else
    find "$ROOT" -name '*.h' -print0 \
        | while IFS= read -r -d '' file; do
            rel="${file#$ROOT/}"
            copy_header "$file" "$rel"
        done
fi

echo "Headers copied to $DEST"
