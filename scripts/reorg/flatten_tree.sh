#!/usr/bin/env bash
# Copy files from exokernel, microkernel and libOS directories
# into a single flattened/ directory. Paths are encoded with
# underscores to avoid collisions.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="$ROOT/flattened"

components=(exokernel microkernel libOS)

mkdir -p "$DEST"

for comp in "${components[@]}"; do
    src="$ROOT/$comp"
    [ -d "$src" ] || continue
    mkdir -p "$DEST/$comp"
    find "$src" -type f -print0 | while IFS= read -r -d '' file; do
        rel="${file#$src/}"
        target="$DEST/$comp/${rel//\//_}"
        cp "$file" "$target"
    done
done

echo "Flattened files copied to $DEST"
