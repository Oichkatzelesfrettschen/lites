#!/usr/bin/env bash
set -euo pipefail

##
# @file create-bootable-image.sh
# @brief Create a bootable disk image with Mach kernel and Lites server
#
# This script creates a complete bootable disk image containing:
# - GRUB bootloader
# - GNU Mach kernel
# - Lites server binary
# - Basic root filesystem structure
#
# The output can be used directly in QEMU or converted to QCOW2 format.
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="${WORKSPACE_ROOT:-/workspace}"
BUILD_DIR="${BUILD_DIR:-${WORKSPACE_ROOT}/build-i386}"

# Default configuration
IMAGE_SIZE="${IMAGE_SIZE:-512M}"
IMAGE_FILE="${IMAGE_FILE:-lites-boot.img}"
MOUNT_POINT="/tmp/lites-mount-$$"
LITES_BINARY="${BUILD_DIR}/lites_emulator"
MACH_KERNEL="${MACH_KERNEL:-/boot/gnumach.gz}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

##
# @brief Print colored message
##
log_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*" >&2
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

##
# @brief Display usage information
##
usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Create a bootable disk image with GNU Mach and Lites.

OPTIONS:
    -s, --size SIZE         Image size (default: 512M)
    -o, --output FILE       Output image file (default: lites-boot.img)
    -l, --lites PATH        Path to Lites binary (default: ${BUILD_DIR}/lites_emulator)
    -k, --kernel PATH       Path to Mach kernel (default: auto-detect)
    -m, --mount POINT       Mount point (default: /tmp/lites-mount-$$)
    -h, --help              Show this help message

ENVIRONMENT VARIABLES:
    WORKSPACE_ROOT          Repository root (default: /workspace)
    BUILD_DIR               Build output directory (default: /workspace/build-i386)
    IMAGE_SIZE              Image size (default: 512M)
    MACH_KERNEL             Path to Mach kernel

EXAMPLES:
    # Create default 512M image
    $0

    # Create 1GB image with custom output
    $0 --size 1G --output my-lites.img

    # Specify custom Lites binary
    $0 --lites /path/to/custom/lites_server

NOTES:
    - Requires root/sudo for mounting and GRUB installation
    - The script will use loopback devices for disk operations
    - Output image can be converted to QCOW2 with qemu-img convert
EOF
}

##
# @brief Check for required tools
##
check_dependencies() {
    local missing_deps=()

    for tool in qemu-img parted mkfs.ext2 grub-install; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_deps+=("$tool")
        fi
    done

    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        log_error "Install with: apt-get install qemu-utils parted e2fsprogs grub-pc-bin grub2-common"
        exit 1
    fi

    # Check for root privileges (needed for mount/losetup)
    if [[ $EUID -ne 0 ]] && ! command -v sudo >/dev/null 2>&1; then
        log_error "This script requires root privileges or sudo"
        exit 1
    fi
}

##
# @brief Find GNU Mach kernel
##
find_mach_kernel() {
    local kernel_paths=(
        "/boot/gnumach.gz"
        "/boot/gnumach"
        "/usr/lib/gnumach/gnumach.gz"
        "/usr/lib/gnumach/gnumach"
        "${WORKSPACE_ROOT}/gnumach"
        "${BUILD_DIR}/gnumach"
    )

    for kpath in "${kernel_paths[@]}"; do
        if [[ -f "$kpath" ]]; then
            MACH_KERNEL="$kpath"
            log_info "Found Mach kernel: $MACH_KERNEL"
            return 0
        fi
    done

    log_warn "Mach kernel not found in standard locations"
    log_warn "You may need to install gnumach-image or specify with --kernel"
    log_warn "Attempting to download GNU Mach from Debian repository..."

    # Try to download GNU Mach kernel
    if download_mach_kernel; then
        return 0
    fi

    return 1
}

