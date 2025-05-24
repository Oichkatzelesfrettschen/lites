#!/usr/bin/env bash
set -euo pipefail

# Determine repository root
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/localmach/include"

mkdir -p "$DEST"

for MACH in mach4 mach3; do
    SRC="$ROOT/src-lites-1.1-2025/xkernel/$MACH"
    [ -d "$SRC" ] || continue
    while IFS= read -r -d '' FILE; do
        REL="${FILE#${SRC}/}"
        REL="${REL/\/include\//\/}"
        TARGET="$DEST/$REL"
        mkdir -p "$(dirname "$TARGET")"
        cp "$FILE" "$TARGET"
    done < <(find "$SRC" -name '*.h' -print0)
done

echo "Headers copied to $DEST"
