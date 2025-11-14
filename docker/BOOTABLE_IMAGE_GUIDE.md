# Bootable Disk Image Creation Guide

This guide explains how to create bootable disk images from the Lites build, convert them to QCOW2 format, and run them in QEMU.

## Overview

The complete workflow transforms the Lites binary into a bootable disk image:

```
┌─────────────────┐     ┌──────────────────┐     ┌─────────────────┐
│  Build Lites    │────▶│  Create .img     │────▶│  Convert to     │
│  i386 Binary    │     │  Bootable Image  │     │  QCOW2 Format   │
└─────────────────┘     └──────────────────┘     └─────────────────┘
                                                           │
                                                           ▼
                                                  ┌─────────────────┐
                                                  │  Boot in QEMU   │
                                                  └─────────────────┘
```

## What Gets Created

The workflow produces three main artifacts:

1. **`lites_emulator`** - The compiled Lites server binary (ELF executable)
2. **`lites-boot.img`** - Bootable raw disk image containing:
   - MBR partition table
   - GRUB bootloader
   - GNU Mach kernel
   - Lites server binary
   - ext2 root filesystem
3. **`lites.qcow2`** - Dynamic QCOW2 image (converted from .img)
   - Compressed and optimized for QEMU
   - Supports snapshots and dynamic growth

## Quick Start

### Complete Automated Workflow

The easiest way to build everything:

```bash
# From repository root
make docker-all
```

This runs the complete workflow:
1. Builds the Docker development image
2. Compiles Lites for i386
3. Creates bootable .img disk image
4. Converts to QCOW2 format
5. Runs in QEMU

### Step-by-Step Workflow

If you want to run each step individually:

```bash
# 1. Build Docker image
make docker-build

# 2. Build Lites binary
make docker-build-lites

# 3. Create bootable .img image
make docker-bootimg

# 4. Convert to QCOW2
make docker-qcow2

# 5. Run in QEMU
make docker-run-qemu
```

### Using Scripts Directly

You can also run the scripts directly inside the Docker container:

```bash
# Enter the development container
docker compose -f docker/docker-compose.yml run --rm --privileged lites-i386-dev bash

# Inside the container:

# 1. Build Lites
bash docker/scripts/build-lites-i386.sh

# 2. Create bootable .img (requires root/sudo for loopback devices)
bash docker/scripts/create-bootable-image.sh

# 3. Convert to QCOW2
bash docker/scripts/convert-img-to-qcow2.sh \
    -i lites-boot.img \
    -o lites.qcow2 \
    --compress \
    --prealloc metadata

# 4. Run in QEMU
bash docker/scripts/run-qemu-i386.sh
```

## Script Reference

### create-bootable-image.sh

Creates a bootable disk image with GRUB, Mach kernel, and Lites server.

**Usage:**
```bash
docker/scripts/create-bootable-image.sh [OPTIONS]
```

**Options:**
- `-s, --size SIZE` - Image size (default: 512M)
- `-o, --output FILE` - Output file (default: lites-boot.img)
- `-l, --lites PATH` - Path to Lites binary
- `-k, --kernel PATH` - Path to Mach kernel (auto-detected)
- `-h, --help` - Show help

**Examples:**
```bash
# Default 512M image
./create-bootable-image.sh

# 1GB image with custom name
./create-bootable-image.sh --size 1G --output my-lites.img

# Custom Lites binary location
./create-bootable-image.sh --lites /path/to/lites_server
```

**What it does:**
1. Creates a raw disk image file
2. Partitions it with MBR (single bootable ext2 partition)
3. Formats partition with ext2 filesystem
4. Creates directory structure (/boot, /bin, /etc, etc.)
5. Copies GNU Mach kernel to /boot/gnumach
6. Copies Lites server to /boot/lites_server
7. Creates GRUB configuration
8. Installs GRUB bootloader to MBR
9. Unmounts and closes the image

**Requirements:**
- Root/sudo access (for loopback device mounting)
- Packages: qemu-utils, parted, e2fsprogs, grub-pc-bin

### convert-img-to-qcow2.sh

Converts raw .img files to optimized QCOW2 format.

**Usage:**
```bash
docker/scripts/convert-img-to-qcow2.sh -i INPUT -o OUTPUT [OPTIONS]
```

**Options:**
- `-i, --input FILE` - Input .img file (required)
- `-o, --output FILE` - Output .qcow2 file (required)
- `-c, --compress` - Enable compression
- `-p, --prealloc MODE` - Preallocation: off, metadata, falloc, full
- `-s, --cluster SIZE` - Cluster size (default: 65536)
- `--compat VERSION` - QCOW2 version: 0.10, 1.1 (default: 1.1)
- `--no-lazy-refcounts` - Disable lazy refcounts