##
# @brief Download GNU Mach kernel from Debian repository
##
download_mach_kernel() {
    local temp_dir
    temp_dir="$(mktemp -d)"
    local mach_url="http://ftp.debian.org/debian/pool/main/g/gnumach/gnumach-image-1-486_2%3a1.8+git20221224-2_hurd-i386.deb"

    log_info "Downloading GNU Mach kernel package..."

    if command -v wget >/dev/null 2>&1; then
        wget -q -O "${temp_dir}/gnumach.deb" "$mach_url" || return 1
    elif command -v curl >/dev/null 2>&1; then
        curl -sL -o "${temp_dir}/gnumach.deb" "$mach_url" || return 1
    else
        log_error "Neither wget nor curl available for downloading"
        return 1
    fi

    log_info "Extracting GNU Mach kernel..."
    (cd "$temp_dir" && ar x gnumach.deb && tar xf data.tar.* 2>/dev/null)

    if [[ -f "${temp_dir}/boot/gnumach.gz" ]]; then
        MACH_KERNEL="${temp_dir}/boot/gnumach.gz"
        log_info "Successfully downloaded Mach kernel to: $MACH_KERNEL"
        return 0
    fi

    rm -rf "$temp_dir"
    return 1
}

##
# @brief Create and partition disk image
##
create_disk_image() {
    local size="$1"
    local output="$2"

    log_info "Creating raw disk image: $output ($size)"

    # Create raw disk image
    qemu-img create -f raw "$output" "$size"

    log_info "Partitioning disk image..."

    # Create MBR partition table with a single bootable ext2 partition
    parted -s "$output" mklabel msdos
    parted -s "$output" mkpart primary ext2 1MiB 100%
    parted -s "$output" set 1 boot on

    log_info "Disk image created and partitioned"
}

##
# @brief Format partition and create filesystem
##
format_partition() {
    local image_file="$1"
    local loop_device

    log_info "Setting up loopback device..."

    # Set up loopback device for the image
    loop_device=$(sudo losetup --show -f -P "$image_file")

    # Give the system a moment to create partition devices
    sleep 1

    log_info "Formatting partition with ext2..."

    # Format the first partition as ext2
    sudo mkfs.ext2 -F -L "LITES-ROOT" "${loop_device}p1"

    echo "$loop_device"
}

##
# @brief Mount filesystem and populate it
##
populate_filesystem() {
    local loop_device="$1"
    local mount_point="$2"
    local lites_binary="$3"
    local mach_kernel="$4"

    log_info "Creating mount point: $mount_point"
    sudo mkdir -p "$mount_point"

    log_info "Mounting filesystem..."
    sudo mount "${loop_device}p1" "$mount_point"

    log_info "Creating directory structure..."
    sudo mkdir -p "$mount_point"/{boot,bin,sbin,etc,dev,proc,tmp,var,home,root}
    sudo mkdir -p "$mount_point/boot/grub"

    log_info "Copying Mach kernel..."
    if [[ "$mach_kernel" == *.gz ]]; then
        # Decompress if gzipped
        sudo gunzip -c "$mach_kernel" > "${mount_point}/boot/gnumach"
    else
        sudo cp "$mach_kernel" "${mount_point}/boot/gnumach"
    fi

    log_info "Copying Lites server..."
    if [[ -f "$lites_binary" ]]; then
        sudo cp "$lites_binary" "${mount_point}/boot/lites_server"
        sudo chmod +x "${mount_point}/boot/lites_server"
    else
        log_warn "Lites binary not found: $lites_binary"
        log_warn "Image will be created without Lites server"
    fi

    log_info "Creating GRUB configuration..."
    create_grub_config "$mount_point"

    log_info "Installing GRUB bootloader..."
    install_grub "$loop_device" "$mount_point"

    log_info "Syncing filesystem..."
    sudo sync

    log_info "Unmounting filesystem..."
    sudo umount "$mount_point"

    log_info "Detaching loopback device..."
    sudo losetup -d "$loop_device"

    sudo rmdir "$mount_point" || true
}

