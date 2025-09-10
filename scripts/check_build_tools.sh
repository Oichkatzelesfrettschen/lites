#!/usr/bin/env bash
set -euo pipefail

# Parse the package list from `setup.sh` by locating the packages array.
# This approach keeps the tool check aligned with the source of truth used by
# the setup script itself and avoids reliance on brittle line numbers.

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SETUP_SCRIPT="$ROOT_DIR/setup.sh"

# Abort early if the setup script is missing; without it we cannot determine
# the expected toolchain packages.
if [[ ! -f "$SETUP_SCRIPT" ]]; then
    echo "Setup script not found: $SETUP_SCRIPT" >&2
    exit 1
fi

##
# \brief Extract package names from \c setup.sh.
#
# The function scans for the \c packages array definition and collects each
# entry until the closing parenthesis. Inline comments are stripped to avoid
# accidental inclusion in the package list.
#
# \return Populates the global array \c packages with the parsed package names.
extract_packages() {
    local line in_block=false
    packages=()

    while IFS= read -r line; do
        # Remove inline comments for robust parsing.
        line="${line%%#*}"

        if [[ $in_block == false ]]; then
            if [[ $line =~ ^packages=\( ]]; then
                in_block=true
                line="${line#packages=(}"
            else
                continue
            fi
        fi

        if [[ $in_block == true ]]; then
            if [[ $line =~ \) ]]; then
                line="${line%%\)*}"
                in_block=false
            fi

            for word in $line; do
                [[ -z $word ]] && continue
                packages+=("$word")
            done

            [[ $in_block == false ]] && break
        fi
    done < "$SETUP_SCRIPT"
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
