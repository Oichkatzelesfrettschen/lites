#!/usr/bin/env bash
# Move all header files from the repository into the Lites source include
# directory while preserving their relative paths. This script can be used to
# consolidate headers for analysis purposes.

set -euo pipefail

TARGET_BASE="src-lites-1.1-2025/include/all_headers"

# Create the destination base directory
mkdir -p "$TARGET_BASE"

# Find every .h file outside of the target directory and move it preserving
# the original path structure under the target base.
find . -path "./$TARGET_BASE/*" -prune -o -name '*.h' -print | while read -r file; do
  relpath="${file#./}"
  dest="$TARGET_BASE/$relpath"
  mkdir -p "$(dirname "$dest")"
  git mv "$file" "$dest"
done
