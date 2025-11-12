#!/usr/bin/env bash
set -euo pipefail

##
# @file setup-mach-headers.sh
# @brief Set up Mach kernel headers for Lites build
#
# This script prepares the Mach kernel headers needed to build Lites.
# It can use headers from the Docker image, clone them from GitHub, or
# use local headers.
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
LOCALMACH_DIR="${REPO_ROOT}/localmach"

##
# @brief Print usage information
##
usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Set up Mach kernel headers for building Lites.

OPTIONS:
    -h, --help          Show this help message
    --docker            Use headers from Docker image (/opt/mach/osfmk)
    --clone             Clone fresh headers from GitHub
    --local PATH        Use headers from local path

DESCRIPTION:
    This script sets up the Mach kernel headers required to build Lites.
    It can use pre-installed headers from the Docker image, clone them
    from nmartin0's osfmk6.1 repository, or use a local installation.

    The headers are placed in: ${LOCALMACH_DIR}/include

EXAMPLES:
    # Use headers from Docker image (default in container)
    $0 --docker

    # Clone fresh headers from GitHub
    $0 --clone

    # Use local Mach installation
    $0 --local /path/to/osfmk

EOF
}

##
# @brief Set up headers from Docker image
##
setup_from_docker() {
    local docker_mach="/opt/mach/osfmk"
    
    if [[ ! -d "${docker_mach}" ]]; then
        echo "Error: Docker Mach headers not found at ${docker_mach}" >&2
        echo "Hint: Are you running inside the lites-i386-dev container?" >&2
        return 1
    fi
    
    echo "Setting up Mach headers from Docker image..."
    echo "Source: ${docker_mach}"
    echo "Target: ${LOCALMACH_DIR}"
    
    # Create localmach directory structure
    mkdir -p "${LOCALMACH_DIR}/include"
    
    # Copy headers from Docker Mach installation
    if [[ -d "${docker_mach}/kernel/src" ]]; then
        echo "Copying kernel headers..."
        rsync -av "${docker_mach}/kernel/src/" "${LOCALMACH_DIR}/include/" \
            --include='*.h' --include='*/' --exclude='*' 2>/dev/null || true
    fi
    
    echo "✓ Mach headers set up in ${LOCALMACH_DIR}/include"
}

##
# @brief Clone headers from GitHub
##
setup_from_github() {
    local temp_dir
    temp_dir=$(mktemp -d)
    
    echo "Cloning Mach headers from GitHub..."
    echo "Repository: https://github.com/nmartin0/osfmk6.1"
    
    git clone --depth 1 https://github.com/nmartin0/osfmk6.1.git "${temp_dir}/osfmk"
    
    mkdir -p "${LOCALMACH_DIR}/include"
    
    if [[ -d "${temp_dir}/osfmk/kernel/src" ]]; then
        echo "Copying kernel headers..."
        rsync -av "${temp_dir}/osfmk/kernel/src/" "${LOCALMACH_DIR}/include/" \
            --include='*.h' --include='*/' --exclude='*'
    fi
    
    rm -rf "${temp_dir}"
    
    echo "✓ Mach headers cloned and set up in ${LOCALMACH_DIR}/include"
}

##
# @brief Set up headers from local path
# @param $1 Path to local Mach installation
##
setup_from_local() {
    local local_path="$1"
    
    if [[ ! -d "${local_path}" ]]; then
        echo "Error: Local path does not exist: ${local_path}" >&2
        return 1
    fi
    
    echo "Setting up Mach headers from local path..."
    echo "Source: ${local_path}"
    echo "Target: ${LOCALMACH_DIR}"
    
    mkdir -p "${LOCALMACH_DIR}/include"
    
    # Try various common structures
    if [[ -d "${local_path}/kernel/src" ]]; then
        rsync -av "${local_path}/kernel/src/" "${LOCALMACH_DIR}/include/" \
            --include='*.h' --include='*/' --exclude='*'
    elif [[ -d "${local_path}/include" ]]; then
        rsync -av "${local_path}/include/" "${LOCALMACH_DIR}/include/" \
            --include='*.h' --include='*/' --exclude='*'
    else
        echo "Error: Could not find headers in ${local_path}" >&2
        return 1
    fi
    
    echo "✓ Mach headers set up from local path"
}

##
# @brief Main function
##
main() {
    local mode="auto"
    local local_path=""
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            --docker)
                mode="docker"
                shift
                ;;
            --clone)
                mode="clone"
                shift
                ;;
            --local)
                mode="local"
                if [[ $# -lt 2 ]]; then
                    echo "Error: --local requires a path argument" >&2
                    exit 1
                fi
                local_path="$2"
                shift 2
                ;;
            *)
                echo "Error: Unknown option: $1" >&2
                usage
                exit 1
                ;;
        esac
    done
    
    # Auto-detect mode if needed
    if [[ "${mode}" == "auto" ]]; then
        if [[ -d "/opt/mach/osfmk" ]]; then
            echo "Auto-detected: Running in Docker container"
            mode="docker"
        else
            echo "Auto-detected: Not in Docker container, will clone from GitHub"
            mode="clone"
        fi
    fi
    
    # Execute based on mode
    case "${mode}" in
        docker)
            setup_from_docker
            ;;
        clone)
            setup_from_github
            ;;
        local)
            setup_from_local "${local_path}"
            ;;
        *)
            echo "Error: Invalid mode: ${mode}" >&2
            exit 1
            ;;
    esac
    
    # Verify headers were set up
    if [[ -d "${LOCALMACH_DIR}/include" ]]; then
        local header_count
        header_count=$(find "${LOCALMACH_DIR}/include" -name "*.h" | wc -l)
        echo ""
        echo "Summary:"
        echo "  Location: ${LOCALMACH_DIR}/include"
        echo "  Headers found: ${header_count}"
        echo ""
        echo "You can now build Lites with:"
        echo "  export LITES_MACH_DIR=${LOCALMACH_DIR}"
        echo "  cmake -B build -DLITES_MACH_DIR=${LOCALMACH_DIR}"
        echo "  cmake --build build"
    else
        echo "Error: Failed to set up Mach headers" >&2
        exit 1
    fi
}

main "$@"
