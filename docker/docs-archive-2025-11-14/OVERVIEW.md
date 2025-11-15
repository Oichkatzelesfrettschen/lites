# Lites i386 Development with QEMU and Docker

This document provides an overview of the complete Docker + QEMU i386 development environment for Lites, including integration with GNU/Hurd tooling.

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Quick Start](#quick-start)
4. [Documentation](#documentation)
5. [Build Options](#build-options)
6. [Development Workflows](#development-workflows)
7. [Advanced Topics](#advanced-topics)

## Overview

This repository provides two complementary development environments for building Lites on i386:

### 1. Docker Environment (Recommended)

**Path:** `docker/`

Modern containerized environment with:
- Pre-configured i386 toolchain
- MIG from Debian packages
- QEMU i386 emulation
- Fast setup and builds
- KVM acceleration support

**Best for:**
- Quick development and testing
- CI/CD integration
- Modern Linux workflow
- Cross-platform development

### 2. GNU/Hurd Environment (Optional)

**Setup:** `docker/scripts/setup-hurd-dev.sh`

Native Hurd environment with:
- Full Debian GNU/Hurd system
- Native Mach/Hurd tooling
- Pure Hurd development experience
- MIG from Hurd sources

**Best for:**
- Hurd-native development
- Testing with real Hurd system
- Learning Mach/Hurd internals
- Historical accuracy

## Architecture

### Nested Virtualization Stack

```
┌─────────────────────────────────────────────────────┐
│ Host Operating System (Linux/macOS/Windows)        │
│                                                     │
│  ┌───────────────────────────────────────────────┐ │
│  │ Docker Container (Debian Bookworm)            │ │
│  │                                               │ │
│  │  Development Tools:                           │ │
│  │  ├─ GCC 12.x with i686-linux-gnu             │ │
│  │  ├─ MIG (Mach Interface Generator)           │ │
│  │  ├─ CMake, Ninja, Make                       │ │
│  │  └─ QEMU system-i386                         │ │
│  │                                               │ │
│  │  ┌─────────────────────────────────────────┐ │ │
│  │  │ QEMU i386 Virtual Machine               │ │ │
│  │  │                                         │ │ │
│  │  │  ┌───────────────────────────────────┐ │ │ │
│  │  │  │ Lites i386 Kernel                 │ │ │ │
│  │  │  │ (4.4BSD Server on Mach)           │ │ │ │
│  │  │  │                                   │ │ │ │
│  │  │  │ - Built with MIG interfaces      │ │ │ │
│  │  │  │ - 80386 optimized code           │ │ │ │
│  │  │  │ - Mach IPC subsystem             │ │ │ │
│  │  │  └───────────────────────────────────┘ │ │ │
│  │  │                                         │ │ │
│  │  └─────────────────────────────────────────┘ │ │
│  │                                               │ │
│  └───────────────────────────────────────────────┘ │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Key Components

1. **Host Machine**: Your development workstation
2. **Docker Container**: Isolated build environment with all dependencies
3. **QEMU i386**: Emulated i386 hardware (with optional KVM acceleration)
4. **Lites Kernel**: The 4.4BSD server running on Mach microkernel

## Quick Start

### Prerequisites

```bash
# Check Docker
docker --version  # Should be 20.10+
docker compose version  # Should be v2.0+

# Check available disk space
df -h  # Need at least 4GB free
```

### 5-Minute Setup

```bash
# 1. Clone repository
git clone https://github.com/Oichkatzelesfrettschen/lites.git
cd lites

# 2. Build Docker image
make -f Makefile.docker docker-build

# 3. Build Lites for i386
make -f Makefile.docker docker-build-lites

# 4. Run in QEMU
make -f Makefile.docker docker-run-qemu
```

## Documentation

| Document | Purpose | Audience |
|----------|---------|----------|
| [GETTING_STARTED.md](GETTING_STARTED.md) | Step-by-step tutorial | New users |
| [README.md](README.md) | Detailed reference | All users |
| [Makefile.docker](../Makefile.docker) | Make target reference | Developers |
| [Dockerfile.i386-dev](Dockerfile.i386-dev) | Image configuration | Advanced users |

### Documentation Quick Links

- **New to Docker?** → Start with [GETTING_STARTED.md](GETTING_STARTED.md)
- **Need details?** → See [README.md](README.md)
- **Want to customize?** → Edit [Dockerfile.i386-dev](Dockerfile.i386-dev)
- **Troubleshooting?** → Check [GETTING_STARTED.md#troubleshooting](GETTING_STARTED.md#troubleshooting)

## Build Options

### Using Make Targets

```bash
# Complete workflow
make -f Makefile.docker docker-all

# Individual steps
make -f Makefile.docker docker-build        # Build Docker image
make -f Makefile.docker docker-build-lites  # Build Lites
make -f Makefile.docker docker-run-qemu     # Run in QEMU
make -f Makefile.docker docker-shell        # Interactive shell

# Debugging
make -f Makefile.docker docker-gdb          # Run with GDB
make -f Makefile.docker docker-test         # Run tests

# Cleanup
make -f Makefile.docker docker-clean        # Remove containers
make -f Makefile.docker docker-distclean    # Remove everything
```

### Using Scripts Directly

```bash
# Enter container
docker/scripts/docker-shell.sh

# Inside container:
docker/scripts/build-lites-i386.sh --clean -j 8
docker/scripts/run-qemu-i386.sh --kvm -m 512M
```

### Using Docker Compose

```bash
# Start development container
docker compose -f docker/docker-compose.yml up -d lites-i386-dev

# Enter container
docker compose -f docker/docker-compose.yml exec lites-i386-dev bash

# Stop
docker compose -f docker/docker-compose.yml down
```

## Development Workflows

### Workflow 1: Quick Edit-Build-Test

```bash
# Terminal 1: Keep container running
make -f Makefile.docker docker-shell

# Inside container:
while true; do
  docker/scripts/build-lites-i386.sh
  docker/scripts/run-qemu-i386.sh
done
```

### Workflow 2: Continuous Integration

```bash
# One-liner for CI
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  bash -c "docker/scripts/build-lites-i386.sh && docker/scripts/test-docker-build.sh"
```

### Workflow 3: Debugging

```bash
# Terminal 1: Start QEMU with GDB
docker/scripts/docker-shell.sh
docker/scripts/run-qemu-i386.sh --gdb 1234

# Terminal 2: Connect GDB
docker/scripts/docker-shell.sh
gdb build-i386/lites_server
(gdb) target remote localhost:1234
(gdb) break main
(gdb) continue
```

### Workflow 4: Using GNU/Hurd

```bash
# Set up Hurd environment (one-time)
docker/scripts/setup-hurd-dev.sh

# Start Hurd
cd hurd-env
./run-hurd.sh

# Inside Hurd:
apt-get update
apt-get install git build-essential mig
# Build Lites with native Hurd tools
```

## Advanced Topics

### KVM Acceleration

For significantly better performance:

```bash
# Verify KVM is available
ls -l /dev/kvm

# Run container with KVM access
docker compose -f docker/docker-compose.yml run \
  --privileged --device /dev/kvm \
  lites-i386-dev
```

Performance improvement: **10-50x faster** than pure emulation.

### Custom Build Flags

Edit environment in container:

```bash
export CFLAGS="-m32 -march=i386 -mtune=i386 -O3"
export LDFLAGS="-m32 -Wl,-z,noexecstack"
docker/scripts/build-lites-i386.sh
```

Or modify `Dockerfile.i386-dev` for permanent changes.

### Nested Docker

You can even run this Docker setup inside another Docker container:

```bash
# Parent container needs privileged mode
docker run --privileged -v /var/run/docker.sock:/var/run/docker.sock ...
```

### Multiple Architecture Support

The Docker image can be extended for other architectures:

```dockerfile
# In Dockerfile.i386-dev
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

### Networking in QEMU

To enable networking for the Lites instance:

```bash
docker/scripts/run-qemu-i386.sh \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device e1000,netdev=net0
```

### Persistent Volumes

Docker Compose creates persistent volumes:

```bash
# List volumes
docker volume ls | grep lites

# Inspect volume
docker volume inspect lites_ccache-i386

# Remove volumes
docker compose -f docker/docker-compose.yml down -v
```

### CI/CD Integration

Example GitHub Actions workflow:

```yaml
name: Build Lites i386
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build Docker image
        run: make -f Makefile.docker docker-build
      - name: Build Lites
        run: make -f Makefile.docker docker-build-lites
      - name: Test
        run: make -f Makefile.docker docker-test
```

## Comparison Matrix

| Feature | Docker Env | Hurd Env | Native |
|---------|-----------|----------|--------|
| Setup Time | 5 min | 30 min | 2 hours |
| Disk Space | 2 GB | 4 GB | 1 GB |
| Build Speed | Fast | Slow | Fastest |
| Portability | High | Medium | Low |
| KVM Support | Yes | Yes | N/A |
| MIG Source | Debian | Hurd | Manual |
| Use Case | Dev/Test | Hurd Dev | Production |

## Troubleshooting

See [GETTING_STARTED.md - Troubleshooting](GETTING_STARTED.md#troubleshooting) for common issues and solutions.

Quick fixes:

```bash
# Rebuild everything
make -f Makefile.docker docker-distclean
make -f Makefile.docker docker-all

# Check Docker resources
docker system df

# Clean up space
docker system prune -a

# Test environment
docker/scripts/test-docker-build.sh
```

## Resources

### Lites Documentation

- [Main README](../README.md)
- [Setup Guide](../docs/setup.md)
- [Architecture](../docs/ARCHITECTURE.md)

### External Resources

- [GNU Mach Documentation](https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html)
- [QEMU Documentation](https://www.qemu.org/docs/master/)
- [Docker Documentation](https://docs.docker.com/)
- [Debian GNU/Hurd](https://www.debian.org/ports/hurd/)

## Support

- **Issues**: https://github.com/Oichkatzelesfrettschen/lites/issues
- **Discussions**: https://github.com/Oichkatzelesfrettschen/lites/discussions
- **Wiki**: https://github.com/Oichkatzelesfrettschen/lites/wiki

## Contributing

Improvements to the Docker environment are welcome! Please:

1. Test your changes thoroughly
2. Update documentation
3. Run `docker/scripts/test-docker-build.sh`
4. Submit a pull request

## License

Same as the main Lites repository. See [LICENSE](../LICENSE).

---

**Happy hacking!** 🚀

For questions or issues with the Docker environment, please open an issue with the `docker` label.
