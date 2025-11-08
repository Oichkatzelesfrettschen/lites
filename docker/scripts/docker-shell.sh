#!/usr/bin/env bash
set -euo pipefail

##
# @file docker-shell.sh
# @brief Enter the Lites i386 development Docker container
#
# This script provides an easy way to start and enter the Docker container
# for Lites i386 development. It handles container lifecycle and provides
# a convenient interactive shell.
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

##
# @brief Print usage information
##
usage() {
    cat <<EOF
Usage: $0 [OPTIONS] [COMMAND]

Enter the Lites i386 development Docker container.

OPTIONS:
    -h, --help          Show this help message
    -b, --build         Build/rebuild the Docker image
    -r, --root          Enter container as root user
    --privileged        Run with privileged mode (for KVM)

COMMAND:
    If provided, execute COMMAND in the container instead of starting a shell.

EXAMPLES:
    # Enter interactive shell
    $0

    # Rebuild image and enter shell
    $0 --build

    # Run a specific command
    $0 ls -la /workspace

    # Build Lites directly
    $0 docker/scripts/build-lites-i386.sh
EOF
}

##
# @brief Check if Docker is installed
##
check_docker() {
    if ! command -v docker >/dev/null 2>&1; then
        echo "Error: Docker is not installed" >&2
        echo "Please install Docker: https://docs.docker.com/get-docker/" >&2
        exit 1
    fi
    
    if ! docker info >/dev/null 2>&1; then
        echo "Error: Docker daemon is not running" >&2
        echo "Please start Docker and try again" >&2
        exit 1
    fi
}

##
# @brief Check if Docker Compose is available
##
check_compose() {
    if docker compose version >/dev/null 2>&1; then
        echo "docker compose"
    elif command -v docker-compose >/dev/null 2>&1; then
        echo "docker-compose"
    else
        echo "Error: Docker Compose is not installed" >&2
        echo "Please install Docker Compose" >&2
        exit 1
    fi
}

##
# @brief Build the Docker image
##
build_image() {
    echo "Building Docker image..."
    cd "${PROJECT_ROOT}"
    
    local compose_cmd
    compose_cmd=$(check_compose)
    
    $compose_cmd -f docker/docker-compose.yml build lites-i386-dev
    echo "Docker image built successfully!"
}

##
# @brief Start and enter the container
##
enter_container() {
    local user_arg="${1:-developer}"
    local privileged="${2:-no}"
    shift 2
    local cmd=("$@")
    
    cd "${PROJECT_ROOT}"
    
    local compose_cmd
    compose_cmd=$(check_compose)
    
    # Prepare docker run arguments
    local run_args=()
    
    if [[ "$user_arg" == "root" ]]; then
        run_args+=(--user root)
    fi
    
    # Check if we should use privileged mode
    if [[ "$privileged" == "yes" ]] || [[ -e /dev/kvm ]]; then
        run_args+=(--privileged)
        run_args+=(--device /dev/kvm:/dev/kvm)
    fi
    
    # Start container if not running
    if ! docker ps --format '{{.Names}}' | grep -q "^lites-i386-dev$"; then
        echo "Starting container..."
        $compose_cmd -f docker/docker-compose.yml up -d lites-i386-dev
    fi
    
    # Execute command or start shell
    if [[ ${#cmd[@]} -gt 0 ]]; then
        docker exec "${run_args[@]}" -it lites-i386-dev "${cmd[@]}"
    else
        docker exec "${run_args[@]}" -it lites-i386-dev /bin/bash
    fi
}

##
# @brief Main entry point
##
main() {
    local build=false
    local user="developer"
    local privileged="no"
    local cmd=()
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -b|--build)
                build=true
                shift
                ;;
            -r|--root)
                user="root"
                shift
                ;;
            --privileged)
                privileged="yes"
                shift
                ;;
            --)
                shift
                cmd=("$@")
                break
                ;;
            *)
                cmd=("$@")
                break
                ;;
        esac
    done
    
    # Check dependencies
    check_docker
    check_compose >/dev/null
    
    # Build if requested
    if [[ "$build" == "true" ]]; then
        build_image
    fi
    
    # Enter container
    enter_container "$user" "$privileged" "${cmd[@]}"
}

main "$@"