**Examples:**
```bash
# Basic conversion
./convert-img-to-qcow2.sh -i lites-boot.img -o lites.qcow2

# Compressed with metadata preallocation (recommended)
./convert-img-to-qcow2.sh \
    -i lites-boot.img \
    -o lites.qcow2 \
    --compress \
    --prealloc metadata

# Maximum performance (larger file, faster runtime)
./convert-img-to-qcow2.sh \
    -i lites-boot.img \
    -o lites.qcow2 \
    --prealloc full \
    --cluster 65536
```

**Preallocation Modes:**
- `off` - Fully dynamic, grows as needed (default, slowest)
- `metadata` - Preallocate metadata only (good balance, recommended)
- `falloc` - Fast preallocation using fallocate()
- `full` - Preallocate all space (fastest runtime, largest file)

**QCOW2 Optimization:**
- **Lazy refcounts**: Enabled by default, 5-10% write performance boost
- **64KB clusters**: Optimal for most workloads
- **Compression**: Reduces file size, increases CPU usage
- **compat=1.1**: Modern QEMU features (recommended)

## Boot Configuration

The bootable image includes a GRUB configuration with three boot options:

### 1. Lites on GNU Mach (default)
```
multiboot /boot/gnumach root=device:hd0s1
module /boot/lites_server
```
Boots Mach kernel with Lites server.

### 2. Lites on GNU Mach (single user)
```
multiboot /boot/gnumach root=device:hd0s1 -s
module /boot/lites_server
```
Same as above but boots into single-user mode.

### 3. GNU Mach (no Lites)
```
multiboot /boot/gnumach root=device:hd0s1
```
Boots only the Mach kernel without Lites.

## Disk Image Structure

### Partition Layout
```
┌─────────────────────────────────────────┐
│ MBR (Master Boot Record)                │  Sector 0
│ - Partition table                       │
│ - GRUB Stage 1 bootloader              │
├─────────────────────────────────────────┤
│ Gap (GRUB core.img)                     │  Sectors 1-2047
├─────────────────────────────────────────┤
│ Partition 1: ext2 filesystem            │  From 1 MiB
│                                          │
│  /boot/                                 │
│    ├── gnumach        (Mach kernel)     │
│    ├── lites_server   (Lites binary)    │
│    └── grub/                            │
│         ├── grub.cfg                    │
│         └── [GRUB modules]              │
│                                          │
│  /bin/, /sbin/, /etc/, /dev/...         │
│  (empty directory structure)            │
└─────────────────────────────────────────┘
```

### Filesystem Details
- **Type**: ext2 (compatible with Mach/Lites)
- **Label**: LITES-ROOT
- **Bootable flag**: Set on partition 1
- **Total size**: Configurable (default 512M)

## Running the Bootable Image

### Direct QEMU Execution

**Raw .img format:**
```bash
qemu-system-i386 \
    -drive file=lites-boot.img,format=raw,if=ide \
    -m 256M \
    -enable-kvm \
    -serial stdio \
    -nographic
```

**QCOW2 format:**
```bash
qemu-system-i386 \
    -drive file=lites.qcow2,format=qcow2,if=ide \
    -m 256M \
    -enable-kvm \
    -serial stdio \
    -nographic
```

### Using Provided Scripts

**Inside Docker:**
```bash
docker/scripts/run-qemu-i386.sh
```

**From host with Make:**
```bash
make docker-run-qemu
```

### Advanced QEMU Options

**With VNC display:**
```bash
qemu-system-i386 \
    -drive file=lites.qcow2,format=qcow2 \
    -m 256M \
    -vnc :0
```

**With networking:**
```bash
qemu-system-i386 \
    -drive file=lites.qcow2,format=qcow2 \
    -m 256M \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device e1000,netdev=net0
```

**With GDB debugging:**
```bash
qemu-system-i386 \
    -drive file=lites.qcow2,format=qcow2 \
    -m 256M \
    -s -S \
    -serial stdio

# In another terminal:
gdb -ex 'target remote localhost:1234'
```

## Troubleshooting

### GRUB Installation Fails

**Problem:** GRUB fails to install to the disk image.

**Solutions:**
1. Ensure you're running with `--privileged` flag for Docker
2. Verify loopback device support: `ls -la /dev/loop*`
3. Try the alternative GRUB installation method (automatic fallback)
4. Install grub-pc-bin: `apt-get install grub-pc-bin grub2-common`

### Mach Kernel Not Found

**Problem:** Script cannot find GNU Mach kernel.

**Solutions:**
1. Install gnumach-image package (Debian/Hurd)
2. Download manually from Debian repository (automatic fallback)
3. Specify kernel path: `--kernel /path/to/gnumach`
4. Place kernel in `/boot/gnumach` or `build-i386/gnumach`

### Image Won't Boot in QEMU

**Problem:** QEMU hangs or shows no output.

