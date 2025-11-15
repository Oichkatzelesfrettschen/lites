# Docker Development Environment for Lites i386

WHY: Containerized development environment provides reproducible builds across
     platforms with all dependencies pre-configured.

WHAT: Complete Docker-based toolchain for building and testing Lites on i386
      architecture with QEMU emulation.

HOW: Use docker compose v2 with provided scripts and Make targets.

---

## Quick Start

```bash
# Complete workflow (5 minutes)
make -f Makefile.docker docker-all

# Individual steps
make -f Makefile.docker docker-build        # Build image (first time only)
make -f Makefile.docker docker-build-lites  # Compile Lites
make -f Makefile.docker docker-run-qemu     # Run in QEMU

# Development shell
make -f Makefile.docker docker-shell
```

## What This Provides

- **Pre-configured i386 toolchain**: GCC multilib, MIG, QEMU, CMake/Ninja
- **Nested virtualization**: QEMU i386 runs inside Docker container
- **Automated build scripts**: One command to build and test
- **KVM acceleration support**: 10-50x faster emulation when available
- **Persistent caching**: ccache for faster incremental builds
- **Cross-platform**: Works on Linux, macOS, Windows with Docker Desktop

## Architecture

```
Host System
  └─ Docker Container (Debian Bookworm)
      ├─ i686-linux-gnu toolchain (GCC 12+)
      ├─ MIG (Mach Interface Generator)
      ├─ QEMU system-i386
      └─ QEMU i386 Virtual Machine
          └─ Lites Kernel (4.4BSD on Mach)
```

## Directory Structure

```
docker/
├── README.md                    # This file (overview and quick reference)
├── DOCKER_GUIDE.md             # Comprehensive Docker build and development guide
├── CI_CD.md                    # GitHub Actions and automation workflows
├── TROUBLESHOOTING.md          # Common issues and solutions
├── Dockerfile.i386-dev         # Docker image definition
├── docker-compose.yml          # Container orchestration (v2)
└── scripts/
    ├── build-lites-i386.sh     # Build automation
    ├── run-qemu-i386.sh        # QEMU launcher with KVM detection
    ├── docker-shell.sh         # Container entry helper
    ├── create-bootable-image.sh    # Bootable disk image creation
    ├── convert-img-to-qcow2.sh     # Image format conversion
    └── [additional scripts]
```

## Documentation Guide

**New to Docker?** → Start with @./DOCKER_GUIDE.md#quick-start

**Setting up CI/CD?** → See @./CI_CD.md

**Build failing?** → Check @./TROUBLESHOOTING.md

**Advanced topics?** → See @./DOCKER_GUIDE.md#advanced-usage

## Common Commands

### Build Operations

```bash
# Enter container for interactive development
docker/scripts/docker-shell.sh

# Inside container: build Lites
docker/scripts/build-lites-i386.sh --clean -j 8

# Clean build (remove all build artifacts)
make -f Makefile.docker docker-clean
```

### QEMU Operations

```bash
# Run with default settings
docker/scripts/run-qemu-i386.sh

# With more memory
docker/scripts/run-qemu-i386.sh -m 512M

# Enable GDB debugging
docker/scripts/run-qemu-i386.sh --gdb 1234

# With VNC display
docker/scripts/run-qemu-i386.sh --vnc
```

### Image Management

```bash
# Build bootable disk image
make -f Makefile.docker docker-bootimg

# Convert to QCOW2 format
make -f Makefile.docker docker-qcow2
```

## Environment Variables

Pre-configured in the container:

```bash
ARCH=i686                          # Target architecture
CC="ccache gcc"                    # C compiler with caching
CFLAGS="-m32 -march=i386 -mtune=i386"  # i386-specific flags
LITES_MACH_DIR=/opt/mach/osfmk/kernel/src  # Mach headers location
```

Override as needed:

```bash
# Custom build directory
export BUILD_DIR=/workspace/custom-build

# Different architecture target
export ARCH=x86_64
```

## Docker Compose v2 Syntax

This project uses modern Docker Compose v2 (not legacy v1):

```bash
# Correct (v2)
docker compose version
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash

# Legacy v1 (not used)
docker-compose version
docker-compose -f docker/docker-compose.yml build
```

## Prerequisites

- **Docker Engine**: 20.10 or later
- **Docker Compose**: v2 (included with Docker Desktop)
- **Disk space**: 5-10 GB free
- **Optional**: KVM support for acceleration (Linux only)

### Verify Setup

```bash
# Check Docker version
docker --version            # Should be 20.10+

# Check Compose v2
docker compose version      # Should be v2.x+

# Check available space
df -h                       # Need 5+ GB free

# Check KVM (Linux only, optional)
ls -l /dev/kvm             # If exists, KVM available
```

## System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| Docker | 20.10+ | Latest stable |
| RAM | 4 GB | 8 GB+ |
| Disk | 5 GB | 10 GB+ |
| CPU | 2 cores | 4+ cores |

## Performance Tips

1. **Enable KVM acceleration** (Linux): 10-50x faster QEMU
2. **Use parallel builds**: `-j $(nproc)` flag
3. **Persistent volumes**: Docker Compose volumes cache builds
4. **ccache**: Already enabled in container for incremental builds
5. **SSD storage**: Significantly faster than HDD

## Typical Workflows

### Daily Development

```bash
# 1. Edit code on host (files mounted in container)
vim core/myfile.c

# 2. Rebuild in container (fast with ccache)
make -f Makefile.docker docker-build-lites

# 3. Test in QEMU
make -f Makefile.docker docker-run-qemu
```

### Debugging Session

```bash
# Terminal 1: Start QEMU with GDB server
make -f Makefile.docker docker-gdb

# Terminal 2: Connect GDB
docker/scripts/docker-shell.sh
gdb build-i386/lites_server
(gdb) target remote localhost:1234
(gdb) break main
(gdb) continue
```

### CI/CD Integration

See @./CI_CD.md for complete GitHub Actions and GitLab CI examples.

```bash
# One-liner for CI (build, test, validate)
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  bash -c "docker/scripts/build-lites-i386.sh && docker/scripts/test-docker-build.sh"
```

## Quick Troubleshooting

**Build fails: Mach headers not found**
```bash
# Headers are pre-installed at /opt/mach/osfmk in container
# If missing, rebuild image:
make -f Makefile.docker docker-build --no-cache
```

**KVM not available**
```bash
# Check host has KVM
ls -l /dev/kvm

# Run container with KVM device
docker compose -f docker/docker-compose.yml run \
  --device /dev/kvm:/dev/kvm lites-i386-dev
```

**QEMU won't start**
```bash
# Verify kernel binary exists
ls -l build-i386/lites_server

# Check file type
file build-i386/lites_server    # Should be: ELF 32-bit LSB executable, Intel 80386
```

For more issues, see @./TROUBLESHOOTING.md

## References

- **Lites Project**: @../README.md (main repository documentation)
- **Docker Guide**: @./DOCKER_GUIDE.md (comprehensive reference)
- **CI/CD Setup**: @./CI_CD.md (automation workflows)
- **Troubleshooting**: @./TROUBLESHOOTING.md (common problems)
- **Mach Documentation**: https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html
- **QEMU Documentation**: https://www.qemu.org/docs/master/system/target-i386.html

## Getting Help

- **Issues**: https://github.com/Oichkatzelesfrettschen/lites/issues
- **Discussions**: https://github.com/Oichkatzelesfrettschen/lites/discussions

---

**License**: Same as main Lites repository (see @../LICENSE)

**Last Updated**: 2025-11-14 (documentation consolidation)
