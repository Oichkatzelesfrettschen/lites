# Lites Build Requirements

This document outlines all requirements and dependencies for building Lites.

## System Requirements

### Supported Platforms
- **Linux** (primary): Ubuntu 20.04+, Debian Bookworm+, Fedora 35+
- **macOS**: 12+ (with Docker Desktop)
- **Windows**: WSL2 or Docker Desktop

### Build Tools (Required)

#### Core Toolchain
- **GCC** 10.0+ or **Clang** 12.0+
  - For i386 builds: `gcc-i686-linux-gnu` and `g++-i686-linux-gnu`
  - For x86_64 builds: `gcc` and `g++`
- **GNU Make** 4.3+
- **CMake** 3.15+
- **Ninja** 1.10+ (optional, for faster builds)

#### Mach Interface Generator
- **MIG** (Mach Interface Generator)
  - Debian/Ubuntu: Install `gnumach-dev`, `hurd-dev`, `mig` packages
  - Fedora: Install `gnumach-devel` package
  - macOS: Use Docker environment (MIG not natively available)

#### Additional Tools
- **Git** 2.25+
- **Python** 3.7+ (for pre-commit hooks)
- **Bash** 4.0+

### Development Tools (Optional but Recommended)

- **clang-format** 12.0+ (for code formatting)
- **clang-tidy** 12.0+ (for static analysis)
- **GDB** or **GDB-multiarch** (for debugging)
- **ccache** (for faster rebuilds)
- **tmux** (for terminal multiplexing)

## Mach Kernel Headers

Lites requires Mach kernel headers for compilation. Three options are available:

### Option 1: Use Docker Environment (Recommended)

The Docker environment includes pre-configured Mach headers from OSF Mach 6.1:

```bash
# Build and enter Docker environment
make -f Makefile.docker docker-build
make -f Makefile.docker docker-shell
```

Headers are automatically available at `/opt/mach/osfmk/kernel/src`.

### Option 2: Manual Header Setup

Run the setup script to configure headers locally:

```bash
./scripts/setup-build-environment.sh
source .env.build
```

This script will:
1. Check for existing headers in `localmach/`, `openmach/`, or `/opt/mach/`
2. Create minimal header stubs if none are found
3. Set environment variables for the build system

### Option 3: Use Openmach Submodule

Clone the openmach repository as a Git submodule:

```bash
git submodule update --init --recursive openmach
```

## Docker Environment

The Docker-based development environment provides a complete, pre-configured toolchain:

### Installation

1. Install Docker Engine 20.10+ or Docker Desktop
2. Verify Docker Compose v2 is available:
   ```bash
   docker compose version
   ```

### Usage

```bash
# Build Docker image
make -f Makefile.docker docker-build

# Interactive shell in container
make -f Makefile.docker docker-shell

# Build Lites inside container
make -f Makefile.docker docker-build-lites

# Run in QEMU
make -f Makefile.docker docker-run-qemu
```

### Docker Image Contents

- Debian Bookworm base
- GCC 12.x with multilib support (i686 and x86_64)
- MIG (Mach Interface Generator)
- QEMU system-i386 and system-x86_64
- CMake, Ninja, Make build systems
- OSF Mach 6.1 kernel headers
- ccache for build caching
- GDB with multiarch support
- Development utilities (vim, tmux, git, etc.)

## Source Directory Structure

### Legacy Source Tree (Optional)

Some build targets require the legacy Lites source tree. Set the `LITES_SRC_DIR` environment variable:

```bash
export LITES_SRC_DIR=/path/to/Items1/lites-1.1.u3
```

The legacy source tree is located in the `Items1/` directory if you have the complete archive.

### Modern Source Tree

The modernized source tree is in the repository root:

- `arch/` - Architecture-specific code
- `core/` - Core Lites server implementation
- `include/` - Header files
- `kern/` - Kernel support code
- `servers/` - Server implementations
- `libs/` - Shared libraries
- `tests/` - Test suites
- `docker/` - Docker environment and scripts
- `scripts/` - Build and utility scripts

## Installation Instructions

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    gcc-multilib g++-multilib \
    gcc-i686-linux-gnu g++-i686-linux-gnu \
    cmake ninja-build \
    git wget curl \
    gnumach-dev hurd-dev mig \
    clang clang-tidy clang-format \
    ccache gdb gdb-multiarch \
    qemu-system-x86 qemu-utils \
    python3 python3-pip \
    tmux vim

# Install pre-commit
python3 -m pip install --upgrade pre-commit
```

### Fedora

```bash
sudo dnf update
sudo dnf install -y \
    gcc gcc-c++ \
    gcc-i686-linux-gnu gcc-c++-i686-linux-gnu \
    cmake ninja-build \
    git wget curl \
    gnumach-devel mig \
    clang clang-tools-extra \
    ccache gdb \
    qemu-system-x86 \
    python3 python3-pip \
    tmux vim

# Install pre-commit
python3 -m pip install --upgrade pre-commit
```

### macOS (via Homebrew + Docker)

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Docker Desktop from https://www.docker.com/products/docker-desktop

# Install development tools
brew install git cmake ninja python3

# Install pre-commit
python3 -m pip install --upgrade pre-commit
```

## Verification

Verify your installation with the setup script:

```bash
./scripts/setup-build-environment.sh
```

This will check for:
- Required build tools
- Mach kernel headers
- Source directory structure
- Environment configuration

## CI/CD Requirements

### GitHub Actions

The repository includes GitHub Actions workflows that automatically:

1. **build.yml** - Build and test on pull requests
   - Installs all dependencies
   - Sets up Mach headers automatically
   - Builds with GCC and Clang
   - Runs tests
   - Validates code formatting

2. **docker-i386-ci.yml** - Docker-based i386 CI/CD
   - Builds Docker image with complete toolchain
   - Validates Docker environment
   - Tests i386 builds
   - Runs integration tests

### Local Testing

Test the CI/CD pipeline locally:

```bash
# Run pre-commit checks
pre-commit run --all-files

# Build with Makefile
make -f Makefile.new ARCH=i686 CC=i686-linux-gnu-gcc

# Build with CMake
cmake -B build -DARCH=i686
cmake --build build

# Run tests
make -C tests/audit
./tests/audit/test_audit
```

## Troubleshooting

### "Mach headers not found"

Run the setup script:
```bash
./scripts/setup-build-environment.sh
source .env.build
```

Or use the Docker environment:
```bash
make -f Makefile.docker docker-shell
```

### "LITES_SRC_DIR not set"

This is optional for most builds. If needed:
```bash
export LITES_SRC_DIR=/path/to/Items1/lites-1.1.u3
```

### "clang-tidy failed"

Ensure the compilation database exists:
```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### Pre-commit hooks failing

Install and update pre-commit:
```bash
python3 -m pip install --upgrade pre-commit
pre-commit install
pre-commit autoupdate
```

## Additional Resources

- **Docker Documentation**: `docker/README.md`
- **CI/CD Workflow Guide**: `docker/CI_CD_WORKFLOW.md`
- **GitHub Actions Guide**: `docker/GITHUB_ACTIONS.md`
- **Mach Headers Setup**: `docker/MACH_HEADERS.md`
- **Getting Started**: `docker/GETTING_STARTED.md`

## Support

For issues or questions:
1. Check the documentation in the `docker/` directory
2. Review existing GitHub Issues
3. Create a new issue with detailed information about your environment and the problem
