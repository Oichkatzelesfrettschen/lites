#!/usr/bin/env bash
set -euo pipefail

##
# @file run-qemu-i386.sh
# @brief Launch QEMU i386 instance with Lites kernel
#
# This script starts a QEMU i386 virtual machine running the Lites
# server. It supports both KVM acceleration and pure emulation mode,
# and provides various options for debugging and development.
##

# Note: SCRIPT_DIR is unused but kept for potential future use
# shellcheck disable=SC2034
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="${WORKSPACE_ROOT:-/workspace}"
BUILD_DIR="${BUILD_DIR:-${WORKSPACE_ROOT}/build-i386}"
QEMU_IMAGES_DIR="${QEMU_IMAGES_DIR:-${WORKSPACE_ROOT}/qemu-images}"

# QEMU configuration
QEMU_MEMORY="${QEMU_MEMORY:-256M}"
QEMU_CPUS="${QEMU_CPUS:-1}"
QEMU_KVM="${QEMU_KVM:-auto}"

##
# @brief Print usage information
##
usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Launch QEMU i386 instance with Lites kernel.

OPTIONS:
    -h, --help              Show this help message
    -k, --kernel PATH       Path to kernel binary (default: auto-detect)
    -i, --image PATH        Boot from disk image (.img or .qcow2)
    -m, --memory SIZE       Memory size (default: 256M)
    -c, --cpus NUM          Number of CPUs (default: 1)
    --kvm                   Force KVM acceleration
    --no-kvm                Disable KVM acceleration
    --serial                Use serial console (default)
    --vnc                   Enable VNC display
    --monitor               Enable QEMU monitor
    --gdb PORT              Enable GDB server on PORT
    -d, --debug             Enable QEMU debug output

ENVIRONMENT VARIABLES:
    WORKSPACE_ROOT          Repository root (default: /workspace)
    BUILD_DIR               Build directory (default: /workspace/build-i386)
    QEMU_IMAGES_DIR         QEMU disk images directory
    QEMU_MEMORY             Memory size (default: 256M)
    QEMU_CPUS               Number of CPUs (default: 1)
    QEMU_KVM                KVM mode: auto, yes, no (default: auto)

EXAMPLES:
    # Boot from disk image
    $0 --image lites.qcow2

    # Boot kernel directly
    $0 --kernel build-i386/lites_emulator

    # With more memory and CPUs
    $0 -i lites.qcow2 -m 512M -c 2

    # Enable GDB debugging
    $0 --image lites.qcow2 --gdb 1234

    # Force pure emulation (no KVM)
    $0 --no-kvm
EOF
}

##
# @brief Detect if KVM is available
#
# @return 0 if KVM is available, 1 otherwise
##
check_kvm() {
    if [[ -e /dev/kvm ]] && [[ -w /dev/kvm ]]; then
        return 0
    fi
    return 1
}

##
# @brief Find disk image
#
# @param image_path Optional explicit image path
# @return Prints the image path if found
##
find_disk_image() {
    local image_path="${1:-}"

    if [[ -n "$image_path" ]] && [[ -f "$image_path" ]]; then
        echo "$image_path"
        return 0
    fi

    # Search for disk images in common locations
    local candidates=(
        "${WORKSPACE_ROOT}/lites.qcow2"
        "${WORKSPACE_ROOT}/lites-boot.img"
        "${WORKSPACE_ROOT}/lites.img"
        "${QEMU_IMAGES_DIR}/lites.qcow2"
        "${QEMU_IMAGES_DIR}/lites-boot.img"
    )

    for candidate in "${candidates[@]}"; do
        if [[ -f "$candidate" ]]; then
            echo "$candidate"
            return 0
        fi
    done

    return 1
}

##
# @brief Find the kernel binary
#
# @param kernel_path Optional explicit kernel path
# @return Prints the kernel path if found
##
find_kernel() {
    local kernel_path="${1:-}"

    if [[ -n "$kernel_path" ]] && [[ -f "$kernel_path" ]]; then
        echo "$kernel_path"
        return 0
    fi

    # Search for kernel in common locations
    local candidates=(
        "${BUILD_DIR}/lites_server"
        "${WORKSPACE_ROOT}/lites_server"
        "${WORKSPACE_ROOT}/build/lites_server"
    )

    for candidate in "${candidates[@]}"; do
        if [[ -f "$candidate" ]]; then
            echo "$candidate"
            return 0
        fi
    done

    echo "Error: Kernel binary not found" >&2
    echo "Searched locations:" >&2
    printf "  %s\n" "${candidates[@]}" >&2
    echo "" >&2
    echo "Build the kernel first using: docker/scripts/build-lites-i386.sh" >&2
    exit 1
}

