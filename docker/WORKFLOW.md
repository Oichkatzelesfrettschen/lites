# Lites i386 Docker Development Workflow

This document illustrates the complete development workflow using the Docker-based i386 environment.

## Quick Reference Card

```
┌────────────────────────────────────────────────────────────┐
│           Lites i386 Docker Quick Commands                 │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  Setup (first time):                                       │
│    make -f Makefile.docker docker-build                   │
│                                                            │
│  Build Lites:                                             │
│    make -f Makefile.docker docker-build-lites             │
│                                                            │
│  Run in QEMU:                                             │
│    make -f Makefile.docker docker-run-qemu                │
│                                                            │
│  Development shell:                                        │
│    make -f Makefile.docker docker-shell                   │
│                                                            │
│  Complete workflow:                                        │
│    make -f Makefile.docker docker-all                     │
│                                                            │
│  Debug with GDB:                                          │
│    make -f Makefile.docker docker-gdb                     │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

## Workflow Visualization

### Basic Development Cycle

```
┌─────────────┐
│  Edit Code  │ ← You work here (on host)
│  (Host FS)  │
└──────┬──────┘
       │ mounted as /workspace
       ↓
┌─────────────────────────────────────────────┐
│           Docker Container                   │
│  ┌──────────────────────────────────────┐  │
│  │  Build Environment                   │  │
│  │  - GCC i686-linux-gnu                │  │
│  │  - MIG (Mach Interface Generator)    │  │
│  │  - CMake, Ninja                      │  │
│  │  - ccache for fast rebuilds          │  │
│  └──────────────────┬───────────────────┘  │
│                     │                        │
│                     ↓                        │
│  ┌──────────────────────────────────────┐  │
│  │  docker/scripts/build-lites-i386.sh  │  │
│  │  Compiles Lites for i386             │  │
│  └──────────────────┬───────────────────┘  │
│                     │                        │
│                     ↓                        │
│         build-i386/lites_server              │
│                     │                        │
│                     ↓                        │
│  ┌──────────────────────────────────────┐  │
│  │  QEMU system-i386                    │  │
│  │  ┌────────────────────────────────┐  │  │
│  │  │  Lites Kernel Running          │  │  │
│  │  │  - Mach IPC subsystem          │  │  │
│  │  │  - 4.4BSD compatibility layer  │  │  │
│  │  │  - Serial console output       │  │  │
│  │  └────────────────────────────────┘  │  │
│  └──────────────────────────────────────┘  │
└─────────────────────────────────────────────┘
```

### Complete Architecture Stack

```
┌───────────────────────────────────────────────────────────────┐
│                      HOST SYSTEM                              │
│  (Linux, macOS, or Windows with Docker Desktop)               │
│                                                               │
│  User's development machine with:                            │
│  - Docker Engine running                                      │
│  - Repository cloned locally                                  │
│  - IDE/editor of choice                                       │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │               DOCKER CONTAINER                          │ │
│  │  Image: lites-i386-dev:latest                          │ │
│  │  OS: Debian Bookworm                                    │ │
│  │                                                         │ │
│  │  Mounted Volumes:                                       │ │
│  │  - Repository root → /workspace (rw)                   │ │
│  │  - ccache-i386 volume (persistent cache)               │ │
│  │  - build-i386 volume (build artifacts)                 │ │
│  │                                                         │ │
│  │  Environment:                                           │ │
│  │  ┌───────────────────────────────────────────────────┐ │ │
│  │  │  Toolchain Layer                                  │ │ │
│  │  │  ├─ gcc-12 with i686-linux-gnu target            │ │ │
│  │  │  ├─ MIG (Mach Interface Generator) 1.8+          │ │ │
│  │  │  ├─ GNU Make, CMake 3.25+, Ninja                 │ │ │
│  │  │  ├─ Bison, Flex for parser generation            │ │ │
│  │  │  └─ ccache for incremental builds                │ │ │
│  │  └───────────────────────────────────────────────────┘ │ │
│  │                                                         │ │
│  │  ┌───────────────────────────────────────────────────┐ │ │
│  │  │  Build Process                                    │ │ │
│  │  │                                                   │ │ │
│  │  │  1. CMake Configuration                          │ │ │
│  │  │     - Detect Mach headers                        │ │ │
│  │  │     - Set i386 compiler flags                    │ │ │
│  │  │     - Configure build targets                    │ │ │
│  │  │                                                   │ │ │
│  │  │  2. MIG Processing                               │ │ │
│  │  │     - Process .defs files                        │ │ │
│  │  │     - Generate IPC stubs                         │ │ │
│  │  │                                                   │ │ │
│  │  │  3. Compilation                                   │ │ │
│  │  │     - Compile with -m32 -march=i386             │ │ │
│  │  │     - Link with Mach libraries                   │ │ │
│  │  │     - Create lites_server binary                │ │ │
│  │  └───────────────────────────────────────────────────┘ │ │
│  │                       ↓                                 │ │
│  │  ┌───────────────────────────────────────────────────┐ │ │
│  │  │  QEMU Emulation Layer                            │ │ │
│  │  │  Binary: qemu-system-i386                        │ │ │
│  │  │                                                   │ │ │
│  │  │  Configuration:                                   │ │ │
│  │  │  - CPU: i386 (or host with KVM)                 │ │ │
│  │  │  - Memory: 256MB default (configurable)          │ │ │
│  │  │  - Kernel: /workspace/build-i386/lites_server   │ │ │
│  │  │  - Console: Serial on stdio                      │ │ │
│  │  │  - Acceleration: KVM if /dev/kvm available      │ │ │
│  │  │                                                   │ │ │
│  │  │  ┌─────────────────────────────────────────────┐ │ │ │
│  │  │  │  EMULATED i386 SYSTEM                       │ │ │ │
│  │  │  │                                             │ │ │ │
│  │  │  │  ┌───────────────────────────────────────┐ │ │ │ │
│  │  │  │  │  Lites i386 Kernel                    │ │ │ │ │
│  │  │  │  │                                       │ │ │ │ │
│  │  │  │  │  Components:                          │ │ │ │ │
│  │  │  │  │  ┌─────────────────────────────────┐ │ │ │ │ │
│  │  │  │  │  │  Mach Microkernel Interface     │ │ │ │ │ │
│  │  │  │  │  │  - IPC subsystem                │ │ │ │ │ │
│  │  │  │  │  │  - Port management              │ │ │ │ │ │
│  │  │  │  │  │  - Thread/task primitives       │ │ │ │ │ │
│  │  │  │  │  └─────────────────────────────────┘ │ │ │ │ │
│  │  │  │  │  ┌─────────────────────────────────┐ │ │ │ │ │
│  │  │  │  │  │  4.4BSD Server                  │ │ │ │ │ │
│  │  │  │  │  │  - POSIX syscall layer          │ │ │ │ │ │
│  │  │  │  │  │  - VFS and file systems         │ │ │ │ │ │
│  │  │  │  │  │  - Network stack                │ │ │ │ │ │
│  │  │  │  │  │  - Device drivers               │ │ │ │ │ │
│  │  │  │  │  └─────────────────────────────────┘ │ │ │ │ │
│  │  │  │  │                                       │ │ │ │ │
│  │  │  │  │  Boot Process:                        │ │ │ │ │
│  │  │  │  │  1. QEMU loads kernel at 0x100000    │ │ │ │ │
│  │  │  │  │  2. Kernel initializes Mach IPC      │ │ │ │ │
│  │  │  │  │  3. BSD server starts                │ │ │ │ │
│  │  │  │  │  4. Serial console activates         │ │ │ │ │
│  │  │  │  │                                       │ │ │ │ │
│  │  │  │  └───────────────────────────────────────┘ │ │ │ │
│  │  │  │                                             │ │ │ │
│  │  │  │  Debug Features:                            │ │ │ │
│  │  │  │  - GDB remote stub (--gdb 1234)            │ │ │ │
│  │  │  │  - Serial console output                    │ │ │ │
│  │  │  │  - QEMU monitor (--monitor)                 │ │ │ │
│  │  │  │                                             │ │ │ │
│  │  │  └─────────────────────────────────────────────┘ │ │ │
│  │  └───────────────────────────────────────────────────┘ │ │
│  └─────────────────────────────────────────────────────────┘ │
└───────────────────────────────────────────────────────────────┘
```

## Typical Workflows

### 1. First-Time Setup

```bash
# Clone repository
git clone https://github.com/Oichkatzelesfrettschen/lites.git
cd lites

