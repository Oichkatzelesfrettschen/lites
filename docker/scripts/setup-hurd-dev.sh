#!/usr/bin/env bash
set -euo pipefail

##
# @file setup-hurd-dev.sh
# @brief Set up GNU/Hurd development environment for Lites
#
# This script helps set up a GNU/Hurd environment that can be used
# to build Lites with native Hurd tooling. It downloads and configures
# a Debian GNU/Hurd QEMU image that can be used for development.
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
HURD_DIR="${PROJECT_ROOT}/hurd-env"
HURD_IMAGE_URL="https://cdimage.debian.org/cdimage/ports/latest/hurd-i386/debian-hurd.img.tar.gz"

##
# @brief Print usage information
##
usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Set up GNU/Hurd development environment for Lites.

This script downloads and configures a Debian GNU/Hurd QEMU image
that can be used as an alternative development environment with
native Mach/Hurd tooling including MIG.

OPTIONS:
    -h, --help          Show this help message
    -d, --dir DIR       Installation directory (default: hurd-env)
    -f, --force         Force re-download even if image exists
    --skip-download     Skip download, use existing image
    --cleanup           Remove Hurd environment

DESCRIPTION:
    This creates a Debian GNU/Hurd QEMU image with:
    - Native GNU Mach kernel
    - Complete Hurd servers
    - MIG (Mach Interface Generator)
    - Development tools (gcc, make, etc.)

EXAMPLES:
    # Download and set up Hurd environment
    $0

    # Use custom directory
    $0 --dir /path/to/hurd

    # Clean up existing environment
    $0 --cleanup

NOTE:
    The Hurd image is several hundred MB. This is an alternative
    to the standard Docker environment for those who want to work
    with native Hurd tooling.
EOF
}

##
# @brief Download Debian GNU/Hurd image
##
download_hurd_image() {
    local dest_dir="$1"
    local force="$2"
    
    mkdir -p "$dest_dir"
    cd "$dest_dir"
    
    if [[ -f "debian-hurd.img" ]] && [[ "$force" != "yes" ]]; then
        echo "Hurd image already exists. Use --force to re-download."
        return 0
    fi
    
    echo "Downloading Debian GNU/Hurd QEMU image..."
    echo "URL: $HURD_IMAGE_URL"
    echo "This may take several minutes..."
    
    if ! wget -c "$HURD_IMAGE_URL" -O debian-hurd.img.tar.gz; then
        echo "Error: Failed to download Hurd image" >&2
        return 1
    fi
    
    echo "Extracting image..."
    tar -xzf debian-hurd.img.tar.gz
    
    # Rename to consistent name
    mv debian-hurd-*.img debian-hurd.img 2>/dev/null || true
    
    echo "Download and extraction complete!"
}

##
# @brief Create launch script for Hurd environment
##
create_launch_script() {
    local dest_dir="$1"
    
    cat > "$dest_dir/run-hurd.sh" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

##
# @brief Launch Debian GNU/Hurd in QEMU
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE="${SCRIPT_DIR}/debian-hurd.img"
MEMORY="${MEMORY:-1G}"
CPUS="${CPUS:-2}"

if [[ ! -f "$IMAGE" ]]; then
    echo "Error: Hurd image not found: $IMAGE" >&2
    echo "Run setup-hurd-dev.sh first" >&2
    exit 1
fi

# Check for KVM
KVM_ARGS=""
if [[ -e /dev/kvm ]] && [[ -w /dev/kvm ]]; then
    KVM_ARGS="-enable-kvm"
    echo "KVM acceleration: enabled"
else
    echo "KVM acceleration: not available"
fi

echo "Starting Debian GNU/Hurd..."
echo "Memory: $MEMORY"
echo "CPUs: $CPUS"
echo ""
echo "Default login: root (no password)"
echo ""

exec qemu-system-i386 \
    $KVM_ARGS \
    -m "$MEMORY" \
    -smp "$CPUS" \
    -drive file="$IMAGE",cache=writeback \
    -net nic,model=e1000 \
    -net user,hostfwd=tcp::2222-:22 \
    -nographic \
    -no-reboot
EOF
    
    chmod +x "$dest_dir/run-hurd.sh"
    echo "Created launch script: $dest_dir/run-hurd.sh"
}

