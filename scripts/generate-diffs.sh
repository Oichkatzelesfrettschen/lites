#!/usr/bin/env bash
set -euo pipefail

# Directory to store diff output
DIFF_DIR="$(git rev-parse --show-toplevel)/diffs"
mkdir -p "$DIFF_DIR"

# Array defining version pairs (from,to)
VERSION_PAIRS=(
  "lites-1.0 lites-1.1"
  "lites-1.1 lites-1.1-950808"
  "lites-1.1-950808 lites-1.1.u1"
  "lites-1.1.u1 lites-1.1.u2"
  "lites-1.1.u2 lites-1.1.u3"
  "lites-1.1.u3 lites-1.1.1"
  "lites-1.1 lites-1.1.1"
)

for pair in "${VERSION_PAIRS[@]}"; do
  set -- $pair
  from="$1"; to="$2";
  out="$DIFF_DIR/${from}_to_${to}.patch"
  echo "Generating diff $from -> $to ..."
  diff -urN "$from" "$to" > "$out" || true
  gzip -f "$out"
  echo "Stored $out.gz"
  echo
done

echo "Diff generation complete. Files stored in $DIFF_DIR"
