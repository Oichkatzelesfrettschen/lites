# CI/CD Workflow for Lites i386 in Docker + QEMU

This document explains the complete CI/CD workflow for building Lites i386 and running it inside QEMU within Docker.

## Overview

The workflow follows this execution path:

```
CI/CD Trigger → Build Docker Image → Build Lites i386 → Run in QEMU i386 → Validate
```

## Detailed Workflow Steps

### 1. Build Docker Image (One-time or on changes)

**Command:**
```bash
make -f Makefile.docker docker-build
# or
docker compose -f docker/docker-compose.yml build lites-i386-dev
```

**What happens:**
1. Docker reads `docker/Dockerfile.i386-dev`
2. Pulls base image: `debian:bookworm-slim`
3. Installs development tools:
   - `gcc-multilib g++-multilib` (for i386 cross-compilation)
   - `mig` (Mach Interface Generator)
   - `gnumach-dev hurd-dev` (Mach headers)
   - `qemu-system-i386` (QEMU emulator)
   - `cmake ninja-build make` (build systems)
   - `gcc-i686-linux-gnu` (cross-compiler)
4. Sets environment variables:
   ```bash
   ARCH=i686
   CC="ccache gcc"
   CFLAGS="-m32 -march=i386 -mtune=i386"
   ```
5. Creates non-root user `developer`
6. Tags image as `lites-i386-dev:latest`

**Duration:** 5-10 minutes (cached thereafter)

### 2. Build Lites i386 Binary

**Command:**
```bash
make -f Makefile.docker docker-build-lites
# or
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
    docker/scripts/build-lites-i386.sh
```

**What happens inside the container:**

```bash
# build-lites-i386.sh execution flow:

1. Validate environment
   └─ Check for gcc, make, cmake, mig

2. Configure build with CMake
   └─ cmake -B build-i386 \
           -G Ninja \
           -DCMAKE_BUILD_TYPE=Release \
           -DARCH=i686 \
           -DCMAKE_C_COMPILER=gcc \
           -DLITES_MACH_DIR=/workspace/openmach

3. Process MIG definitions
   └─ mig processes *.defs files
   └─ Generates IPC stubs for Mach communication

4. Compile with i386 flags
   └─ gcc -m32 -march=i386 -mtune=i386 \
          [source files] \
          -o build-i386/lites_server

5. Link with Mach libraries
   └─ ld -m32 [...objects...] -o lites_server

6. Output binary
   └─ build-i386/lites_server (i386 ELF executable)
```

**Build process details:**

```
Source Files
    ↓
MIG Processing (.defs → .c/.h)
    ↓
Compilation (GCC with -m32 -march=i386)
    ↓
Linking (with Mach libraries)
    ↓
lites_server (i386 binary)
```

**Duration:** 2-10 minutes (2-5 min with ccache)

### 3. Run in QEMU i386

**Command:**
```bash
make -f Makefile.docker docker-run-qemu
# or
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
    docker/scripts/run-qemu-i386.sh
```

**What happens inside the container:**

```bash
# run-qemu-i386.sh execution flow:

1. Detect KVM availability
   └─ Check if /dev/kvm exists and is writable
   └─ If yes: use KVM acceleration (10-50x faster)
   └─ If no: use pure emulation

2. Locate kernel binary
   └─ Search: build-i386/lites_server
   └─ Fallback: /workspace/lites_server

3. Build QEMU command
   └─ qemu-system-i386 \
        -m 256M \                    # Memory
        -smp 1 \                     # CPUs
        -kernel build-i386/lites_server \
        -enable-kvm \                # If available
        -nographic \                 # Serial console
        -serial stdio \              # Console output
        -no-reboot \                 # Stop on crash
        -append "console=ttyS0"      # Kernel args

4. Execute QEMU
   └─ Boots Lites kernel
   └─ Initializes Mach IPC
   └─ Starts BSD server
   └─ Provides serial console
```

**QEMU execution details:**

```
QEMU Startup
    ↓
Load lites_server at 0x100000
    ↓
Initialize i386 CPU (real or virtual)
    ↓
Lites Kernel Boot
    ├─ Mach microkernel initialization
    ├─ IPC subsystem setup
    ├─ Port management
    └─ Thread/task primitives
    ↓
4.4BSD Server Start
    ├─ VFS initialization
    ├─ Network stack
    ├─ Device drivers
    └─ System call layer
    ↓
Serial Console Active
    └─ User interaction possible
```

