#!/usr/bin/env bash
set -euo pipefail

##
# @file setup-build-environment.sh
# @brief Set up the build environment for Lites
#
# This script prepares the build environment by:
# - Setting up Mach kernel headers
# - Configuring source directories
# - Validating dependencies
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== Setting up Lites build environment ==="

# Set source directory to Items1/lites-1.1.u3 if it exists
if [[ -d "${ROOT_DIR}/Items1/lites-1.1.u3" ]]; then
    export LITES_SRC_DIR="${ROOT_DIR}/Items1/lites-1.1.u3"
    echo "✓ Found Lites source at: ${LITES_SRC_DIR}"
elif [[ -d "${ROOT_DIR}/build/lites-1.1.u3" ]]; then
    export LITES_SRC_DIR="${ROOT_DIR}/build/lites-1.1.u3"
    echo "✓ Found Lites source at: ${LITES_SRC_DIR}"
else
    echo "⚠ Lites source directory not found, using current directory"
    export LITES_SRC_DIR="${ROOT_DIR}"
fi

# Set up Mach headers
setup_mach_headers() {
    echo "Setting up Mach headers..."
    
    # Check if headers already exist
    if [[ -d "${ROOT_DIR}/localmach/include" ]]; then
        echo "✓ Mach headers already exist in localmach/include"
        export LITES_MACH_DIR="${ROOT_DIR}/localmach"
        return 0
    fi
    
    # Check for openmach submodule
    if [[ -d "${ROOT_DIR}/openmach/include" ]]; then
        echo "✓ Using Mach headers from openmach submodule"
        export LITES_MACH_DIR="${ROOT_DIR}/openmach"
        return 0
    fi
    
    # Check for OSF Mach 6.1 (from Docker or manual installation)
    if [[ -d "/opt/mach/osfmk/kernel/src" ]]; then
        echo "✓ Using Mach headers from OSF Mach 6.1 at /opt/mach/osfmk"
        export LITES_MACH_DIR="/opt/mach/osfmk/kernel/src"
        export MACH_INCLUDE_DIR="/opt/mach/osfmk/kernel/src"
        return 0
    fi
    
    # Try to create minimal header stubs for basic builds
    echo "⚠ No Mach headers found, creating minimal stubs..."
    mkdir -p "${ROOT_DIR}/localmach/include/mach"
    cat > "${ROOT_DIR}/localmach/include/mach/mach.h" <<'EOF'
/* Minimal Mach header stub for basic builds */
#ifndef _MACH_MACH_H_
#define _MACH_MACH_H_

typedef int kern_return_t;
typedef int mach_port_t;

#define KERN_SUCCESS 0

#endif /* _MACH_MACH_H_ */
EOF
    
    echo "✓ Created minimal Mach header stubs in localmach/include"
    export LITES_MACH_DIR="${ROOT_DIR}/localmach"
    
    return 0
}

setup_mach_headers

# Export environment variables
cat > "${ROOT_DIR}/.env.build" <<EOF
export LITES_SRC_DIR="${LITES_SRC_DIR:-${ROOT_DIR}}"
export LITES_MACH_DIR="${LITES_MACH_DIR:-}"
export MACH_INCLUDE_DIR="${MACH_INCLUDE_DIR:-}"
EOF

echo ""
echo "Build environment configured:"
echo "  LITES_SRC_DIR=${LITES_SRC_DIR:-${ROOT_DIR}}"
echo "  LITES_MACH_DIR=${LITES_MACH_DIR:-<not set>}"
echo "  MACH_INCLUDE_DIR=${MACH_INCLUDE_DIR:-<not set>}"
echo ""
echo "To use these settings in your shell, run:"
echo "  source .env.build"
echo ""
echo "✓ Build environment setup complete"
