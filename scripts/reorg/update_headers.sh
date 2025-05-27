#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
DEST="$ROOT/flattened/include"
MAP="$DEST/headers.csv"

rm -rf "$DEST"
mkdir -p "$DEST"

# create mapping header
echo "original,flattened" > "$MAP"

copy_header() {
    local src="$1"
    local rel="$2"
    local base target enc dest_name
    base=$(basename "$rel")
    target="$DEST/$base"
    if [ -e "$target" ]; then
        enc="${rel//\//_}"
        target="$DEST/$enc"
    fi
    cp "$src" "$target"
    dest_name="$(basename "$target")"
    echo "$rel,$dest_name" >> "$MAP"
}

if [ -f "$ROOT/headers_inventory.csv" ]; then
    tail -n +2 "$ROOT/headers_inventory.csv" | while IFS= read -r path; do
        file="$ROOT/${path#./}"
        [ -f "$file" ] || continue
        copy_header "$file" "${path#./}"
    done
else
    find "$ROOT" -name '*.h' -print0 |
        while IFS= read -r -d '' file; do
            rel="${file#$ROOT/}"
            copy_header "$file" "$rel"
        done
fi

echo "Flattened headers copied to $DEST"