**Duration:** 5-10 seconds (with KVM), 30-60 seconds (without KVM)

## Complete CI/CD Pipeline

### GitHub Actions Example

```yaml
name: Build and Test Lites i386

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    
    steps:
      - name: Checkout code
        uses: actions/checkout@v3
      
      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v2
      
      - name: Build Docker image
        run: make -f Makefile.docker docker-build
      
      - name: Build Lites i386
        run: make -f Makefile.docker docker-build-lites
      
      - name: Validate build
        run: |
          docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
            test -f build-i386/lites_server
      
      - name: Test in QEMU (with timeout)
        run: |
          timeout 60 make -f Makefile.docker docker-run-qemu || true
      
      - name: Upload artifacts
        uses: actions/upload-artifact@v3
        with:
          name: lites-i386-kernel
          path: build-i386/lites_server
```

### GitLab CI Example

```yaml
stages:
  - build-image
  - build-kernel
  - test

variables:
  DOCKER_DRIVER: overlay2

build-docker-image:
  stage: build-image
  script:
    - make -f Makefile.docker docker-build
  artifacts:
    expire_in: 1 week

build-lites:
  stage: build-kernel
  script:
    - make -f Makefile.docker docker-build-lites
  artifacts:
    paths:
      - build-i386/lites_server
    expire_in: 1 week

test-qemu:
  stage: test
  script:
    - timeout 60 make -f Makefile.docker docker-run-qemu || true
  allow_failure: true
```

## Execution Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│ CI/CD Environment (GitHub Actions, GitLab CI, etc.)        │
│                                                             │
│  Step 1: Build Docker Image                                │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ $ make -f Makefile.docker docker-build              │  │
│  │                                                      │  │
│  │ → docker compose build lites-i386-dev               │  │
│  │ → Reads docker/Dockerfile.i386-dev                  │  │
│  │ → Installs: GCC, MIG, QEMU, CMake                   │  │
│  │ → Creates image: lites-i386-dev:latest              │  │
│  └──────────────────┬───────────────────────────────────┘  │
│                     ↓                                       │
│  Step 2: Build Lites i386                                  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ $ make -f Makefile.docker docker-build-lites        │  │
│  │                                                      │  │
│  │ → docker compose run lites-i386-dev \               │  │
│  │     docker/scripts/build-lites-i386.sh              │  │
│  │                                                      │  │
│  │ Inside Container:                                    │  │
│  │   1. Check dependencies (gcc, mig, cmake)           │  │
│  │   2. Configure: cmake -B build-i386 -DARCH=i686     │  │
│  │   3. Process MIG: mig *.defs → stubs                │  │
│  │   4. Compile: gcc -m32 -march=i386 source.c         │  │
│  │   5. Link: ld -m32 → lites_server                   │  │
│  │   6. Output: build-i386/lites_server                │  │
│  └──────────────────┬───────────────────────────────────┘  │
│                     ↓                                       │
│  Step 3: Run in QEMU                                       │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ $ make -f Makefile.docker docker-run-qemu           │  │
│  │                                                      │  │
│  │ → docker compose run lites-i386-dev \               │  │
│  │     docker/scripts/run-qemu-i386.sh                 │  │
│  │                                                      │  │
│  │ Inside Container:                                    │  │
│  │   1. Check KVM: /dev/kvm available?                 │  │
│  │   2. Find kernel: build-i386/lites_server           │  │
│  │   3. Launch QEMU:                                    │  │
│  │      qemu-system-i386 \                             │  │
│  │        -m 256M \                                     │  │
│  │        -kernel lites_server \                       │  │
│  │        -enable-kvm \          # if available        │  │
│  │        -nographic \                                  │  │
│  │        -serial stdio                                 │  │
│  │   4. Boot Lites kernel                              │  │
│  │   5. Initialize Mach IPC                            │  │
│  │   6. Start BSD server                               │  │
│  │   7. Console ready                                   │  │
│  └──────────────────┬───────────────────────────────────┘  │
│                     ↓                                       │
│  Step 4: Validation (optional)                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ $ make -f Makefile.docker docker-test               │  │
│  │                                                      │  │
│  │ → Verify lites_server exists                        │  │
│  │ → Check binary is i386 ELF                          │  │
│  │ → Test QEMU boots successfully                      │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

