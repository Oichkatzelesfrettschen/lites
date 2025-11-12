#!/usr/bin/env bash
##
## @file docker-build-and-run.sh
## @brief Complete workflow: Build Lites, create disk image, run in QEMU
## @details End-to-end automation for building and running Lites in Docker+QEMU
##

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Configuration
DOCKER_IMAGE="lites-i386-dev:latest"
# shellcheck disable=SC2034
CONTAINER_NAME="lites-build-$$"  # Reserved for future use
DISK_IMAGE="lites.qcow2"
DISK_SIZE="2G"
MEMORY="256M"
ENABLE_KVM=1

## Build Docker image
build_docker_image() {
    echo "==> Building Docker image: $DOCKER_IMAGE"
    cd "$PROJECT_ROOT/docker"
    docker compose build dev
    echo "✅ Docker image built successfully"
}

## Build Lites inside Docker container
build_lites() {
    echo "==> Building Lites i386 inside Docker container"
    
    docker compose run --rm dev bash -c "
        set -e
        echo 'Setting up build environment...'
        source /workspace/.env.build 2>/dev/null || true
        
        echo 'Running build script...'
        if [[ -f docker/scripts/build-lites-i386.sh ]]; then
            bash docker/scripts/build-lites-i386.sh
        else
            echo 'Build script not found, using Makefile...'
            make -f Makefile.new ARCH=i686 clean all
        fi
        
        echo '✅ Lites build complete'
    "
}

## Create disk image
create_disk_image() {
    echo "==> Creating QEMU disk image: $DISK_IMAGE ($DISK_SIZE)"
    
    docker compose run --rm dev bash -c "
        bash docker/scripts/create-lites-disk.sh \
            --size $DISK_SIZE \
            --output $DISK_IMAGE \
            --format qcow2 \
            --compression
    "
    
    echo "✅ Disk image created: $DISK_IMAGE"
}

## Run Lites in QEMU
run_qemu() {
    echo "==> Running Lites in QEMU (memory: $MEMORY)"
    
    # Detect KVM support
    KVM_ARGS=""
    if [[ $ENABLE_KVM -eq 1 ]] && [[ -e /dev/kvm ]]; then
        echo "KVM acceleration enabled"
        KVM_ARGS="-enable-kvm -cpu host"
    else
        echo "KVM not available, using emulation"
        KVM_ARGS="-cpu qemu32"
    fi
    
    docker compose run --rm --device=/dev/kvm:/dev/kvm dev bash -c "
        qemu-system-i386 \
            $KVM_ARGS \
            -m $MEMORY \
            -drive file=$DISK_IMAGE,format=qcow2,if=ide \
            -netdev user,id=net0,hostfwd=tcp::2222-:22 \
            -device e1000,netdev=net0 \
            -serial stdio \
            -nographic \
            -no-reboot
    " || echo "QEMU exited"
}

## Display help
usage() {
    cat <<EOF
Usage: $0 [COMMAND]

Commands:
    build       - Build Docker image
    compile     - Build Lites inside Docker
    disk        - Create QEMU disk image
    run         - Run Lites in QEMU
    all         - Run complete workflow (default)
    help        - Show this help

Environment variables:
    DISK_SIZE   - Disk image size (default: 2G)
    MEMORY      - QEMU memory (default: 256M)
    ENABLE_KVM  - Enable KVM if available (default: 1)

Examples:
    $0 all              # Complete workflow
    $0 compile          # Only build Lites
    $0 run              # Only run in QEMU
EOF
}

## Main function
main() {
    local command="${1:-all}"
    
    cd "$PROJECT_ROOT"
    
    case "$command" in
        build)
            build_docker_image
            ;;
        compile)
            build_lites
            ;;
        disk)
            create_disk_image
            ;;
        run)
            run_qemu
            ;;
        all)
            build_docker_image
            build_lites
            create_disk_image
            run_qemu
            ;;
        help|--help|-h)
            usage
            ;;
        *)
            echo "Error: Unknown command: $command" >&2
            usage
            exit 1
            ;;
    esac
}

main "$@"
