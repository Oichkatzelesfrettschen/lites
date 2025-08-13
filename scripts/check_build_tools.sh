#!/usr/bin/env bash
set -euo pipefail

# Parse the package list from `docs/setup.md` by scanning the fenced code block
# that starts with `sudo apt install -y`. This keeps the tool check in sync
# with the documentation.

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SETUP_DOC="$ROOT_DIR/docs/setup.md"

##
# \brief Extract package names from `docs/setup.md`.
#
# The function locates the `sudo apt install -y` block and reads each package
# name until the closing triple backticks. Continuation backslashes are stripped
# so the resulting words represent actual package names.
#
# \return Populates the global array \c packages with the parsed package names.
extract_packages() {
    local line in_block=false
    packages=()
    while IFS= read -r line; do
        if [[ $line =~ ^sudo\ apt\ install ]]; then
            in_block=true
            line="${line#*install -y }"
        fi
        if [[ $in_block == true ]]; then
            if [[ $line == '```' ]]; then
                break
            fi
            line="${line%\\}"
            for word in $line; do
                packages+=("$word")
            done
        fi
    done < "$SETUP_DOC"
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