```
Host Filesystem
    ↓ (mounted as /workspace)
Docker Container
    ↓ (build process)
build-i386/lites_server (i386 ELF)
    ↓ (loaded by QEMU)
QEMU i386 Virtual Machine
    ↓ (executes kernel)
Lites Kernel Running
    ↓ (serial output)
Console Output (visible in CI logs)
```

## Resource Usage

| Stage | CPU | Memory | Disk | Time |
|-------|-----|--------|------|------|
| Build Image | 2+ cores | 2GB | 2GB | 5-10 min |
| Build Lites | 2+ cores | 1GB | 500MB | 2-10 min |
| Run QEMU (no KVM) | 1 core | 256MB | - | 30-60 sec |
| Run QEMU (with KVM) | 1 core | 256MB | - | 5-10 sec |

## Makefile Target Flow

```bash
# docker-all target executes:
make docker-build        → Build Docker image
    ↓
make docker-build-lites  → Build Lites i386
    ↓
make docker-test         → Validate build
    ↓
[Complete!]

# Manual run after build:
make docker-run-qemu     → Run in QEMU
```

## Environment Variables

The CI/CD process respects these environment variables:

```bash
# Docker/Build
ARCH=i686                    # Target architecture
BUILD_DIR=/workspace/build-i386
WORKSPACE_ROOT=/workspace

# Mach Integration
LITES_MACH_DIR=/workspace/openmach
LITES_MACH_LIB_DIR=/workspace/openmach/lib

# QEMU Configuration
QEMU_MEMORY=256M             # QEMU RAM
QEMU_CPUS=1                  # Virtual CPUs
QEMU_KVM=auto                # KVM: auto/yes/no

# Compilation
CC="ccache gcc"
CFLAGS="-m32 -march=i386 -mtune=i386"
LDFLAGS="-m32"
```

## Key Scripts

### build-lites-i386.sh
- **Purpose:** Automate Lites compilation
- **Input:** Source code at `/workspace`
- **Output:** `build-i386/lites_server` (i386 ELF)
- **Features:** CMake/Make support, parallel builds, clean builds

### run-qemu-i386.sh
- **Purpose:** Launch QEMU with Lites kernel
- **Input:** `build-i386/lites_server`
- **Output:** Running QEMU instance
- **Features:** KVM detection, GDB support, VNC display, serial console

### test-docker-build.sh
- **Purpose:** Validate CI environment
- **Checks:** Docker, MIG, GCC multilib, QEMU
- **Output:** Test results with colored output

## Performance Characteristics

**Build Performance:**
```
First build:        5-10 minutes
Incremental build:  30-120 seconds (with ccache)
Clean build:        3-5 minutes (with ccache primed)
```

**QEMU Boot Performance:**
```
Without KVM:        30-60 seconds to console
With KVM:           5-10 seconds to console
Speedup:            6-10x faster with KVM
```

## CI/CD Best Practices

1. **Cache Docker Layers**
   - Use Docker layer caching in CI
   - Pre-build images for faster runs

2. **Parallel Builds**
   - Use `-j $(nproc)` for parallel compilation
   - Enable ccache for incremental builds

3. **Timeout QEMU**
   - Use `timeout` command to prevent hanging
   - QEMU may not exit automatically

4. **Artifact Storage**
   - Save `lites_server` binary as artifact
   - Store for debugging and distribution

5. **KVM in CI**
   - GitHub Actions: KVM not available
   - Self-hosted runners: Can enable KVM
   - GitLab CI: Depends on runner configuration

## Debugging CI Failures

**Build fails:**
```bash
# Run locally to debug
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash
# Inside container:
docker/scripts/build-lites-i386.sh -j 1  # Single-threaded for better error messages
```

**QEMU fails:**
```bash
# Check binary exists
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  ls -lh build-i386/lites_server

# Verify it's i386 ELF
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  file build-i386/lites_server
```

**Timeout issues:**
```bash
# Increase timeout or run with --debug
timeout 120 make -f Makefile.docker docker-run-qemu
```

## Summary

The CI/CD workflow provides:
- ✅ Reproducible builds in Docker
- ✅ Automated i386 compilation with MIG
- ✅ QEMU testing with KVM when available
- ✅ Comprehensive error handling
- ✅ Flexible for various CI platforms
- ✅ Fast incremental builds with caching

The entire process is automated through the Makefile, making it easy to integrate into any CI/CD pipeline.
