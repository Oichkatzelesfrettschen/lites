# Getting Started with Docker i386 Development

This guide walks you through setting up and using the Docker-based development environment for building Lites on i386 architecture.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Quick Start](#quick-start)
3. [Step-by-Step Guide](#step-by-step-guide)
4. [Understanding the Environment](#understanding-the-environment)
5. [Common Workflows](#common-workflows)
6. [Troubleshooting](#troubleshooting)

## Prerequisites

Before you begin, ensure you have:

- **Docker Engine** 20.10 or later
  - Install: https://docs.docker.com/get-docker/
- **Docker Compose** v2 or later
  - Usually included with Docker Desktop
  - Standalone: https://docs.docker.com/compose/install/
- **Git** for cloning the repository
- **4GB+ free disk space** for Docker images and build artifacts
- **Optional:** KVM support for hardware-accelerated QEMU

### Verify Prerequisites

```bash
# Check Docker version
docker --version
# Should show: Docker version 20.10.x or later

# Check Docker Compose
docker compose version
# Should show: Docker Compose version v2.x.x or later

# Check Docker is running
docker ps
# Should list running containers or show empty list (no errors)

# Check KVM availability (optional, Linux only)
ls -l /dev/kvm
# If exists: you can use KVM acceleration
```

## Quick Start

The fastest way to build and run Lites i386:

```bash
# 1. Clone the repository (if not already done)
git clone https://github.com/Oichkatzelesfrettschen/lites.git
cd lites

# 2. Run the complete workflow
make -f Makefile.docker docker-all

# 3. When build completes, run in QEMU
make -f Makefile.docker docker-run-qemu
```

That's it! The Docker image will be built, Lites will be compiled for i386, and you can run it in QEMU.

## Step-by-Step Guide

### Step 1: Build the Docker Image

First, build the Docker image containing all development tools:

```bash
make -f Makefile.docker docker-build
```

This will:
- Pull the Debian base image
- Install GCC, MIG, QEMU, and other tools
- Configure the i386 build environment
- Set up a non-root development user

Expected output:
```
Building Docker image for Lites i386 development...
[+] Building 180.5s (12/12) FINISHED
 => [internal] load build definition...
 => => transferring dockerfile: 2.14kB
 ...
 => exporting to image
 => => naming to docker.io/library/lites-i386-dev:latest
```

**Time:** ~3-5 minutes on first build (cached thereafter)

### Step 2: Enter the Development Container

Open an interactive shell in the container:

```bash
make -f Makefile.docker docker-shell
```

Or use the script directly:
```bash
docker/scripts/docker-shell.sh
```

You should see a prompt like:
```
lites-i386-dev:/workspace$
```

The repository is mounted at `/workspace`, so any changes you make are reflected on your host.

### Step 3: Build Lites for i386

Inside the container, build Lites:

```bash
docker/scripts/build-lites-i386.sh
```

Options:
```bash
# Clean build
docker/scripts/build-lites-i386.sh --clean

# Use more CPU cores
docker/scripts/build-lites-i386.sh -j 8

# Use Makefile instead of CMake
docker/scripts/build-lites-i386.sh --make
```

Expected output:
```
Building Lites i386 with CMake...
  Architecture: i686
  Build directory: /workspace/build-i386
  Jobs: 4
-- The C compiler identification is GNU 12.2.0
...
[100%] Built target lites_server
Build completed successfully!
Binaries are in: /workspace/build-i386
```

**Time:** ~2-10 minutes depending on system

### Step 4: Run in QEMU

Launch the Lites kernel in QEMU i386:

```bash
docker/scripts/run-qemu-i386.sh
```

You should see:
```
KVM acceleration: enabled
Using kernel: /workspace/build-i386/lites_server

Starting QEMU i386 instance...
Command: qemu-system-i386 -m 256M -smp 1 -kernel /workspace/build-i386/lites_server ...

[Lites boot messages]
```

To exit QEMU: Press `Ctrl+A`, then `X`

### Step 5: Iterate and Develop

The typical development cycle:

1. **Edit code** on your host (files are in the mounted `/workspace`)
2. **Rebuild** inside container: `docker/scripts/build-lites-i386.sh`
3. **Test** in QEMU: `docker/scripts/run-qemu-i386.sh`
4. **Debug** if needed: `docker/scripts/run-qemu-i386.sh --gdb 1234`

## Understanding the Environment

### Directory Structure

```
lites/
├── docker/
│   ├── Dockerfile.i386-dev          # Docker image definition
│   ├── docker-compose.yml           # Service orchestration
│   ├── scripts/
│   │   ├── build-lites-i386.sh     # Build automation
│   │   ├── run-qemu-i386.sh        # QEMU launcher
│   │   └── docker-shell.sh         # Container entry helper
│   └── README.md                    # Detailed documentation
├── Makefile.docker                  # Convenience make targets
└── [source files mounted to /workspace in container]
```

### Container Environment

When you enter the container, you get:

- **Working directory:** `/workspace` (your repository root)
- **User:** `developer` (non-root for safety)
- **Compiler:** GCC 12.x with i686-linux-gnu support
- **Build tools:** CMake, Ninja, Make, MIG
- **Emulator:** QEMU system-i386
- **Debugger:** GDB with multiarch support

### Pre-configured Variables

```bash
ARCH=i686                        # Target architecture
CC="ccache gcc"                  # C compiler with cache
CFLAGS="-m32 -march=i386 ..."   # i386-specific flags
```

## Common Workflows

### Building from Outside the Container

You don't need to enter the container for simple builds:

```bash
# Build Lites
make -f Makefile.docker docker-build-lites

# Test the build
make -f Makefile.docker docker-test

# Run in QEMU
make -f Makefile.docker docker-run-qemu
```

### Debugging with GDB

To debug the kernel:

**Terminal 1 (QEMU with GDB server):**
```bash
docker/scripts/run-qemu-i386.sh --gdb 1234
```

**Terminal 2 (GDB client):**
```bash
# Enter container
docker/scripts/docker-shell.sh

# Start GDB
gdb build-i386/lites_server
(gdb) target remote localhost:1234
(gdb) break main
(gdb) continue
```

### Using VNC Display

If you want graphical output:

```bash
# Run with VNC
docker/scripts/run-qemu-i386.sh --vnc

# Connect VNC client to localhost:5900
```

### Persistent Development Session

Keep a container running in the background:

```bash
# Start container in detached mode
docker compose -f docker/docker-compose.yml up -d lites-i386-dev

# Enter whenever you want to work
docker/scripts/docker-shell.sh

# Stop when done
docker compose -f docker/docker-compose.yml down
```

### Cleaning Up

```bash
# Remove containers and volumes
make -f Makefile.docker docker-clean

# Also remove images
make -f Makefile.docker docker-distclean
```

## Troubleshooting

### "Docker daemon is not running"

**Problem:** Docker engine is not started.

**Solution:**
- **Linux:** `sudo systemctl start docker`
- **macOS/Windows:** Start Docker Desktop application

### "Permission denied" accessing /dev/kvm

**Problem:** Container can't access KVM for acceleration.

**Solution 1:** Run with privileged mode (security trade-off):
```bash
docker/scripts/docker-shell.sh --privileged
```

**Solution 2:** Add your user to the `kvm` group (Linux):
```bash
sudo usermod -aG kvm $USER
# Log out and log back in
```

**Solution 3:** Run without KVM (slower but works):
```bash
docker/scripts/run-qemu-i386.sh --no-kvm
```

### "Kernel binary not found"

**Problem:** Lites hasn't been built yet.

**Solution:**
```bash
# Build first
docker/scripts/build-lites-i386.sh

# Then run
docker/scripts/run-qemu-i386.sh
```

### Build Errors: "Mach headers not found"

**Problem:** MIG/Mach headers are missing.

**Solution 1:** Clone OpenMach:
```bash
git clone https://github.com/machkernel/openmach.git openmach
```

**Solution 2:** Use the import script:
```bash
scripts/import-mach-headers.sh
```

### Slow Build Performance

**Optimizations:**

1. **Enable KVM acceleration** (if on Linux)
2. **Increase Docker resources:**
   - Docker Desktop → Settings → Resources
   - Increase CPUs and memory
3. **Use parallel builds:**
   ```bash
   docker/scripts/build-lites-i386.sh -j $(nproc)
   ```
4. **ccache is already enabled** in the container

### QEMU Won't Start

**Check these:**

1. Verify the kernel binary exists:
   ```bash
   ls -l build-i386/lites_server
   ```

2. Check QEMU is installed:
   ```bash
   docker exec lites-i386-dev which qemu-system-i386
   ```

3. Try with debug output:
   ```bash
   docker/scripts/run-qemu-i386.sh --debug
   ```

### Container Can't Access Files

**Problem:** Permission issues with mounted volumes.

**Solution:** The container runs as user `developer` (UID may differ from host).

Fix permissions:
```bash
# On host
sudo chown -R $USER:$USER .

# Or run container as root
docker/scripts/docker-shell.sh --root
```

## Next Steps

Once you're comfortable with the basics:

1. **Read the detailed Docker README:** [docker/README.md](docker/README.md)
2. **Explore advanced QEMU options:** `docker/scripts/run-qemu-i386.sh --help`
3. **Customize the Docker image:** Edit `docker/Dockerfile.i386-dev`
4. **Set up nested GNU/Hurd:** See the GNU/Hurd documentation
5. **Contribute improvements:** Submit PRs to the repository

## Getting Help

- **Docker Documentation:** https://docs.docker.com/
- **QEMU Documentation:** https://www.qemu.org/docs/master/
- **Lites Documentation:** [docs/INDEX.md](docs/INDEX.md)
- **Issue Tracker:** https://github.com/Oichkatzelesfrettschen/lites/issues

## Summary

You now know how to:
- ✅ Build the Docker development image
- ✅ Enter the development container
- ✅ Build Lites for i386
- ✅ Run Lites in QEMU
- ✅ Debug with GDB
- ✅ Troubleshoot common issues

Happy developing! 🚀