##
# @brief Find optional emulator/initrd binary
#
# @return Prints the emulator path if found, otherwise empty
##
find_emulator() {
    local candidates=(
        "${BUILD_DIR}/lites_emulator"
        "${WORKSPACE_ROOT}/lites_emulator"
        "${WORKSPACE_ROOT}/build/lites_emulator"
    )
    
    for candidate in "${candidates[@]}"; do
        if [[ -f "$candidate" ]]; then
            echo "$candidate"
            return 0
        fi
    done
    
    return 1
}

##
# @brief Detect disk image format
##
detect_image_format() {
    local image="$1"

    # Check file extension first
    case "$image" in
        *.qcow2)
            echo "qcow2"
            return 0
            ;;
        *.img)
            echo "raw"
            return 0
            ;;
    esac

    # Try to detect with qemu-img if available
    if command -v qemu-img >/dev/null 2>&1; then
        local format
        format=$(qemu-img info "$image" 2>/dev/null | grep "file format:" | awk '{print $3}')
        if [[ -n "$format" ]]; then
            echo "$format"
            return 0
        fi
    fi

    # Default to raw
    echo "raw"
}

##
# @brief Build QEMU command line for disk boot
##
build_qemu_cmd_disk() {
    local disk_image="$1"
    local use_kvm="$2"
    local use_vnc="$3"
    local use_monitor="$4"
    local gdb_port="$5"
    local debug="$6"
    local memory="$7"
    local cpus="$8"

    local qemu_cmd="qemu-system-i386"
    local qemu_args=()
    local image_format

    image_format=$(detect_image_format "$disk_image")

    # Basic configuration
    qemu_args+=(-m "$memory")
    qemu_args+=(-smp "$cpus")

    # Boot from disk image
    qemu_args+=(-drive "file=$disk_image,format=$image_format,if=ide")

    # KVM acceleration
    if [[ "$use_kvm" == "yes" ]]; then
        qemu_args+=(-enable-kvm)
        qemu_args+=(-cpu host)
    else
        qemu_args+=(-cpu qemu32)
    fi

    # Display configuration
    if [[ "$use_vnc" == "yes" ]]; then
        qemu_args+=(-vnc :0)
        echo "VNC display available on :5900" >&2
    else
        qemu_args+=(-nographic)
    fi

    # Serial console
    qemu_args+=(-serial stdio)

    # Monitor
    if [[ "$use_monitor" == "yes" ]]; then
        # shellcheck disable=SC2054
        qemu_args+=(-monitor telnet::4444,server,nowait)
        echo "QEMU monitor available on telnet://localhost:4444" >&2
    fi

    # GDB server
    if [[ -n "$gdb_port" ]]; then
        qemu_args+=(-gdb "tcp::$gdb_port")
        qemu_args+=(-S)
        echo "GDB server listening on port $gdb_port" >&2
        echo "Connect with: gdb -ex 'target remote localhost:$gdb_port'" >&2
    fi

    # Debug output
    if [[ "$debug" == "yes" ]]; then
        qemu_args+=(-d int,cpu_reset)
        qemu_args+=(-D qemu-debug.log)
        echo "Debug output will be written to qemu-debug.log" >&2
    fi

    # No reboot on error
    qemu_args+=(-no-reboot)

    echo "$qemu_cmd ${qemu_args[*]}"
}

##
# @brief Build QEMU command line for kernel boot
##
build_qemu_cmd_kernel() {
    local kernel="$1"
    local use_kvm="$2"
    local use_vnc="$3"
    local use_monitor="$4"
    local gdb_port="$5"
    local debug="$6"
    local memory="$7"
    local cpus="$8"

    local qemu_cmd="qemu-system-i386"
    local qemu_args=()

    # Basic configuration
    qemu_args+=(-m "$memory")
    qemu_args+=(-smp "$cpus")
    qemu_args+=(-kernel "$kernel")

    # KVM acceleration
    if [[ "$use_kvm" == "yes" ]]; then
        qemu_args+=(-enable-kvm)
        qemu_args+=(-cpu host)
    else
        qemu_args+=(-cpu qemu32)
    fi

    # Optional emulator/initrd
    local emulator
    if emulator=$(find_emulator); then
        echo "Using emulator/initrd: $emulator" >&2
        qemu_args+=(-initrd "$emulator")
    fi
    
    # Display configuration
    if [[ "$use_vnc" == "yes" ]]; then
        qemu_args+=(-vnc :0)
        echo "VNC display available on :5900" >&2
    else
        qemu_args+=(-nographic)
    fi
    
    # Serial console
    qemu_args+=(-serial stdio)
    
    # Monitor
    if [[ "$use_monitor" == "yes" ]]; then
        # shellcheck disable=SC2054
        qemu_args+=(-monitor telnet::4444,server,nowait)
        echo "QEMU monitor available on telnet://localhost:4444" >&2
    fi
    
    # GDB server
    if [[ -n "$gdb_port" ]]; then
        qemu_args+=(-gdb "tcp::$gdb_port")
        qemu_args+=(-S)
        echo "GDB server listening on port $gdb_port" >&2
        echo "Connect with: gdb -ex 'target remote localhost:$gdb_port'" >&2
    fi
    
    # Debug output
    if [[ "$debug" == "yes" ]]; then
        # shellcheck disable=SC2054
        qemu_args+=(-d guest_errors,unimp)
    fi
    
    # Additional recommended options
    qemu_args+=(-no-reboot)
    qemu_args+=(-append "console=ttyS0")
    
    echo "$qemu_cmd ${qemu_args[*]}"
}

