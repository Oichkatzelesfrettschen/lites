#!/usr/bin/env bash
set -euo pipefail

# Parse the list of packages from setup.sh between the 'core build tools'
# and the 'Python' section. These packages include the expected build tools
# that should be installed. We check whether the main command for each
# package is available in the PATH.

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SETUP_SH="$ROOT_DIR/setup.sh"

# Extract package names from setup.sh
pkg_lines=$(sed -n '48,58p' "$SETUP_SH" | tr -d '\\' | sed 's/; do//' )
packages=()
while read -r line; do
    for word in $line; do
        case "$word" in
            for|pkg|in|\\) continue ;;
            apt_pin_install|done) continue ;;
            *) packages+=("$word") ;;
        esac
    done
done <<< "$pkg_lines"

missing=()
for pkg in "${packages[@]}"; do
    # Skip meta packages or library-only names
    case "$pkg" in
        build-essential|libopenblas-dev|liblapack-dev|libeigen3-dev|\
        libasan6|libubsan1|systemtap-sdt-dev|linux-perf|ca-certificates|\
        libopenmpi-dev|openmpi-bin|likwid|hwloc|crash)
            continue
            ;;
    esac
    cmd=${pkg}
    # Some packages have different command names
    case "$pkg" in
        g++) cmd=g++ ;;
        ninja-build) cmd=ninja ;;
        byacc|flex|bison|m4) cmd=$pkg ;;
        *) ;;
    esac
    if ! command -v "$cmd" >/dev/null 2>&1; then
        missing+=("$pkg")
    fi
done

if [ ${#missing[@]} -eq 0 ]; then
    echo "All core build tools present."
else
    echo "Missing build tools: ${missing[*]}"
fi
