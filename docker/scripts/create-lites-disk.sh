#!/usr/bin/env bash
# shellcheck disable=SC2034
##
## @file create-lites-disk.sh
## @brief Create QCOW2 disk image for Lites with best practices
## @details Creates a properly configured QCOW2 disk image for running Lites
##          in QEMU with optimal settings for performance and compatibility.
##
## Usage: create-lites-disk.sh [OPTIONS]
##
## Options:
##   -s, --size SIZE     Disk size (default: 2G)
##   -o, --output FILE   Output file path (default: lites.qcow2)
##   -f, --format        Format: qcow2, raw, vdi, vmdk (default: qcow2)
##   -c, --compression   Enable compression (qcow2 only)
##   -h, --help          Show this help message
##

set -euo pipefail

# Script directory for relative paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Default values
DISK_SIZE="2G"
OUTPUT_FILE="lites.qcow2"
DISK_FORMAT="qcow2"
COMPRESSION=""
VERBOSE=0

## Display usage information
usage() {
    sed -n '3,15p' "$0" | sed 's/^## //' | sed 's/^##//'
    exit 0
}

## Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -s|--size)
                DISK_SIZE="$2"
                shift 2
                ;;
            -o|--output)
                OUTPUT_FILE="$2"
                shift 2
                ;;
            -f|--format)
                DISK_FORMAT="$2"
                shift 2
                ;;
            -c|--compression)
                COMPRESSION="-c"
                shift
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -h|--help)
                usage
                ;;
            *)
                echo "Error: Unknown option: $1" >&2
                usage
                ;;
        esac
    done
}

## Create QCOW2 disk image with best practices
create_disk_image() {
    local size="$1"
    local output="$2"
    local format="$3"
    local compression="$4"

    echo "Creating $format disk image: $output ($size)"

    # Check if qemu-img is available
    if ! command -v qemu-img &> /dev/null; then
        echo "Error: qemu-img not found. Install qemu-utils package." >&2
        exit 1
    fi

    # Remove existing file if present
    if [[ -f "$output" ]]; then
        echo "Warning: $output exists. Removing..." >&2
        rm -f "$output"
    fi

    # Create image based on format
    case "$format" in
        qcow2)
            # QCOW2 with lazy_refcounts for better performance
            # cluster_size=64k optimal for most workloads
            # compat=1.1 for modern QEMU features
            # shellcheck disable=SC2086
            qemu-img create -f qcow2 \
                -o lazy_refcounts=on,cluster_size=65536,compat=1.1 \
                $compression \
                "$output" "$size"
            ;;
        raw)
            # Raw format - fastest but no compression/snapshots
            qemu-img create -f raw "$output" "$size"
            ;;
        vdi)
            # VirtualBox format
            qemu-img create -f vdi "$output" "$size"
            ;;
        vmdk)
            # VMware format
            qemu-img create -f vmdk "$output" "$size"
            ;;
        *)
            echo "Error: Unsupported format: $format" >&2
            exit 1
            ;;
    esac

    echo "✅ Disk image created successfully"
    
    # Show image info
    if [[ $VERBOSE -eq 1 ]]; then
        echo ""
        echo "Image information:"
        qemu-img info "$output"
    fi
}

## Create a bootable disk with MBR partition table
create_bootable_disk() {
    local disk="$1"
    
    echo "Setting up bootable disk structure..."
    
    # This would require additional tools like parted, mkfs, etc.
    # For now, create a basic disk that can be partitioned later
    echo "Note: Disk created but not partitioned. Use parted/fdisk to partition." >&2
}

## Main function
main() {
    parse_args "$@"
    
    # Validate disk size format
    if ! [[ "$DISK_SIZE" =~ ^[0-9]+[KMGT]?$ ]]; then
        echo "Error: Invalid disk size format: $DISK_SIZE" >&2
        echo "Use formats like: 2G, 512M, 10G" >&2
        exit 1
    fi
    
    # Create the disk image
    create_disk_image "$DISK_SIZE" "$OUTPUT_FILE" "$DISK_FORMAT" "$COMPRESSION"
    
    echo ""
    echo "Disk image ready: $OUTPUT_FILE"
    echo "To use with QEMU:"
    echo "  qemu-system-i386 -drive file=$OUTPUT_FILE,format=$DISK_FORMAT,if=ide"
}

# Run main function if script is executed (not sourced)
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