##
# @brief Main entry point
##
main() {
    local kernel_path=""
    local image_path=""
    local use_kvm="auto"
    local use_vnc="no"
    local use_monitor="no"
    local gdb_port=""
    local debug="no"
    local memory="$QEMU_MEMORY"
    local cpus="$QEMU_CPUS"

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -k|--kernel)
                kernel_path="$2"
                shift 2
                ;;
            -i|--image)
                image_path="$2"
                shift 2
                ;;
            -m|--memory)
                memory="$2"
                shift 2
                ;;
            -c|--cpus)
                cpus="$2"
                shift 2
                ;;
            --kvm)
                use_kvm="yes"
                shift
                ;;
            --no-kvm)
                use_kvm="no"
                shift
                ;;
            --serial)
                # Already default
                shift
                ;;
            --vnc)
                use_vnc="yes"
                shift
                ;;
            --monitor)
                use_monitor="yes"
                shift
                ;;
            --gdb)
                gdb_port="$2"
                shift 2
                ;;
            -d|--debug)
                debug="yes"
                shift
                ;;
            *)
                echo "Unknown option: $1" >&2
                usage
                exit 1
                ;;
        esac
    done
    
    # Determine KVM usage
    if [[ "$use_kvm" == "auto" ]]; then
        if check_kvm; then
            use_kvm="yes"
            echo "KVM acceleration: enabled" >&2
        else
            use_kvm="no"
            echo "KVM acceleration: not available (using pure emulation)" >&2
        fi
    elif [[ "$use_kvm" == "yes" ]] && ! check_kvm; then
        echo "Error: KVM requested but /dev/kvm is not accessible" >&2
        echo "Ensure the Docker container is run with --device=/dev/kvm or --privileged" >&2
        exit 1
    fi

    # Determine boot mode: disk image or direct kernel
    local boot_mode
    local qemu_cmd

    if [[ -n "$image_path" ]]; then
        # Boot from disk image (preferred)
        local disk_image
        disk_image=$(find_disk_image "$image_path")
        if [[ -z "$disk_image" ]]; then
            echo "Error: Disk image not found: $image_path" >&2
            exit 1
        fi
        echo "Boot mode: disk image" >&2
        echo "Using disk image: $disk_image" >&2
        qemu_cmd=$(build_qemu_cmd_disk "$disk_image" "$use_kvm" "$use_vnc" "$use_monitor" "$gdb_port" "$debug" "$memory" "$cpus")
    elif [[ -n "$kernel_path" ]]; then
        # Direct kernel boot
        local kernel
        kernel=$(find_kernel "$kernel_path")
        echo "Boot mode: direct kernel" >&2
        echo "Using kernel: $kernel" >&2
        qemu_cmd=$(build_qemu_cmd_kernel "$kernel" "$use_kvm" "$use_vnc" "$use_monitor" "$gdb_port" "$debug" "$memory" "$cpus")
    else
        # Auto-detect: try disk image first, then kernel
        local disk_image
        if disk_image=$(find_disk_image ""); then
            echo "Boot mode: disk image (auto-detected)" >&2
            echo "Using disk image: $disk_image" >&2
            qemu_cmd=$(build_qemu_cmd_disk "$disk_image" "$use_kvm" "$use_vnc" "$use_monitor" "$gdb_port" "$debug" "$memory" "$cpus")
        else
            # Fallback to kernel boot
            local kernel
            kernel=$(find_kernel "$kernel_path")
            echo "Boot mode: direct kernel (no disk image found)" >&2
            echo "Using kernel: $kernel" >&2
            qemu_cmd=$(build_qemu_cmd_kernel "$kernel" "$use_kvm" "$use_vnc" "$use_monitor" "$gdb_port" "$debug" "$memory" "$cpus")
        fi
    fi

    echo "" >&2
    echo "Starting QEMU i386 instance..." >&2
    echo "Command: $qemu_cmd" >&2
    echo "" >&2

    # Execute QEMU
    # shellcheck disable=SC2086
    exec $qemu_cmd
}

main "$@"
