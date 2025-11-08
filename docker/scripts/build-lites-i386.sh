#!/usr/bin/env bash
set -euo pipefail

##
# @file build-lites-i386.sh
# @brief Build Lites for i386 architecture inside Docker container
#
# This script automates the build process for Lites targeting the i386
# architecture. It sets up the environment, checks for dependencies,
# and invokes the appropriate build system.
##

# Note: SCRIPT_DIR is unused but kept for potential future use
# shellcheck disable=SC2034
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="${WORKSPACE_ROOT:-/workspace}"
BUILD_DIR="${BUILD_DIR:-${WORKSPACE_ROOT}/build-i386}"
ARCH="${ARCH:-i686}"

##
# @brief Print usage information
##
usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Build Lites for i386 architecture.

OPTIONS:
    -h, --help          Show this help message
    -c, --clean         Clean build directory before building
    -j, --jobs N        Number of parallel jobs (default: auto)
    --cmake             Use CMake build system (default)
    --make              Use Makefile.new build system

ENVIRONMENT VARIABLES:
    WORKSPACE_ROOT      Repository root (default: /workspace)
    BUILD_DIR           Build output directory (default: /workspace/build-i386)
    ARCH                Target architecture (default: i686)
    LITES_MACH_DIR      Mach kernel headers directory
    SRCDIR              Source directory (default: Items1/lites-1.1.u3)

EXAMPLES:
    # Build with CMake (default)
    $0

    # Clean build with 4 jobs
    $0 --clean -j 4

    # Build using Makefile.new
    $0 --make
EOF
}

##
# @brief Clean the build directory
##
clean_build() {
    echo "Cleaning build directory: ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
}

##
# @brief Check for required dependencies
##
check_dependencies() {
    local missing_deps=()
    
    # Check for essential tools
    for tool in gcc make cmake mig; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_deps+=("$tool")
        fi
    done
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        echo "Error: Missing required dependencies: ${missing_deps[*]}" >&2
        echo "Please install them or rebuild the Docker container." >&2
        exit 1
    fi
}

##
# @brief Build using CMake
##
build_with_cmake() {
    local jobs="${1:-$(nproc)}"
    
    echo "Building Lites i386 with CMake..."
    echo "  Architecture: ${ARCH}"
    echo "  Build directory: ${BUILD_DIR}"
    echo "  Jobs: ${jobs}"
    
    cd "${WORKSPACE_ROOT}"
    
    # Configure CMake
    cmake -B "${BUILD_DIR}" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DARCH="${ARCH}" \
        -DCMAKE_C_COMPILER=gcc \
        -DCMAKE_CXX_COMPILER=g++ \
        ${LITES_MACH_DIR:+-DLITES_MACH_DIR="${LITES_MACH_DIR}"} \
        ${LITES_MACH_LIB_DIR:+-DLITES_MACH_LIB_DIR="${LITES_MACH_LIB_DIR}"} \
        ${SRCDIR:+-DLITES_SRC_DIR="${SRCDIR}"}
    
    # Build
    cmake --build "${BUILD_DIR}" -j "${jobs}"
    
    echo "Build completed successfully!"
    echo "Binaries are in: ${BUILD_DIR}"
}

##
# @brief Build using Makefile.new
##
build_with_make() {
    local jobs="${1:-$(nproc)}"
    
    echo "Building Lites i386 with Makefile.new..."
    echo "  Architecture: ${ARCH}"
    echo "  Jobs: ${jobs}"
    
    cd "${WORKSPACE_ROOT}"
    
    make -f Makefile.new \
        -j "${jobs}" \
        ARCH="${ARCH}" \
        ${LITES_MACH_DIR:+LITES_MACH_DIR="${LITES_MACH_DIR}"} \
        ${LITES_MACH_LIB_DIR:+LITES_MACH_LIB_DIR="${LITES_MACH_LIB_DIR}"} \
        ${SRCDIR:+SRCDIR="${SRCDIR}"}
    
    echo "Build completed successfully!"
}

##
# @brief Main entry point
##
main() {
    local clean=false
    local jobs
    local build_system="cmake"
    
    jobs="$(nproc)"
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -c|--clean)
                clean=true
                shift
                ;;
            -j|--jobs)
                jobs="$2"
                shift 2
                ;;
            --cmake)
                build_system="cmake"
                shift
                ;;
            --make)
                build_system="make"
                shift
                ;;
            *)
                echo "Unknown option: $1" >&2
                usage
                exit 1
                ;;
        esac
    done
    
    # Check dependencies
    check_dependencies
    
    # Clean if requested
    if [[ "$clean" == "true" ]]; then
        clean_build
    fi
    
    # Build
    case "$build_system" in
        cmake)
            build_with_cmake "$jobs"
            ;;
        make)
            build_with_make "$jobs"
            ;;
    esac
}

main "$@"
