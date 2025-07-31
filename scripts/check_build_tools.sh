#!/usr/bin/env bash
set -euo pipefail

# Parse the list of packages from setup.sh by reading the block starting from
# the line containing `packages=` up to the closing parenthesis. This ensures
# that the package list remains accurate even if setup.sh changes.

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SETUP_SH="$ROOT_DIR/setup.sh"

##
# \brief Extract package names from setup.sh.
#
# The function locates the `packages=` block within setup.sh and reads each
# package name listed until the closing parenthesis. Continuation backslashes
# are stripped so that the resulting words represent actual package names.
#
# \return Populates the global array \c packages with the parsed package names.
extract_packages() {
    local line in_list=false
    packages=()
    while IFS= read -r line; do
        if [[ $line =~ ^packages=\( ]]; then
            in_list=true
            continue
        fi
        if [[ $in_list == true ]]; then
            [[ $line =~ ^\) ]] && break
            line="${line%\\}"
            for word in $line; do
                packages+=("$word")
            done
        fi
    done < "$SETUP_SH"
}

##
# \brief Check that each build tool's command exists in the PATH.
#
# Packages that do not correspond to user-facing commands are skipped.
# Any missing commands are collected in the global array \c missing.
check_build_tools() {
    local pkg cmd
    missing=()
    for pkg in "${packages[@]}"; do
        case "$pkg" in
            build-essential|libopenblas-dev|liblapack-dev|libeigen3-dev|\
            libasan6|libubsan1|systemtap-sdt-dev|linux-perf|ca-certificates|\
            libopenmpi-dev|openmpi-bin|likwid|hwloc|crash)
                continue
                ;;
        esac

        cmd="$pkg"
        case "$pkg" in
            g++) cmd=g++ ;;
            ninja-build) cmd=ninja ;;
            byacc|flex|bison|m4) cmd="$pkg" ;;
        esac

        if ! command -v "$cmd" >/dev/null 2>&1; then
            missing+=("$pkg")
        fi
    done
}

extract_packages
check_build_tools

if [ ${#missing[@]} -eq 0 ]; then
    echo "All core build tools present."
else
    echo "Missing build tools: ${missing[*]}"
fi