##
# @brief Create GRUB configuration
##
create_grub_config() {
    local mount_point="$1"

    # Create grub.cfg
    sudo tee "${mount_point}/boot/grub/grub.cfg" > /dev/null <<'EOF'
set timeout=5
set default=0

menuentry "Lites on GNU Mach" {
    insmod ext2
    set root=(hd0,msdos1)
    multiboot /boot/gnumach root=device:hd0s1
    module /boot/lites_server
    boot
}

menuentry "Lites on GNU Mach (single user)" {
    insmod ext2
    set root=(hd0,msdos1)
    multiboot /boot/gnumach root=device:hd0s1 -s
    module /boot/lites_server
    boot
}

menuentry "GNU Mach (no Lites)" {
    insmod ext2
    set root=(hd0,msdos1)
    multiboot /boot/gnumach root=device:hd0s1
    boot
}
EOF

    log_info "GRUB configuration created"
}

##
# @brief Install GRUB bootloader
##
install_grub() {
    local loop_device="$1"
    local mount_point="$2"

    # Install GRUB to the disk image
    # Note: grub-install may need different options depending on system
    if sudo grub-install \
        --target=i386-pc \
        --boot-directory="${mount_point}/boot" \
        --modules="ext2 part_msdos multiboot biosdisk" \
        "$loop_device" 2>/dev/null; then
        log_info "GRUB installed successfully"
    else
        log_warn "GRUB installation may have issues"
        log_warn "The image may not be bootable"

        # Try alternative installation method
        log_info "Attempting alternative GRUB installation..."
        sudo grub-mkstandalone \
            --format=i386-pc \
            --output="${mount_point}/boot/grub/core.img" \
            --install-modules="ext2 part_msdos multiboot biosdisk" \
            --modules="ext2 part_msdos multiboot biosdisk" \
            /boot/grub/grub.cfg="${mount_point}/boot/grub/grub.cfg" || true
    fi
}

##
# @brief Main entry point
##
main() {
    local clean=false

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -s|--size)
                IMAGE_SIZE="$2"
                shift 2
                ;;
            -o|--output)
                IMAGE_FILE="$2"
                shift 2
                ;;
            -l|--lites)
                LITES_BINARY="$2"
                shift 2
                ;;
            -k|--kernel)
                MACH_KERNEL="$2"
                shift 2
                ;;
            -m|--mount)
                MOUNT_POINT="$2"
                shift 2
                ;;
            *)
                log_error "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done

    log_info "=== Lites Bootable Image Creator ==="
    log_info "Configuration:"
    log_info "  Image size: $IMAGE_SIZE"
    log_info "  Output file: $IMAGE_FILE"
    log_info "  Lites binary: $LITES_BINARY"
    log_info "  Mount point: $MOUNT_POINT"

    # Check dependencies
    check_dependencies

    # Find Mach kernel
    if ! find_mach_kernel; then
        log_error "Cannot proceed without Mach kernel"
        log_error "Please install gnumach-image or specify --kernel"
        exit 1
    fi

    # Remove existing image if present
    if [[ -f "$IMAGE_FILE" ]]; then
        log_warn "Removing existing image: $IMAGE_FILE"
        rm -f "$IMAGE_FILE"
    fi

    # Create and partition disk image
    create_disk_image "$IMAGE_SIZE" "$IMAGE_FILE"

    # Format partition
    loop_device=$(format_partition "$IMAGE_FILE")

    # Populate filesystem
    populate_filesystem "$loop_device" "$MOUNT_POINT" "$LITES_BINARY" "$MACH_KERNEL"

    log_info ""
    log_info "=== Bootable image created successfully ==="
    log_info "Output: $IMAGE_FILE"
    log_info ""
    log_info "To test in QEMU:"
    log_info "  qemu-system-i386 -drive file=$IMAGE_FILE,format=raw -m 256M"
    log_info ""
    log_info "To convert to QCOW2:"
    log_info "  qemu-img convert -f raw -O qcow2 $IMAGE_FILE lites.qcow2"
    log_info ""
}

main "$@"