# Build Docker image (5-10 minutes)
make -f Makefile.docker docker-build

# Build Lites (2-5 minutes)
make -f Makefile.docker docker-build-lites

# Run in QEMU
make -f Makefile.docker docker-run-qemu
```

### 2. Development Iteration

```bash
# Edit code on host with your favorite editor
vim core/some_file.c

# Rebuild (fast with ccache)
make -f Makefile.docker docker-build-lites

# Test in QEMU
make -f Makefile.docker docker-run-qemu
```

### 3. Interactive Development

```bash
# Terminal 1: Enter container
make -f Makefile.docker docker-shell

# Inside container - continuous development
while true; do
  clear
  docker/scripts/build-lites-i386.sh
  echo "Build complete. Starting QEMU..."
  docker/scripts/run-qemu-i386.sh
done
```

### 4. Debugging Session

```bash
# Terminal 1: Start QEMU with GDB server
make -f Makefile.docker docker-gdb

# Terminal 2: Connect GDB
docker/scripts/docker-shell.sh
gdb build-i386/lites_server
(gdb) target remote localhost:1234
(gdb) break main
(gdb) continue
(gdb) step
```

### 5. CI/CD Pipeline

```yaml
# .github/workflows/build-i386.yml
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
      - uses: actions/upload-artifact@v3
        with:
          name: lites-i386-kernel
          path: build-i386/lites_server
