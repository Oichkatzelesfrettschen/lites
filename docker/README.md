# Docker Development Environment for Lites i386

This directory contains Docker-based development environment for building and running Lites on i386 architecture inside QEMU, all within a Docker container.

## Overview

The Docker environment provides:

- **Complete i386 toolchain** with GCC multilib support
- **GNU Mach and Hurd development tools** including MIG (Mach Interface Generator)
- **QEMU i386 emulation** with optional KVM acceleration
- **Pre-configured build environment** optimized for i386 builds
- **Convenient scripts** for building and running Lites

## Architecture

```
┌─────────────────────────────────────────────┐
│           Host Machine                      │
│                                             │
│  ┌───────────────────────────────────────┐ │
│  │   Docker Container                    │ │
│  │                                       │ │
│  │  ┌─────────────────────────────────┐ │ │
│  │  │  QEMU i386 Instance             │ │ │
│  │  │                                 │ │ │
│  │  │  ┌───────────────────────────┐ │ │ │
│  │  │  │  Lites i386 Kernel        │ │ │ │
│  │  │  │  (Built with MIG)         │ │ │ │
│  │  │  └───────────────────────────┘ │ │ │
│  │  └─────────────────────────────────┘ │ │
│  │                                       │ │
│  │  Build Tools:                         │ │
│  │  - GCC i686-linux-gnu                │ │
│  │  - MIG (Mach Interface Generator)    │ │
│  │  - CMake/Ninja                       │ │
│  │  - QEMU system-i386                  │ │
│  └───────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

## Quick Start

### Prerequisites

- Docker Engine 20.10 or later
- Docker Compose v2 or later (or docker-compose)
- At least 4GB of free disk space
- Optional: KVM support for hardware acceleration

### Build and Run

1. **Build the Docker image:**
   ```bash
   docker compose -f docker/docker-compose.yml build
   ```

2. **Enter the development container:**
   ```bash
   docker/scripts/docker-shell.sh
   ```

3. **Build Lites for i386:**
   ```bash
   # Inside the container
   docker/scripts/build-lites-i386.sh
   ```

4. **Run in QEMU:**
   ```bash
   # Inside the container
   docker/scripts/run-qemu-i386.sh
   ```

### One-liner for Quick Testing

From the repository root on your host:

```bash
# Build image, enter container, build Lites, and run in QEMU
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash -c "
  docker/scripts/build-lites-i386.sh && 
  docker/scripts/run-qemu-i386.sh
"
```

## Files and Structure

```
docker/
├── Dockerfile.i386-dev          # Main Dockerfile with all dependencies
├── docker-compose.yml           # Compose configuration for services
├── scripts/
│   ├── build-lites-i386.sh     # Build script for i386 Lites
│   ├── run-qemu-i386.sh        # Launch QEMU with Lites kernel
│   └── docker-shell.sh         # Convenience script to enter container
└── README.md                    # This file
```

## Docker Services

The `docker-compose.yml` defines two services:

### 1. lites-i386-dev (Development Container)

Interactive development environment with all build tools.

**Start the service:**
```bash
docker compose -f docker/docker-compose.yml up -d lites-i386-dev
```

**Enter the container:**
```bash
docker compose -f docker/docker-compose.yml exec lites-i386-dev bash
```

Or use the convenience script:
```bash
docker/scripts/docker-shell.sh
```

### 2. lites-qemu-i386 (QEMU Runner)

Pre-configured to automatically run Lites in QEMU when started.

**Start the service:**
```bash
docker compose -f docker/docker-compose.yml up lites-qemu-i386
```

This will automatically:
1. Start the container
2. Launch QEMU i386
3. Boot the Lites kernel

## Detailed Usage

### Building Lites

The `build-lites-i386.sh` script supports multiple options:

```bash
# Clean build with CMake (default)
docker/scripts/build-lites-i386.sh --clean

# Use Makefile.new instead of CMake
docker/scripts/build-lites-i386.sh --make

# Parallel build with 8 jobs
docker/scripts/build-lites-i386.sh -j 8

# Show help
docker/scripts/build-lites-i386.sh --help
```

**Environment variables:**
- `ARCH`: Target architecture (default: i686)
- `BUILD_DIR`: Build output directory
- `LITES_MACH_DIR`: Mach kernel headers location
- `SRCDIR`: Lites source directory

### Running in QEMU

The `run-qemu-i386.sh` script provides flexible QEMU configuration:

```bash
# Basic usage
docker/scripts/run-qemu-i386.sh

# With more memory
docker/scripts/run-qemu-i386.sh -m 512M

# Force KVM acceleration
docker/scripts/run-qemu-i386.sh --kvm

# Enable GDB debugging on port 1234
docker/scripts/run-qemu-i386.sh --gdb 1234

# Use VNC display
docker/scripts/run-qemu-i386.sh --vnc

