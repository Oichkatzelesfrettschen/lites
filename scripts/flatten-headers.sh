#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/include"
MAP_FILE=""

usage() {
    echo "Usage: $0 [--map <file>]" >&2
    exit 1
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -m|--map)
            MAP_FILE="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [--map <file>]"
            exit 0
            ;;
        *)
            usage
            ;;
    esac
done

mkdir -p "$DEST"
if [ -n "$MAP_FILE" ]; then
    echo "original,flattened" > "$MAP_FILE"
fi

copy_header() {
    local src="$1"
    local rel="$2"
    local base target enc dest_name
    base=$(basename "$rel")
    target="$DEST/$base"
    if [ -e "$target" ]; then
        # avoid collisions by encoding the relative path
        enc="${rel//\//_}"
        target="$DEST/$enc"
    fi
    cp "$src" "$target"
    if [ -n "$MAP_FILE" ]; then
        dest_name="$(basename "$target")"
        echo "$rel,$dest_name" >> "$MAP_FILE"
    fi
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
