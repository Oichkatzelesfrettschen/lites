# Comprehensive Docker Development Guide for Lites i386

WHY: Complete reference for building, developing, and testing Lites in Docker.

WHAT: Detailed guide covering Docker image creation, build processes, QEMU
      configuration, disk images, and Mach headers integration.

HOW: Step-by-step instructions with examples and best practices.

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Docker Environment](#docker-environment)
3. [Building Lites](#building-lites)
4. [QEMU Configuration](#qemu-configuration)
5. [Disk Images](#disk-images)
6. [Mach Headers](#mach-headers)
7. [Advanced Topics](#advanced-topics)

---

## Getting Started

### Prerequisites

**Required:**
- Docker Engine 20.10 or later
- Docker Compose v2 (included with Docker Desktop)
- 4+ GB RAM, 5+ GB free disk space
- Git for cloning the repository

**Optional:**
- KVM support (Linux only) for 10-50x faster QEMU
- VNC client for graphical display

### Initial Setup

```bash
# 1. Clone repository
git clone https://github.com/Oichkatzelesfrettschen/lites.git
cd lites

# 2. Verify Docker
docker --version                # Should be 20.10+
docker compose version          # Should be v2.x+

# 3. Check disk space
df -h                          # Need 5+ GB free

# 4. Check KVM (Linux only, optional)
ls -l /dev/kvm                 # If exists, KVM available
```

### First Build

```bash
# Complete workflow (one command)
make -f Makefile.docker docker-all

# This executes:
# 1. Build Docker image (5-10 min first time)
# 2. Build Lites for i386 (2-10 min)
# 3. Validate build artifacts
```

Expected output:
```
Building Docker image...
[+] Building 180.5s (12/12) FINISHED
Successfully tagged lites-i386-dev:latest

Building Lites i386...
[100%] Built target lites_server
Build completed successfully!

Binaries: /workspace/build-i386/lites_server
```

---

## Docker Environment

### Docker Image Contents

The `lites-i386-dev` image includes:

**Base**: Debian Bookworm (stable)

**Build Tools:**
- GCC 12.x with multilib (i386 cross-compilation)
- Clang/LLVM
- CMake 3.25+, Ninja, Make
- Autotools, pkg-config

**Mach/Hurd Tools:**
- MIG (Mach Interface Generator) 1.8+
- GNU Mach headers (from osfmk6.1)
- Hurd development headers

**Cross-compilation:**
- i686-linux-gnu toolchain
- Binutils for i386

**Emulation:**
- QEMU system-i386 with KVM support

**Development:**
- GDB with multiarch support
- tmux for session management
- ccache for faster rebuilds
- vim, git, less, file

### Building the Docker Image

```bash
# Using Make (recommended)
make -f Makefile.docker docker-build

# Using Docker Compose directly
docker compose -f docker/docker-compose.yml build lites-i386-dev

# Force rebuild (no cache)
docker compose -f docker/docker-compose.yml build --no-cache lites-i386-dev
```

Build time: 5-10 minutes first time, cached thereafter.

### Container Environment

**Working directory**: `/workspace` (repository root mounted)

**User**: `developer` (non-root, UID may differ from host)

**Pre-configured variables**:
```bash
ARCH=i686
CC="ccache gcc"
CXX="ccache g++"
CFLAGS="-m32 -march=i386 -mtune=i386"
CXXFLAGS="-m32 -march=i386 -mtune=i386"
LDFLAGS="-m32 -Wl,-z,noexecstack"
LITES_MACH_DIR=/opt/mach/osfmk/kernel/src
```

### Entering the Container

```bash
# Using convenience script (recommended)
docker/scripts/docker-shell.sh

# Using Make
make -f Makefile.docker docker-shell

# Using Docker Compose
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash

# With KVM device access
docker compose -f docker/docker-compose.yml run --rm \
  --device /dev/kvm:/dev/kvm lites-i386-dev bash
```

### Persistent Volumes

Docker Compose creates these volumes:

- `ccache-i386`: Compiler cache (faster rebuilds)
- `build-i386`: Build artifacts
- `qemu-images`: QEMU disk images

**Manage volumes**:
```bash
# List volumes
docker volume ls | grep lites

# Inspect volume
docker volume inspect lites_ccache-i386

# Remove volumes (clean slate)
docker compose -f docker/docker-compose.yml down -v
```

---

## Building Lites

### Build Script Options

```bash
# Basic build (default: CMake with Ninja)
docker/scripts/build-lites-i386.sh

# Clean build (remove previous artifacts)
docker/scripts/build-lites-i386.sh --clean

# Parallel build (8 jobs)
docker/scripts/build-lites-i386.sh -j 8

# Use Makefile instead of CMake
docker/scripts/build-lites-i386.sh --make

# Combination
docker/scripts/build-lites-i386.sh --clean -j $(nproc)
```

### Build Process

The build script executes these steps:

1. **Validate environment**
   - Check for gcc, make, cmake, mig
   - Verify Mach headers location

2. **Configure build**
   ```bash
   cmake -B build-i386 \
       -G Ninja \
       -DCMAKE_BUILD_TYPE=Release \
       -DARCH=i686 \
       -DCMAKE_C_COMPILER=gcc \
       -DLITES_MACH_DIR=/opt/mach/osfmk/kernel/src
   ```

3. **Process MIG definitions**
   - MIG processes `*.defs` files
   - Generates IPC stubs for Mach communication

4. **Compile with i386 flags**
   ```bash
   gcc -m32 -march=i386 -mtune=i386 \
       [source files] \
       -o build-i386/lites_server
   ```

5. **Link with Mach libraries**
   ```bash
   ld -m32 [...objects...] -Wl,-z,noexecstack -o lites_server
   ```

Build time: 2-10 minutes (2-5 min with ccache)

### Build Outputs

```
build-i386/
├── lites_server         # Main i386 kernel binary (ELF 32-bit)
├── lites_emulator       # Userspace emulation testbed
├── CMakeFiles/          # CMake metadata
└── [object files]
```

### Verify Build

```bash
# Check binary exists
ls -lh build-i386/lites_server

# Verify architecture
file build-i386/lites_server
# Should show: ELF 32-bit LSB executable, Intel 80386

# Check dependencies
ldd build-i386/lites_server
```

---

## QEMU Configuration

### Basic QEMU Usage

```bash
# Using script (recommended)
docker/scripts/run-qemu-i386.sh

# Direct QEMU command
qemu-system-i386 \
    -m 256M \
    -smp 1 \
    -kernel build-i386/lites_server \
    -nographic \
    -serial stdio
```

### Script Options

```bash
# Memory configuration
docker/scripts/run-qemu-i386.sh -m 512M

# CPU count
docker/scripts/run-qemu-i386.sh --smp 2

# Force KVM acceleration
docker/scripts/run-qemu-i386.sh --kvm

# Disable KVM
docker/scripts/run-qemu-i386.sh --no-kvm

# GDB debugging on port 1234
docker/scripts/run-qemu-i386.sh --gdb 1234

# VNC display on port 5900
docker/scripts/run-qemu-i386.sh --vnc

# QEMU monitor on port 4444
docker/scripts/run-qemu-i386.sh --monitor 4444

# Combination
docker/scripts/run-qemu-i386.sh --kvm -m 512M --gdb 1234 --vnc
```

### KVM Acceleration

**WHY**: KVM provides 10-50x speedup over pure emulation.

**Check availability**:
```bash
# Host kernel module
lsmod | grep kvm           # Should show kvm_intel or kvm_amd

# Device access
ls -l /dev/kvm             # Should exist with rw permissions

# CPU virtualization
egrep -c '(vmx|svm)' /proc/cpuinfo  # Should be > 0
```

**Enable in Docker**:
```bash
# Add KVM device to container
docker compose -f docker/docker-compose.yml run --rm \
  --device /dev/kvm:/dev/kvm lites-i386-dev

# Or use privileged mode (less secure)
docker compose -f docker/docker-compose.yml run --rm \
  --privileged lites-i386-dev
```

**Add user to KVM group (Linux)**:
```bash
sudo usermod -aG kvm $USER
# Log out and log back in
```

### Networking in QEMU

**User-mode networking (NAT, no root required)**:
```bash
# Port forwarding: SSH on host:2222 -> guest:22
docker/scripts/run-qemu-i386.sh \
    -netdev user,id=net0,hostfwd=tcp::2222-:22,hostfwd=tcp::8080-:80 \
    -device e1000,netdev=net0
```

Guest configuration:
```bash
# Inside QEMU guest
ifconfig eth0 10.0.2.15 netmask 255.255.255.0
route add default gw 10.0.2.2
# DNS: 10.0.2.3 (provided by QEMU)
```

**TAP networking (requires root, full network access)**:
```bash
# Setup TAP interface (run once)
sudo docker/scripts/setup-qemu-networking.sh tap

# Use TAP in QEMU
qemu-system-i386 \
    -netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
    -device e1000,netdev=net0
```

### Serial Console

**Exit QEMU**: Press `Ctrl+A`, then `X`

**Serial to file**:
```bash
qemu-system-i386 -serial file:serial.log
```

**Serial over TCP**:
```bash
qemu-system-i386 -serial tcp::4444,server,nowait

# Connect with telnet
telnet localhost 4444
```

### Debugging with GDB

**Terminal 1 (QEMU with GDB server)**:
```bash
docker/scripts/run-qemu-i386.sh --gdb 1234
```

**Terminal 2 (GDB client)**:
```bash
# Enter container
docker/scripts/docker-shell.sh

# Start GDB
gdb build-i386/lites_server
(gdb) target remote localhost:1234
(gdb) break main
(gdb) continue
(gdb) step
(gdb) backtrace
```

---

## Disk Images

### Creating Disk Images

**Quick creation**:
```bash
# Using script
docker/scripts/create-lites-disk.sh --size 2G --output lites.qcow2

# Manual with qemu-img
qemu-img create -f qcow2 -o lazy_refcounts=on,cluster_size=65536 lites.qcow2 2G
```

### Bootable Disk Images

**Complete bootable image with GRUB + Mach + Lites**:

```bash
# Automated creation (requires --privileged)
docker/scripts/create-bootable-image.sh --size 512M --output lites-boot.img

# Convert to QCOW2
docker/scripts/convert-img-to-qcow2.sh \
    -i lites-boot.img \
    -o lites.qcow2 \
    --compress \
    --prealloc metadata
```

**What gets created**:
- MBR partition table
- GRUB bootloader
- GNU Mach kernel (`/boot/gnumach`)
- Lites server (`/boot/lites_server`)
- ext2 filesystem with directory structure

### Image Formats

| Format | Pros | Cons | Use Case |
|--------|------|------|----------|
| QCOW2 | Compression, snapshots, thin provisioning | Slower | Development |
| RAW | Best performance | No compression, full space | Production |

### QCOW2 Best Practices

**Recommended options**:
```bash
qemu-img create -f qcow2 \
    -o lazy_refcounts=on \         # 5-10% write speedup
    -o cluster_size=65536 \         # 64KB optimal
    -o compat=1.1 \                 # Modern features
    -o preallocation=metadata \     # Faster writes
    lites.qcow2 2G
```

### Image Management

**Info**:
```bash
qemu-img info lites.qcow2
```

**Convert formats**:
```bash
# QCOW2 to RAW
qemu-img convert -f qcow2 -O raw lites.qcow2 lites.raw

# RAW to QCOW2 with compression
qemu-img convert -f raw -O qcow2 -c lites.raw lites.qcow2
```

**Resize**:
```bash
qemu-img resize lites.qcow2 +1G
```

**Snapshots**:
```bash
# Create
qemu-img snapshot -c snapshot1 lites.qcow2

# List
qemu-img snapshot -l lites.qcow2

# Apply
qemu-img snapshot -a snapshot1 lites.qcow2

# Delete
qemu-img snapshot -d snapshot1 lites.qcow2
```

---

## Mach Headers

### Header Sources

**Primary**: osfmk6.1 (OSF Mach 6.1)
- Repository: https://github.com/nmartin0/osfmk6.1
- License: BSD-style (unencumbered)
- Pre-installed in Docker image

**Location in container**: `/opt/mach/osfmk/kernel/src`

**Environment variable**: `LITES_MACH_DIR=/opt/mach/osfmk/kernel/src`

### Verify Headers

```bash
# Inside container
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  bash -c "ls -la /opt/mach/osfmk/kernel/src | head -20"

# Count header files
find /opt/mach/osfmk -name '*.h' | wc -l
```

### Manual Setup (if needed)

**Option 1: Use Docker image headers**:
```bash
docker/scripts/setup-mach-headers.sh --docker
```

**Option 2: Clone from GitHub**:
```bash
docker/scripts/setup-mach-headers.sh --clone
```

**Option 3: Use local installation**:
```bash
docker/scripts/setup-mach-headers.sh --local /path/to/osfmk
```

**Option 4: Manual git clone**:
```bash
git clone --depth 1 https://github.com/nmartin0/osfmk6.1.git openmach
export LITES_MACH_DIR="$(pwd)/openmach/kernel/src"
cmake -B build -DLITES_MACH_DIR="${LITES_MACH_DIR}"
```

### Build Integration

CMake searches for headers in this order:
1. `$LITES_MACH_DIR` environment variable
2. `/opt/mach/osfmk/kernel/src` (Docker image)
3. `localmach/include` (from setup script)
4. `openmach/include` (git submodule)

---

## Advanced Topics

### Custom Dockerfile

Extend the Docker image:

```dockerfile
FROM lites-i386-dev:latest

# Add custom tools
RUN apt-get update && apt-get install -y \
    your-package-here \
    another-tool

# Add custom scripts
COPY my-scripts/ /usr/local/bin/

# Set custom environment
ENV MY_VAR=value
```

Build custom image:
```bash
docker build -t my-lites-dev -f my-dockerfile .
```

### Development Workflows

**Interactive development loop**:
```bash
# Keep container running
make -f Makefile.docker docker-shell

# Inside container
while true; do
  clear
  docker/scripts/build-lites-i386.sh
  echo "Build complete. Starting QEMU..."
  docker/scripts/run-qemu-i386.sh
done
```

**Edit-build-test cycle**:
```bash
# Terminal 1: Watch for file changes and rebuild
while inotifywait -e modify core/*.c; do
  make -f Makefile.docker docker-build-lites
done

# Terminal 2: Run QEMU
make -f Makefile.docker docker-run-qemu
```

### Performance Optimization

**Build optimization**:
```bash
# Use all CPU cores
export MAKEFLAGS="-j$(nproc)"

# Link-time optimization
export CFLAGS="-O3 -flto"

# ccache is already enabled
```

**QEMU optimization**:
```bash
# KVM + host CPU
qemu-system-i386 -enable-kvm -cpu host

# VIRTIO devices (faster I/O)
qemu-system-i386 -drive file=lites.qcow2,if=virtio

# Write-back cache
qemu-system-i386 -drive file=lites.qcow2,cache=writeback
```

### Multi-platform Support

```dockerfile
# In Dockerfile.i386-dev, add cross-compilers
RUN apt-get install -y \
    gcc-x86-64-linux-gnu \
    gcc-arm-linux-gnueabihf \
    gcc-riscv64-linux-gnu
```

Then:
```bash
ARCH=x86_64 docker/scripts/build-lites-i386.sh
ARCH=arm docker/scripts/build-lites-i386.sh
```

### QEMU Monitor

```bash
# Access monitor via telnet
qemu-system-i386 -monitor telnet:127.0.0.1:5555,server,nowait

# In another terminal
telnet localhost 5555
```

Monitor commands:
```
info block          # Show disks
info network        # Show network
info registers      # CPU state
savevm snapshot1    # Create VM snapshot
loadvm snapshot1    # Restore snapshot
quit                # Exit QEMU
```

---

## Reference

**Environment variables**:
```bash
ARCH=i686                          # Target architecture
BUILD_DIR=/workspace/build-i386    # Build output
LITES_MACH_DIR=/opt/mach/osfmk/kernel/src  # Mach headers
QEMU_MEMORY=256M                   # QEMU RAM
QEMU_CPUS=1                        # Virtual CPUs
```

**Key files**:
```
docker/
├── Dockerfile.i386-dev         # Image definition
├── docker-compose.yml          # Service orchestration
└── scripts/
    ├── build-lites-i386.sh     # Build automation
    ├── run-qemu-i386.sh        # QEMU launcher
    ├── docker-shell.sh         # Container entry
    ├── create-bootable-image.sh    # Bootable disk creation
    ├── convert-img-to-qcow2.sh     # Image conversion
    └── setup-mach-headers.sh       # Header setup
```

**Documentation**:
- @./README.md - Quick reference
- @./CI_CD.md - Automation workflows
- @./TROUBLESHOOTING.md - Common problems

**External**:
- GNU Mach: https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html
- QEMU: https://www.qemu.org/docs/master/
- Docker: https://docs.docker.com/

---

**Last Updated**: 2025-11-14 (documentation consolidation)
