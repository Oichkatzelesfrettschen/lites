#!/bin/sh
set -e

# Change to the BSD directory containing symlinks before running this script
# The script iterates over every pair of BSD source directories and generates
# a unified diff for them. The resulting patch files are stored in the current
# directory.

# Gather list of directories pointed to by the symlinks
DIRS=(
  "$(readlink -f netbsd)"
  "$(readlink -f i386_boot_bsd)"
  "$(readlink -f 4_3BSD_Reno)"
  "$(readlink -f 4_3BSD_Tahoe)"
  "$(readlink -f 4_3UWiscBSD)"
)
N=${#DIRS[@]}

for ((i=0; i<N; i++)); do
  for ((j=i+1; j<N; j++)); do
    name_i=$(basename "${DIRS[i]}")
    name_j=$(basename "${DIRS[j]}")
    patch="${name_i}_vs_${name_j}.patch"
    echo "Generating $patch ..."
    diff -ruN "${DIRS[i]}" "${DIRS[j]}" > "$patch" || true
  done
done

echo "Diff generation complete."