**Solutions:**
1. Add `-nographic -serial stdio` for serial console output
2. Check GRUB configuration: mount the image and verify `/boot/grub/grub.cfg`
3. Verify kernel and binary exist in /boot/
4. Try booting with VNC: `-vnc :0` and connect with VNC viewer
5. Enable QEMU debug: `-d int,cpu_reset -D qemu.log`

### Loopback Device Busy

**Problem:** Cannot detach loopback device.

**Solutions:**
```bash
# Force unmount
sudo umount -f /tmp/lites-mount-*

# Force detach loop device
sudo losetup -D

# Kill any processes using the mount
sudo lsof /tmp/lites-mount-* | awk 'NR>1 {print $2}' | xargs sudo kill
```

### Permission Denied Errors

**Problem:** Cannot mount or create loop devices.

**Solutions:**
1. Run with `--privileged` in Docker
2. Use sudo for script execution
3. Check Docker daemon is running with proper permissions
4. Verify user is in docker group: `groups $USER`

### Conversion Takes Too Long

**Problem:** QCOW2 conversion is very slow.

**Solutions:**
1. Remove `--compress` flag (faster but larger file)
2. Use `--prealloc off` instead of metadata
3. Increase I/O priority: `nice -n -10 ...`
4. Use faster storage (SSD instead of HDD)

## Technical Details

### GNU Mach Kernel

The bootable image uses GNU Mach, a microkernel from the GNU Project:
- **Version**: 1.8+ (from Debian Hurd repositories)
- **Format**: ELF executable (may be gzipped)
- **Source**: http://ftp.debian.org/debian/pool/main/g/gnumach/
- **Compatibility**: i386/i486/i686 architecture

### GRUB Multiboot

Lites boots using GRUB's multiboot specification:
- **GRUB version**: 2.x (grub-pc)
- **Target**: i386-pc
- **Modules**: ext2, part_msdos, multiboot, biosdisk

### QCOW2 Format

QCOW2 (QEMU Copy-On-Write version 2) features:
- **Sparse files**: Only allocated blocks use disk space
- **Snapshots**: Create point-in-time snapshots
- **Compression**: Optional zlib compression
- **Encryption**: Optional AES encryption (not used here)
- **Backing files**: Can use differential images

**Default settings in this workflow:**
```
compat=1.1              # Modern QEMU features
lazy_refcounts=on       # 5-10% write speedup
cluster_size=65536      # 64KB clusters (optimal)
preallocation=metadata  # Fast with reasonable size
```

## CI/CD Integration

### GitHub Actions

The workflow can be integrated into CI/CD:

```yaml
jobs:
  build-bootable-image:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Build Docker image
        run: make docker-build

      - name: Build Lites
        run: make docker-build-lites

      - name: Create bootable disk images
        run: make docker-disk

      - name: Upload artifacts
        uses: actions/upload-artifact@v3
        with:
          name: lites-disk-images
          path: |
            lites-boot.img
            lites.qcow2
```

### Automated Testing

Test the bootable image automatically:

```bash
# Start QEMU in background with timeout
timeout 30 qemu-system-i386 \
    -drive file=lites.qcow2,format=qcow2 \
    -m 256M \
    -nographic \
    -serial file:boot.log

# Check boot log for success
grep "Lites" boot.log && echo "Boot successful"
```

## Performance Tuning

### QCOW2 Optimization

For best performance:
```bash
# Metadata preallocation + larger clusters
./convert-img-to-qcow2.sh \
    -i lites-boot.img \
    -o lites.qcow2 \
    --prealloc metadata \
    --cluster 131072

# Disable compression for speed
# (compression adds ~20% conversion time)
```

### QEMU Optimization

```bash
# Enable KVM acceleration (10-50x speedup)
qemu-system-i386 -enable-kvm ...

# Use virtio drivers (faster I/O)
qemu-system-i386 \
    -drive file=lites.qcow2,if=virtio ...

# Increase cache (better performance)
qemu-system-i386 \
    -drive file=lites.qcow2,cache=writeback ...
```

### Build Optimization

```bash
# Parallel builds
export MAKEFLAGS="-j$(nproc)"

# Use ccache (enabled by default in Docker)
export CC="ccache gcc"

# Link-time optimization
export CFLAGS="-O3 -flto"
```

## See Also

- [Docker README](README.md) - Docker development environment
- [QEMU Disk Guide](QEMU_DISK_GUIDE.md) - QEMU disk management
- [CI/CD Workflow](CI_CD_WORKFLOW.md) - Continuous integration setup
- [Makefile.docker](../Makefile.docker) - Make targets reference

## References

- [GNU Mach Manual](https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html)
- [GRUB Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [QEMU Documentation](https://www.qemu.org/docs/master/)
- [QCOW2 Image Format](https://github.com/qemu/qemu/blob/master/docs/interop/qcow2.txt)
