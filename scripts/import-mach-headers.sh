#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/localmach/include"

copy_headers() {
    local src="$1"
    find "$src" -name '*.h' -print0 | while IFS= read -r -d '' file; do
        rel="${file#$src/}"
        target="$DEST/$rel"
        mkdir -p "$(dirname "$target")"
        cp "$file" "$target"
    done
}

if [ -d "$ROOT/openmach/include" ]; then
    mkdir -p "$DEST"
    copy_headers "$ROOT/openmach/include"
    echo "Headers copied from openmach to $DEST"
    exit 0
fi

if [ -d "$ROOT/src-lites-1.1-2025/xkernel" ]; then
    "$ROOT/scripts/extract-xmach-headers.sh"
    exit 0
fi

echo "No Mach headers found (openmach or xkernel)" >&2
exit 1
