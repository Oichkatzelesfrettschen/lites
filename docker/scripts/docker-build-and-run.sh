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

## Create bootable disk image (.img)
create_bootable_image() {
    echo "==> Creating bootable disk image (.img)"

    local raw_image="lites-boot.img"

    # Run with privileged mode for loop device access
    docker compose run --rm --privileged dev bash -c "
        bash docker/scripts/create-bootable-image.sh \
            --size $DISK_SIZE \
            --output $raw_image \
            --lites build-i386/lites_emulator
    "

    echo "✅ Bootable .img created: $raw_image"
    echo "$raw_image"
}

## Convert .img to QCOW2
convert_to_qcow2() {
    local raw_image="$1"

    echo "==> Converting $raw_image to QCOW2: $DISK_IMAGE"

    docker compose run --rm dev bash -c "
        bash docker/scripts/convert-img-to-qcow2.sh \
            --input $raw_image \
            --output $DISK_IMAGE \
            --compress \
            --prealloc metadata
    "

    echo "✅ QCOW2 image created: $DISK_IMAGE"
}

## Run Lites in QEMU
run_qemu() {
    echo "==> Running Lites in QEMU (memory: $MEMORY)"

    # Use the run-qemu-i386.sh script which auto-detects boot mode
    docker compose run --rm --device=/dev/kvm:/dev/kvm dev bash -c "
        docker/scripts/run-qemu-i386.sh --memory $MEMORY
    " || echo "QEMU exited"
}

## Display help
usage() {
    cat <<EOF
Usage: $0 [COMMAND]

Commands:
    build       - Build Docker image
    compile     - Build Lites inside Docker
    bootimg     - Create bootable .img file
    qcow2       - Convert .img to QCOW2
    disk        - Create bootable .img and convert to QCOW2
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
        bootimg)
            create_bootable_image
            ;;
        qcow2)
            local raw_img="${2:-lites-boot.img}"
            convert_to_qcow2 "$raw_img"
            ;;
        disk)
            local raw_img
            raw_img=$(create_bootable_image)
            convert_to_qcow2 "$raw_img"
            ;;
        run)
            run_qemu
            ;;
        all)
            build_docker_image
            build_lites
            local raw_img
            raw_img=$(create_bootable_image)
            convert_to_qcow2 "$raw_img"
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