```

## File Flow Diagram

```
Repository Root (Host)
├── docker/
│   ├── Dockerfile.i386-dev ─────┐
│   ├── docker-compose.yml ──────┤─→ Build Docker Image
│   ├── scripts/                 │
│   │   ├── build-lites-i386.sh ─┼─→ Build Automation
│   │   ├── run-qemu-i386.sh ────┼─→ QEMU Launcher
│   │   └── docker-shell.sh ─────┼─→ Container Entry
│   └── README.md                │
├── core/ ───────────────────────┼─→ Mounted in Container
├── drivers/ ────────────────────┤   at /workspace
├── servers/ ────────────────────┤
└── [other source files] ────────┘
                                 │
                      Container Filesystem
                                 │
                                 ↓
/workspace/ (mounted from host)
├── docker/scripts/build-lites-i386.sh
│   │
│   ↓
│   Invokes CMake/Make
│   │
│   ↓
│   Processes .defs with MIG
│   │
│   ↓
│   Compiles with gcc -m32 -march=i386
│   │
│   ↓
build-i386/lites_server ←──────┐
    │                          │
    ↓                          │
docker/scripts/run-qemu-i386.sh│
    │                          │
    ↓                          │
qemu-system-i386 -kernel ──────┘
    │
    ↓
[Lites boots in QEMU]
    │
    ↓
Serial console output on stdio
```

## Tool Interactions

```
┌──────────────┐
│ Docker CLI   │
└──────┬───────┘
       │ docker compose up
       ↓
┌──────────────────┐
│ Docker Compose   │
└──────┬───────────┘
       │ creates container from
       ↓
┌──────────────────────────┐
│ Dockerfile.i386-dev      │
│ (Multi-stage build)      │
└──────┬───────────────────┘
       │ installs
       ↓
┌──────────────────────────┐
│ Development Tools:       │
│ - gcc, g++              │
│ - mig, gnumach-dev      │
│ - qemu-system-i386      │
│ - cmake, ninja          │
└──────┬───────────────────┘
       │ used by
       ↓
┌──────────────────────────┐
│ build-lites-i386.sh      │
│ (Build automation)       │
└──────┬───────────────────┘
       │ invokes
       ├─→ CMake (configuration)
       ├─→ MIG (interface gen)
       ├─→ GCC (compilation)
       └─→ LD (linking)
       │
       │ produces
       ↓
┌──────────────────────────┐
│ lites_server (i386 ELF)  │
└──────┬───────────────────┘
       │ loaded by
       ↓
┌──────────────────────────┐
│ run-qemu-i386.sh         │
└──────┬───────────────────┘
       │ executes
       ↓
┌──────────────────────────┐
│ qemu-system-i386         │
│ (with KVM if available)  │
└──────────────────────────┘
```

## Performance Characteristics

```
Operation               | Time (No KVM) | Time (With KVM) | Notes
────────────────────────┼───────────────┼─────────────────┼─────────────
Docker image build      | 5-10 min      | 5-10 min        | One-time
Initial Lites build     | 5-10 min      | 5-10 min        | With ccache
Incremental rebuild     | 30-120 sec    | 30-120 sec      | Few files
QEMU boot (to console)  | 30-60 sec     | 5-10 sec        | 6-10x faster!
Development cycle       | 1-3 min       | 1-2 min         | Edit→Build→Test
```

## Resource Requirements

```
Component          | Minimum | Recommended | Notes
───────────────────┼─────────┼─────────────┼──────────────────────
Host RAM           | 4 GB    | 8 GB+       | For Docker + QEMU
Host Disk Space    | 5 GB    | 10 GB+      | Image + builds
Host CPU           | 2 cores | 4+ cores    | Parallel builds
Docker RAM limit   | 2 GB    | 4 GB        | Container memory
QEMU RAM           | 256 MB  | 512 MB      | Guest memory
```

## See Also

- [OVERVIEW.md](OVERVIEW.md) - Complete architecture documentation
- [GETTING_STARTED.md](GETTING_STARTED.md) - Tutorial for beginners
- [README.md](README.md) - Detailed feature reference
- [../docs/setup.md](../docs/setup.md) - Native build setup
