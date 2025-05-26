#!/usr/bin/env bash
set -euo pipefail

# Determine repository root
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="$ROOT/localmach/include"
if [ -z "${LITES_SRC_DIR:-}" ]; then
    echo "LITES_SRC_DIR must be set" >&2
    exit 1
fi
SRC_DIR="$LITES_SRC_DIR"

mkdir -p "$DEST"

for MACH in mach4 mach3; do
    SRC="$SRC_DIR/xkernel/$MACH"
    [ -d "$SRC" ] || continue
    while IFS= read -r -d '' FILE; do
        REL="${FILE#${SRC}/}"
        if [[ $REL == include/* ]]; then
            REL=${REL#include/}
        fi
        TARGET="$DEST/$REL"
        mkdir -p "$(dirname "$TARGET")"
        cp "$FILE" "$TARGET"
    done < <(find "$SRC" -name '*.h' -print0)
done

echo "Headers copied to $DEST"