##
# @brief Create documentation for Hurd environment
##
create_hurd_docs() {
    local dest_dir="$1"
    
    cat > "$dest_dir/README.md" <<'EOF'
# Debian GNU/Hurd Development Environment

This directory contains a Debian GNU/Hurd QEMU image for Lites development.

## Usage

### Start the Hurd environment

```bash
./run-hurd.sh
```

Default credentials:
- Username: `root`
- Password: (none, just press Enter)

### SSH Access

SSH is forwarded to localhost:2222:

```bash
ssh -p 2222 root@localhost
```

### Building Lites in Hurd

Once logged into the Hurd system:

```bash
# Install additional packages if needed
apt-get update
apt-get install git build-essential mig

# Clone Lites (or mount from host via 9p/virtfs)
git clone https://github.com/Oichkatzelesfrettschen/lites.git
cd lites

# Build with native Hurd tooling
make -f Makefile.new ARCH=i686
```

### Mounting Host Directory

To access your host Lites repository from within Hurd, you can use
QEMU's virtfs feature. Edit `run-hurd.sh` and add:

```bash
-virtfs local,path=/path/to/lites,mount_tag=host_lites,security_model=none
```

Then in Hurd:
```bash
mkdir /mnt/host
mount -t 9p -o trans=virtio host_lites /mnt/host
```

## Resources

- [GNU Hurd Documentation](https://www.gnu.org/software/hurd/)
- [Debian GNU/Hurd](https://www.debian.org/ports/hurd/)
- [Hurd FAQ](https://www.gnu.org/software/hurd/faq.html)

## Comparison with Docker Environment

| Feature | Docker (docker/) | Hurd (hurd-env/) |
|---------|------------------|------------------|
| Setup Time | Fast (5 min) | Slow (20+ min) |
| Disk Space | ~2 GB | ~4 GB |
| MIG Version | Debian packages | Native Hurd |
| Development | Modern Linux | Pure Hurd |
| Performance | Faster | Slower |
| Use Case | Quick builds | Hurd-native dev |

For most users, the Docker environment (in `docker/`) is recommended.
Use this Hurd environment only if you specifically need native Hurd
tooling or want to develop within a true Mach/Hurd system.
EOF
    
    echo "Created documentation: $dest_dir/README.md"
}

##
# @brief Clean up Hurd environment
##
cleanup_hurd() {
    local dest_dir="$1"
    
    if [[ ! -d "$dest_dir" ]]; then
        echo "Nothing to clean up: $dest_dir does not exist"
        return 0
    fi
    
    echo "Removing Hurd environment: $dest_dir"
    rm -rf "$dest_dir"
    echo "Cleanup complete!"
}

##
# @brief Main entry point
##
main() {
    local dest_dir="$HURD_DIR"
    local force="no"
    local skip_download="no"
    local do_cleanup="no"
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -d|--dir)
                dest_dir="$2"
                shift 2
                ;;
            -f|--force)
                force="yes"
                shift
                ;;
            --skip-download)
                skip_download="yes"
                shift
                ;;
            --cleanup)
                do_cleanup="yes"
                shift
                ;;
            *)
                echo "Unknown option: $1" >&2
                usage
                exit 1
                ;;
        esac
    done
    
    # Handle cleanup
    if [[ "$do_cleanup" == "yes" ]]; then
        cleanup_hurd "$dest_dir"
        exit 0
    fi
    
    # Check prerequisites
    if ! command -v wget >/dev/null 2>&1; then
        echo "Error: wget is required but not installed" >&2
        echo "Install with: apt-get install wget" >&2
        exit 1
    fi
    
    if ! command -v qemu-system-i386 >/dev/null 2>&1; then
        echo "Warning: qemu-system-i386 not found" >&2
        echo "Install with: apt-get install qemu-system-x86" >&2
    fi
    
    echo "Setting up GNU/Hurd development environment..."
    echo "Installation directory: $dest_dir"
    echo ""
    
    # Download if needed
    if [[ "$skip_download" != "yes" ]]; then
        download_hurd_image "$dest_dir" "$force"
    fi
    
    # Create helper scripts
    create_launch_script "$dest_dir"
    create_hurd_docs "$dest_dir"
    
    echo ""
    echo "======================================"
    echo "GNU/Hurd environment ready!"
    echo "======================================"
    echo ""
    echo "To start the Hurd system:"
    echo "  cd $dest_dir"
    echo "  ./run-hurd.sh"
    echo ""
    echo "See $dest_dir/README.md for more information."
}

main "$@"
