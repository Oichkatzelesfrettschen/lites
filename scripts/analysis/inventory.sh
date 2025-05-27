#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

OUT_DIR="$ROOT/analysis"
mkdir -p "$OUT_DIR"

bsd_list="$OUT_DIR/bsd_files.txt"
mach_list="$OUT_DIR/mach_files.txt"
lites_list="$OUT_DIR/lites_files.txt"

: > "$bsd_list"
: > "$mach_list"
: > "$lites_list"

find . -type f \( -name '*.c' -o -name '*.h' -o -name '*.S' -o -name '*.s' -o -name '*.asm' \) -print0 |
while IFS= read -r -d '' file; do
    rel="${file#./}"
    if grep -qi "University of California" "$file"; then
        echo "$rel" >> "$bsd_list"
    fi
    if grep -qi "Carnegie Mellon" "$file"; then
        echo "$rel" >> "$mach_list"
    fi
    if grep -qiE "(Lites|University of Utah|hut.fi)" "$file"; then
        echo "$rel" >> "$lites_list"
    fi
done

sort -u -o "$bsd_list" "$bsd_list"
sort -u -o "$mach_list" "$mach_list"
sort -u -o "$lites_list" "$lites_list"

echo "BSD files listed in $bsd_list"
echo "Mach files listed in $mach_list"
echo "Lites files listed in $lites_list"
