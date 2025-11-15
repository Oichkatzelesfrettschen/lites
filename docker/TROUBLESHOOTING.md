# Troubleshooting Guide for Lites Docker Environment

WHY: Centralized solutions for common issues save development time.

WHAT: Practical fixes for Docker, build, QEMU, and system problems.

HOW: Follow step-by-step solutions with verification commands.

---

## Table of Contents

1. [Docker Issues](#docker-issues)
2. [Build Failures](#build-failures)
3. [QEMU Problems](#qemu-problems)
4. [KVM and Acceleration](#kvm-and-acceleration)
5. [Disk Image Issues](#disk-image-issues)
6. [Performance Problems](#performance-problems)
7. [CI/CD Issues](#cicd-issues)

---

## Docker Issues

### Docker Daemon Not Running

**Symptom**: `Cannot connect to the Docker daemon`

**Solution**:
```bash
# Linux
sudo systemctl start docker
sudo systemctl enable docker

# macOS/Windows
# Start Docker Desktop application

# Verify
docker ps
```

### Permission Denied

**Symptom**: `Got permission denied while trying to connect to Docker`

**Solution**:
```bash
# Add user to docker group (Linux)
sudo usermod -aG docker $USER

# Log out and log back in, then verify
groups | grep docker

# Alternative: use sudo
sudo docker ps
```

### Docker Compose Not Found

**Symptom**: `docker compose: command not found`

**Solutions**:

**Check version**:
```bash
# v2 is built into Docker 20.10+
docker compose version

# If using legacy v1
docker-compose --version
```

**Install v2** (if missing):
```bash
# Linux
sudo apt-get update
sudo apt-get install docker-compose-plugin

# Or download manually
mkdir -p ~/.docker/cli-plugins/
curl -SL https://github.com/docker/compose/releases/download/v2.20.0/docker-compose-linux-x86_64 \
    -o ~/.docker/cli-plugins/docker-compose
chmod +x ~/.docker/cli-plugins/docker-compose
```

### Out of Disk Space

**Symptom**: `no space left on device`

**Check usage**:
```bash
docker system df
```

**Clean up**:
```bash
# Remove unused containers, networks, images
docker system prune -a

# Remove specific volumes
docker volume ls
docker volume rm volume_name

# Nuclear option (removes everything)
docker system prune -a --volumes
```

### Container Won't Start

**Symptom**: Container exits immediately

**Debug**:
```bash
# Check logs
docker compose -f docker/docker-compose.yml logs lites-i386-dev

# Try running interactively
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash

# Check for port conflicts
docker ps -a
```

---

## Build Failures

### Mach Headers Not Found

**Symptom**: `fatal error: mach/mach.h: No such file or directory`

**Verify headers exist**:
```bash
# Inside container
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
    bash -c "ls -la /opt/mach/osfmk/kernel/src | head -20"
```

**Solution 1: Rebuild image**:
```bash
# Force rebuild without cache
make -f Makefile.docker docker-build --no-cache
```

**Solution 2: Manual header setup**:
```bash
# Inside container
docker/scripts/setup-mach-headers.sh --docker
```

**Solution 3: Set environment variable**:
```bash
export LITES_MACH_DIR=/opt/mach/osfmk/kernel/src
cmake -B build -DLITES_MACH_DIR="${LITES_MACH_DIR}"
```

### Multilib Not Installed

**Symptom**: `/usr/bin/ld: cannot find -lc` or `skipping incompatible /usr/lib/x86_64-linux-gnu/libc.so`

**Solution**:
```bash
# Inside container
apt-get update
apt-get install gcc-multilib g++-multilib

# Rebuild image
make -f Makefile.docker docker-build --no-cache
```

### MIG Errors

**Symptom**: `mig: command not found` or MIG processing failures

**Verify MIG**:
```bash
# Inside container
which mig
mig --version

# Should show: Mach Interface Generator 1.8+
```

**Solution**:
```bash
# Reinstall MIG
apt-get update
apt-get install --reinstall mig

# Or rebuild image
make -f Makefile.docker docker-build --no-cache
```

### CMake Configuration Fails

**Symptom**: `CMake Error at CMakeLists.txt`

**Debug**:
```bash
# Inside container
cd /workspace
rm -rf build-i386
cmake -B build-i386 -DARCH=i686 --debug-output
```

**Common fixes**:
```bash
# Clear CMake cache
rm -rf build-i386/CMakeCache.txt build-i386/CMakeFiles

# Use Makefile instead
docker/scripts/build-lites-i386.sh --make

# Check CMake version
cmake --version    # Should be 3.15+
```

### Compilation Errors

**Symptom**: `error: ...` during compilation

**Debug with single-threaded build**:
```bash
# See full error messages
docker/scripts/build-lites-i386.sh -j 1
```

**Check compiler**:
```bash
gcc --version      # Should be 12.x+
gcc -m32 --version # Should support -m32 flag
```

**Verify flags**:
```bash
echo $CFLAGS       # Should include -m32 -march=i386
```

### Linker Errors

**Symptom**: `undefined reference to ...` or `cannot find -l...`

**Check libraries**:
```bash
# Find library
find /usr/lib -name 'lib*.a' | grep -i yourlib

# Check 32-bit libraries
ldconfig -p | grep '(libc6,x86)'
```

**Solution**:
```bash
# Install missing 32-bit libraries
apt-get install lib32stdc++6 lib32gcc-s1

# Or rebuild without missing dependency
```

---

## QEMU Problems

### Kernel Binary Not Found

**Symptom**: `Could not open 'build-i386/lites_server': No such file or directory`

**Solution**:
```bash
# Build first
make -f Makefile.docker docker-build-lites

# Verify binary exists
ls -lh build-i386/lites_server

# Check file type
file build-i386/lites_server
# Should show: ELF 32-bit LSB executable, Intel 80386
```

### QEMU Won't Start

**Symptom**: QEMU hangs or shows no output

**Debug**:
```bash
# Add debug flags
docker/scripts/run-qemu-i386.sh --debug

# Or run with verbose output
qemu-system-i386 -kernel build-i386/lites_server -d int,cpu_reset -D qemu.log
```

**Try different display**:
```bash
# Serial console (default)
docker/scripts/run-qemu-i386.sh

# VNC display
docker/scripts/run-qemu-i386.sh --vnc
# Then connect VNC client to localhost:5900

# No graphics at all
qemu-system-i386 -nographic -serial stdio
```

### Serial Console Not Working

**Symptom**: No output on serial console

**Solutions**:

**Check QEMU args**:
```bash
# Ensure serial is configured
qemu-system-i386 -serial stdio -nographic
```

**Kernel boot args**:
```bash
# Add console=ttyS0 to kernel parameters
qemu-system-i386 -kernel lites_server -append "console=ttyS0"
```

**Alternative: Use telnet**:
```bash
qemu-system-i386 -serial tcp::4444,server,nowait

# In another terminal
telnet localhost 4444
```

### QEMU Crashes

**Symptom**: `Segmentation fault` or `QEMU: Terminated`

**Debug**:
```bash
# Run with GDB
gdb --args qemu-system-i386 -kernel build-i386/lites_server -m 256M
(gdb) run
(gdb) backtrace
```

**Common fixes**:
```bash
# Reduce memory
docker/scripts/run-qemu-i386.sh -m 128M

# Disable KVM
docker/scripts/run-qemu-i386.sh --no-kvm

# Update QEMU
apt-get update && apt-get upgrade qemu-system-i386
```

### Executable Stack Warning

**Symptom**: `warning: executable stack` when running in QEMU

**Solution**:
Already fixed in build scripts with `-Wl,-z,noexecstack`. If still occurring:

```bash
# Check linker flags
echo $LDFLAGS        # Should include -Wl,-z,noexecstack

# Manually add flag
cmake -B build -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,noexecstack"
```

---

## KVM and Acceleration

### KVM Not Available

**Symptom**: `KVM acceleration: not available`

**Check CPU virtualization**:
```bash
# Should return > 0
egrep -c '(vmx|svm)' /proc/cpuinfo

# Check KVM modules
lsmod | grep kvm
# Should show: kvm_intel or kvm_amd
```

**Load KVM modules**:
```bash
# Intel
sudo modprobe kvm_intel

# AMD
sudo modprobe kvm_amd

# Verify
ls -l /dev/kvm
# Should show: crw-rw-rw- ... kvm
```

### Permission Denied on /dev/kvm

**Symptom**: `Could not access KVM kernel module: Permission denied`

**Solution 1: Add user to kvm group**:
```bash
sudo usermod -aG kvm $USER
# Log out and log back in
groups | grep kvm
```

**Solution 2: Fix permissions**:
```bash
sudo chmod 666 /dev/kvm
```

**Solution 3: Run Docker with KVM device**:
```bash
docker compose -f docker/docker-compose.yml run --rm \
    --device /dev/kvm:/dev/kvm lites-i386-dev
```

**Solution 4: Use privileged mode** (less secure):
```bash
docker compose -f docker/docker-compose.yml run --rm \
    --privileged lites-i386-dev
```

### Nested Virtualization

**Symptom**: KVM not available when running Docker inside a VM

**Enable nested virtualization**:

**VMware**:
```
VM Settings → Processors → Virtualization Engine
☑ Virtualize Intel VT-x/EPT or AMD-V/RVI
```

**VirtualBox**:
```bash
VBoxManage modifyvm "VM name" --nested-hw-virt on
```

**KVM/libvirt**:
```xml
<!-- In VM XML -->
<cpu mode='host-passthrough'/>
```

**Verify**:
```bash
# Inside VM
cat /sys/module/kvm_intel/parameters/nested  # Should show Y
```

---

## Disk Image Issues

### Loopback Device Busy

**Symptom**: `device or resource busy` when creating bootable images

**Solution**:
```bash
# Force unmount
sudo umount -f /tmp/lites-mount-*

# Detach all loop devices
sudo losetup -D

# Kill processes using mount
sudo lsof /tmp/lites-mount-* | awk 'NR>1 {print $2}' | xargs sudo kill -9

# Clean up
sudo rm -rf /tmp/lites-mount-*
```

### GRUB Installation Fails

**Symptom**: `grub-install` fails when creating bootable image

**Verify packages**:
```bash
dpkg -l | grep grub
# Should show: grub-pc-bin, grub2-common
```

**Install missing packages**:
```bash
apt-get update
apt-get install grub-pc-bin grub2-common
```

**Try alternative method**:
```bash
# Script uses automatic fallback
docker/scripts/create-bootable-image.sh --size 512M
```

### Image Corruption

**Symptom**: `QCOW2 image corrupted` or `Invalid backing file`

**Check and repair**:
```bash
# Check image
qemu-img check lites.qcow2

# Attempt repair
qemu-img check -r all lites.qcow2

# If unrepairable, restore from backup
cp lites.qcow2.backup lites.qcow2
```

**Prevent corruption**:
```bash
# Always shutdown QEMU cleanly (not kill -9)
# Use snapshots before risky operations
qemu-img snapshot -c before-test lites.qcow2
```

### Conversion Takes Too Long

**Symptom**: QCOW2 conversion very slow

**Speed up**:
```bash
# Remove compression
docker/scripts/convert-img-to-qcow2.sh \
    -i lites-boot.img -o lites.qcow2 \
    --prealloc off    # No compression, faster

# Use RAW format instead
qemu-img convert -f raw -O raw lites-boot.img lites.raw
```

---

## Performance Problems

### Slow Builds

**Diagnose**:
```bash
# Check ccache stats
ccache -s

# Check CPU usage
top
htop

# Check I/O
iostat -x 1
```

**Optimize**:
```bash
# Use all CPU cores
docker/scripts/build-lites-i386.sh -j $(nproc)

# Ensure ccache is working
export CC="ccache gcc"
ccache -M 5G    # Increase cache size

# Use SSD for Docker storage
# Docker Desktop → Settings → Resources → Disk image location
```

### Slow QEMU

**Check if KVM is enabled**:
```bash
# Should show KVM acceleration enabled
docker/scripts/run-qemu-i386.sh | grep -i kvm
```

**Optimize**:
```bash
# Enable KVM
docker/scripts/run-qemu-i386.sh --kvm -cpu host

# Use VIRTIO
qemu-system-i386 -drive file=lites.qcow2,if=virtio

# Increase cache
qemu-system-i386 -drive file=lites.qcow2,cache=writeback

# More memory
docker/scripts/run-qemu-i386.sh -m 512M
```

**Performance improvement**:
- With KVM: 10-50x faster than emulation
- VIRTIO: 2-3x faster disk I/O
- Writeback cache: 20-30% faster writes

### High Memory Usage

**Check**:
```bash
# Docker memory usage
docker stats

# Container memory
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
    bash -c "free -h"
```

**Optimize**:
```bash
# Limit Docker memory
docker compose -f docker/docker-compose.yml run --rm \
    -m 2g lites-i386-dev

# Reduce QEMU memory
docker/scripts/run-qemu-i386.sh -m 128M

# Clean up build artifacts
rm -rf build-i386/*.o
```

---

## CI/CD Issues

### Pipeline Fails at Build Stage

**Debug locally**:
```bash
# Run same commands locally
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
    bash -c "docker/scripts/build-lites-i386.sh"
```

**Check logs**:
```bash
# GitHub Actions: View workflow run logs
# GitLab CI: CI/CD → Pipelines → job logs
```

**Common fixes**:
```bash
# Clear cache
# GitHub: Settings → Actions → Caches → Delete
# GitLab: CI/CD → Pipelines → Clear runner caches

# Rebuild image
docker compose -f docker/docker-compose.yml build --no-cache
```

### Artifacts Upload Fails

**Check size**:
```bash
# GitHub Actions: 500 MB max per artifact
# GitLab CI: depends on runner configuration

du -sh lites-i386-dev.tar.gz
```

**Compress**:
```bash
# Better compression
gzip -9 lites-i386-dev.tar

# Or use xz
xz -9 lites-i386-dev.tar
```

### Timeout Errors

**Increase timeout**:
```yaml
# GitHub Actions
- name: Build Lites
  run: make -f Makefile.docker docker-build-lites
  timeout-minutes: 30

# GitLab CI
build:lites:
  timeout: 30m
```

**Optimize**:
```bash
# Use caching
# Reduce parallelism if runner has limited resources
docker/scripts/build-lites-i386.sh -j 2
```

---

## General Debugging

### Enable Verbose Output

```bash
# Bash scripts
set -x    # Print commands as they execute

# Docker
docker run -e DEBUG=1 ...

# QEMU
qemu-system-i386 -d int,cpu_reset -D qemu.log
```

### Check Versions

```bash
# Docker
docker --version
docker compose version

# Inside container
gcc --version
cmake --version
mig --version
qemu-system-i386 --version

# Kernel
uname -r
```

### Clean Slate

```bash
# Remove all Docker artifacts
docker compose -f docker/docker-compose.yml down -v
docker system prune -a --volumes

# Remove build artifacts
rm -rf build-i386

# Rebuild everything
make -f Makefile.docker docker-all
```

---

## Getting Help

If problems persist:

1. **Check existing issues**: https://github.com/Oichkatzelesfrettschen/lites/issues
2. **Search discussions**: https://github.com/Oichkatzelesfrettschen/lites/discussions
3. **Open new issue** with:
   - Description of problem
   - Steps to reproduce
   - Error messages (full output)
   - System info (OS, Docker version, etc.)
   - What you've tried

---

## References

- @./README.md - Quick reference
- @./DOCKER_GUIDE.md - Comprehensive guide
- @./CI_CD.md - Automation workflows
- Docker Docs: https://docs.docker.com/
- QEMU Docs: https://www.qemu.org/docs/master/

---

**Last Updated**: 2025-11-14 (documentation consolidation)