# Show help
docker/scripts/run-qemu-i386.sh --help
```

**QEMU Features:**
- Serial console on stdio
- Optional VNC display on port 5900
- Optional GDB server for kernel debugging
- Optional QEMU monitor on telnet port 4444
- KVM acceleration when available

### Development Workflow

Typical development workflow inside the container:

```bash
# 1. Enter the container
docker/scripts/docker-shell.sh

# 2. Make code changes (files are mounted from host)
vim core/some_file.c

# 3. Build
docker/scripts/build-lites-i386.sh

# 4. Test in QEMU
docker/scripts/run-qemu-i386.sh

# 5. Debug if needed
docker/scripts/run-qemu-i386.sh --gdb 1234
# In another terminal:
gdb build-i386/lites_server
(gdb) target remote localhost:1234
(gdb) continue
```

## Environment Details

### Installed Tools

The Docker image includes:

**Build Tools:**
- GCC 12.x with multilib support
- Clang/LLVM
- CMake 3.25+
- Ninja build system
- Make, Autotools

**Mach/Hurd Tools:**
- MIG (Mach Interface Generator)
- GNU Mach headers
- Hurd development headers

**Cross-compilation:**
- i686-linux-gnu toolchain
- Binutils for i386

**Debugging:**
- GDB with multiarch support
- QEMU with debugging capabilities

**Other:**
- tmux for session management
- ccache for faster rebuilds

### Environment Variables

Inside the container, these are pre-configured:

```bash
ARCH=i686
CC="ccache gcc"
CXX="ccache g++"
CFLAGS="-m32 -march=i386 -mtune=i386"
CXXFLAGS="-m32 -march=i386 -mtune=i386"
LDFLAGS="-m32"
```

## Troubleshooting

### KVM Not Available

If you see "KVM acceleration: not available":

1. **Check host CPU virtualization:**
   ```bash
   egrep -c '(vmx|svm)' /proc/cpuinfo
   ```
   Should return > 0.

2. **Ensure KVM modules are loaded:**
   ```bash
   lsmod | grep kvm
   ```

3. **Run container with KVM access:**
   ```bash
   docker compose -f docker/docker-compose.yml run --privileged --device /dev/kvm lites-i386-dev
   ```

4. **Nested virtualization:** If running inside a VM, enable nested virtualization in the hypervisor settings.

### Build Failures

**Missing Mach headers:**
```bash
# Clone OpenMach into the repository root
git clone https://github.com/machkernel/openmach.git openmach

# Or use the import script
scripts/import-mach-headers.sh
```

**Multilib issues:**
```bash
# Ensure 32-bit libraries are installed
apt-get install gcc-multilib g++-multilib
```

### QEMU Issues

**Kernel not found:**
```bash
# Make sure you've built Lites first
docker/scripts/build-lites-i386.sh

# Verify the binary exists
ls -la build-i386/lites_server
```

**Executable stack warnings:**
The build scripts automatically add `-Wl,-z,noexecstack` to prevent this.

**Serial console issues:**
Use `-nographic` mode (default) for reliable serial output.

## Advanced Usage

### Custom Dockerfile

To extend the Docker image:

```dockerfile
FROM lites-i386-dev:latest

# Add custom tools
RUN apt-get update && apt-get install -y \
    your-package-here

# Add custom scripts
COPY my-scripts/ /usr/local/bin/
```

### Persistent Volumes

The docker-compose.yml creates volumes for:
- `ccache-i386`: Compiler cache (faster rebuilds)
- `build-i386`: Build artifacts
- `qemu-images`: QEMU disk images

To reset:
```bash
docker compose -f docker/docker-compose.yml down -v
```

### Using with CI/CD

Example GitLab CI configuration:

```yaml
build-lites-i386:
  image: lites-i386-dev:latest
  script:
    - docker/scripts/build-lites-i386.sh
  artifacts:
    paths:
      - build-i386/
```

### Networking in QEMU

To enable networking in the QEMU instance:

```bash
# User-mode networking
docker/scripts/run-qemu-i386.sh -netdev user,id=net0 -device e1000,netdev=net0

# With port forwarding (SSH on port 2222)
docker/scripts/run-qemu-i386.sh -netdev user,id=net0,hostfwd=tcp::2222-:22
```

## Performance Tips

1. **Use KVM acceleration** when available (massive speedup)
2. **Enable ccache** (already configured in the image)
3. **Use Ninja** instead of Make for faster builds
4. **Increase parallel jobs:** `-j $(nproc)`
5. **Use persistent volumes** to cache builds

## References

- [GNU Mach Building Guide](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/building.html)
- [QEMU User Documentation](https://www.qemu.org/docs/master/system/target-i386.html)
- [Docker Multi-platform Builds](https://docs.docker.com/build/building/multi-platform/)
- [Lites Documentation](../docs/INDEX.md)

## See Also

- [Main README](../README.md) - Repository overview
- [Setup Guide](../docs/setup.md) - Host setup instructions
- [Architecture](../docs/ARCHITECTURE.md) - Lites architecture details
- [QEMU Scripts](../scripts/run-qemu.sh) - Alternative QEMU runner

## License

Same as the main Lites repository. See [LICENSE](../LICENSE) for details.
