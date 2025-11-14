# Comprehensive Mach Microkernel Build Requirements for Lites

## Table of Contents
1. [Executive Summary](#executive-summary)
2. [Required Tooling](#required-tooling)
3. [Build Techniques for Lites](#build-techniques-for-lites)
4. [Tool Installation Methods](#tool-installation-methods)
5. [Known Issues and Solutions](#known-issues-and-solutions)
6. [Version Compatibility Matrix](#version-compatibility-matrix)
7. [References](#references)

---

## Executive Summary

Building Lites (a 4.4BSD Unix server running on the Mach microkernel) requires specific tools and headers from the Mach ecosystem. This guide consolidates research from historical documentation, modern build systems, and practical implementations to provide a complete build environment setup.

**Key Finding**: Most required tools are available through package managers (Debian/Ubuntu) but NOT in Arch Linux repositories. Arch users must build from source or use containerized environments.

---

## Required Tooling

### 1. Mach Interface Generator (MIG)

**What it is**: An IDL (Interface Definition Language) compiler that generates C code from Mach interface definitions (.defs files).

**Latest Version**: GNU MIG 1.8 (as of 2025)

**Purpose**: Required to compile GNU Mach, GNU Hurd, and Lites distributions. Processes .defs files into stub code for IPC.

**Build Requirements for MIG**:
- C compiler (gcc or clang)
- Standard 32-bit C library with headers
- awk (gawk preferred)
- yacc (bison)
- lex (flex)
- make
- **GNU Mach header files** (prerequisite)

**Important Note**: MIG must be built as a 32-bit binary for proper i386/i686 development. On 64-bit systems, use:
```bash
./configure --target-arch=i386
```

### 2. Mach Kernel Headers

**Options** (choose one):

#### Option A: GNU Mach Headers (Modern Hurd Development)
- **Package**: gnumach-dev (Debian/Ubuntu)
- **Source**: https://www.gnu.org/software/gnumach/
- **Version**: Based on Mach 4 from University of Utah

#### Option B: OSF Mach 6.1 Headers (Recommended for Lites)
- **Source**: https://github.com/nmartin0/osfmk6.1
- **Description**: OSF Mach 6.1 - Open Software Foundation's Mach 3.0 microkernel
- **License**: BSD-style (unencumbered)
- **Used in**: Current Lites Docker development environment

#### Option C: OpenMach/Mach4 Headers
- **Source**: https://github.com/openmach/mach4
- **Description**: Mach4 archived source (UK22 release from 1996)
- **Historical**: Based on Utah's Mach 4

#### Option D: xMach Headers
- **Source**: https://github.com/neozeed/xMach
- **Description**: Mach4 + Lites from SourceForge CVS
- **Note**: Includes updated configuration files for modern builds

**Current Lites Project Configuration**:
The repository automatically detects headers in this order:
1. `$LITES_MACH_DIR` environment variable
2. `/opt/mach/osfmk/kernel/src` (Docker image default)
3. `localmach/include` (from setup script)
4. `openmach/include` (git submodule)

### 3. Cross-Compilation Toolchain

**Recommended Setup** (for i386/i686 Lites builds):

#### Core Tools:
- **GCC**: gcc 2.7.2.3 (historical) or modern gcc-multilib
- **Binutils**: 2.12.1 or 2.9.x from OSKit
- **Target**: i586-linux or i686-linux

#### Modern Alternative (Docker Environment):
```bash
# Debian/Ubuntu packages
gcc-multilib g++-multilib
gcc-i686-linux-gnu g++-i686-linux-gnu
binutils-i686-linux-gnu
```

#### Configure for 32-bit Cross Compilation:
```bash
# Using native 64-bit compiler in 32-bit mode
./configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386'

# Using dedicated cross-compiler
./configure --host=i686-gnu CC=i686-linux-gnu-gcc LD=i686-linux-gnu-ld
```

### 4. Additional Build Tools

| Tool | Purpose | Source |
|------|---------|--------|
| autoconf | Build system configuration | Package manager |
| automake | Makefile generation | Package manager |
| libtool | Shared library support | Package manager |
| pkg-config | Dependency detection | Package manager |
| flex | Lexical analyzer generator | Package manager |
| bison | Parser generator | Package manager |
| gawk | GNU awk (pattern processing) | Package manager |
| make | Build automation | Package manager |
| cmake | Modern build system | Package manager |
| ninja | Fast build backend | Package manager |

### 5. Optional Libraries

**libmach**: The CMU UX server-specific library
- **Variants**:
  - libmach.a: CMU USER distribution
  - libmach_sa.a: Personality-neutral standalone (used by Lites/Hurd)
- **Modern Status**: Largely superseded by Mach support in GNU C Library
- **Enable**: Use `--enable-libmach` configure option

---

## Build Techniques for Lites

### Historical Build (Original xMach Method)

**Requirements**:
- i586-linux cross toolchain (gcc 2.7.2.3 + binutils 2.12.1 or OSKit binutils)
- MIG (32-bit build)
- Updated configuration files

**Critical Steps**:

1. **Configure Kernel**:
```bash
mkdir kernel-build
cd kernel-build
../kernel/configure \
  --host=i586-linux \
  --target=i586-linux \
  --build=i586-linux \
  --enable-elf \
  --enable-libmach \
  --enable-linuxdev \
  --prefix=/usr/local/xmach

# CRITICAL: Copy fixed Makeconf
cp ../updated-conf/kernel/Makeconf .
```

2. **Configure Lites**:
```bash
mkdir lites-build
cd lites-build
../lites/configure \
  --host=i586-linux \
  --target=i586-linux \
  --build=i586-linux \
  --enable-mach4 \
  --prefix=/usr/local/xmach \
  --with-mach4=../kernel \
  LITES_CONFIG=LARGE

# CRITICAL: Copy fixed config
cp ../updated-conf/lites/Makeconf .
```

**WARNING**: The `configure` script does NOT populate compiler tools correctly. You MUST copy/fix the Makeconf file or the build will be corrupted. If corrupted, the ONLY fix is to remove the build directory and start over.

### Modern Build (Current Lites Repository)

**Using CMake** (Recommended):
```bash
# Set Mach headers location
export LITES_MACH_DIR=/path/to/mach/headers

# Configure with Ninja generator
cmake -G Ninja -B build \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_LINKER=lld \
  -DLITES_MACH_DIR="${LITES_MACH_DIR}"

# Build
ninja -C build
```

**Using Makefile.new**:
```bash
export LITES_MACH_DIR=/path/to/mach/headers
make -f Makefile.new ARCH=i686
```

### Docker-Based Build (Easiest)

The repository includes a complete Docker environment with all dependencies pre-installed:

```bash
# Build Docker image and compile Lites for i386
make -f Makefile.docker docker-all

# Run in QEMU i386 instance
make -f Makefile.docker docker-run-qemu

# Enter interactive development container
make -f Makefile.docker docker-shell
```

**Docker Environment Includes**:
- Complete i386 toolchain (gcc-multilib)
- MIG from Debian packages
- QEMU i386 emulation with KVM
- OSF Mach 6.1 headers pre-installed at /opt/mach/osfmk
- All build scripts pre-configured

---

## Tool Installation Methods

### On Debian/Ubuntu (Recommended)

**Complete Development Environment**:
```bash
# Update system
sudo apt update
sudo apt dist-upgrade -y

# Install Mach development packages
sudo apt install -y \
  gnumach-dev \
  hurd-dev \
  mig

# Install toolchain
sudo apt install -y \
  build-essential \
  gcc-multilib \
  g++-multilib \
  cmake \
  ninja-build \
  autoconf \
  automake \
  libtool \
  pkg-config \
  flex \
  bison \
  gawk

# Install cross-compilers
sudo apt install -y \
  gcc-i686-linux-gnu \
  g++-i686-linux-gnu \
  binutils-i686-linux-gnu

# Install QEMU for testing
sudo apt install -y \
  qemu-system-x86 \
  qemu-utils
```

**MIG Version in Debian**:
- Bullseye (oldstable): mig 1.8+git20200618-5
- Bookworm (stable): mig 1.8+git20221221-3
- Trixie/Sid (testing/unstable): mig 1.8+git20231217-5

### On Arch Linux

**Status**: NO official packages or AUR packages for GNU MIG (Mach Interface Generator)

**Note**: The AUR package `mig-console-git` is Mozilla's MIG (Mozilla InvestiGator), NOT the Mach Interface Generator.

**Options for Arch Users**:

#### Option 1: Build MIG from Source
```bash
# Install build dependencies
sudo pacman -S base-devel gcc flex bison gawk make

# Clone GNU MIG
git clone https://git.savannah.gnu.org/git/hurd/mig.git
cd mig

# Configure and build (32-bit)
./autogen.sh
./configure --prefix=/usr/local --target-arch=i386
make
sudo make install
```

**Prerequisites**: You MUST have GNU Mach headers installed before building MIG.

#### Option 2: Use Docker (Recommended)
```bash
# Use the provided Debian-based Docker environment
cd /path/to/lites
make -f Makefile.docker docker-shell
```

#### Option 3: Create PKGBUILD for AUR

A PKGBUILD for GNU MIG could be created based on this template:

```bash
# pkgname=gnumig
# pkgver=1.8
# pkgrel=1
# pkgdesc="GNU Mach Interface Generator - IDL compiler for Mach IPC"
# arch=('i686' 'x86_64')
# url="https://www.gnu.org/software/hurd/microkernel/mach/mig.html"
# license=('GPL')
# depends=('glibc' 'gawk' 'flex' 'bison')
# makedepends=('git' 'autoconf' 'automake')
# source=("git+https://git.savannah.gnu.org/git/hurd/mig.git#tag=v${pkgver}")
```

### Installing Mach Headers

#### Method 1: Clone OSF Mach 6.1 (Recommended for Lites)
```bash
git clone --depth 1 https://github.com/nmartin0/osfmk6.1.git openmach
export LITES_MACH_DIR="$(pwd)/openmach/kernel/src"
```

#### Method 2: Use Debian gnumach-dev Package
```bash
sudo apt install gnumach-dev
# Headers installed to /usr/include/mach/
```

#### Method 3: Clone OpenMach/Mach4
```bash
git clone https://github.com/openmach/mach4.git openmach
export LITES_MACH_DIR="$(pwd)/openmach/kernel"
```

#### Method 4: Use Lites Import Script
```bash
# Populates localmach/include from bundled xkernel sources
cd /path/to/lites
./scripts/import-mach-headers.sh
export LITES_MACH_DIR="$(pwd)/localmach/include"
```

---

## Known Issues and Solutions

### Issue 1: Makeconf Configuration Corruption

**Problem**: Configure script does not populate compiler tools correctly in Makeconf.

**Symptom**: Build fails with missing compiler or incorrect paths.

**Solution**:
- For xMach builds: ALWAYS copy the corrected Makeconf from `updated-conf/` directory
- If build is corrupted: Delete entire build directory and reconfigure from scratch
- Make configure scripts executable: `chmod +x kernel/c* lites/conf*`

**Impact**: Critical - will cause build failure

### Issue 2: MIG 64-bit Incompatibility

**Problem**: MIG does not work properly in 64-bit environments for i386 targets.

**Symptom**: Generated stub code has incorrect sizes or alignment.

**Solution**:
- Build MIG with `--target-arch=i386`
- Use 32-bit usermode environment
- Configure with: `CC=i686-linux-gnu-gcc MIG=i686-linux-gnu-mig`

**Impact**: High - affects all interface code generation

### Issue 3: GNU C Library (glibc) Build Failures

**Problem**: glibc may fail to build due to missing Mach header directory references.

**Symptom**: Compiler cannot find Mach headers during glibc compilation.

**Solution**:
- Point glibc to additional include directory: `CFLAGS=-I/path/to/mach4-i386/include`
- Use `--with-headers` configure option
- May require disk space (glibc build can fail due to insufficient space)

**Impact**: Medium - only affects full system builds

### Issue 4: PA-RISC Assembly Parser Problems

**Problem**: Assembler parsing error with '!' as line separator in PA-RISC code.

**Symptom**: Build fails on PA-RISC target with assembly syntax errors.

**Solution**:
- Use patched binutils
- Modify assembly to use different line separator
- Target i386/i686 instead (better supported)

**Impact**: Low - only affects PA-RISC architecture

### Issue 5: Permission Denied Errors

**Problem**: Build scripts not executable.

**Symptom**: "./configure: Permission denied"

**Solution**:
```bash
chmod +x kernel/c*
chmod +x lites/conf*
```

**Impact**: Low - easy fix

### Issue 6: Missing Dependencies in Cross-Build Environment

**Problem**: Cross-compilation requires specific tool prefixes not in PATH.

**Symptom**: "i586-linux-gcc: command not found"

**Solution**:
```bash
# Add cross-toolchain to PATH
export PATH=/usr/local/i586-linux2/bin:$PATH
export PATH=/usr/local/i586-linux2/lib/gcc-lib/i586-linux/2.7.2.3:$PATH
```

**Impact**: High - blocks build start

### Issue 7: Context Overflow in Large Directories

**Problem**: Recursive listings of large directories (like node_modules) can overwhelm context.

**Symptom**: Slow operations, excessive output

**Solution**:
- Use limited depth: `make list-directory` limits nested dirs to 100 items
- Add directories to .gitignore
- Use specific file searches instead of recursive scans

**Impact**: Low - performance/convenience issue

---

## Version Compatibility Matrix

### Tested Toolchain Combinations

| Component | Version | Target Arch | Status | Notes |
|-----------|---------|-------------|--------|-------|
| GCC | 2.7.2.3 | i586-linux | Historical | Original xMach toolchain |
| GCC | 4.8.2 | i686-pc-gnu | Tested | Arch Hurd Project |
| GCC | Modern multilib | i686/i386 | Working | Current Docker env |
| Binutils | 2.9.1 | i386 | Historical | OSKit recommendation |
| Binutils | 2.12.1 | i586-linux | Working | xMach compatible |
| MIG | 1.8 | i386 | Current | Latest GNU release |
| GNU Mach | 1.8 | i386 | Current | Compatible with MIG 1.8 |
| OSF Mach | 6.1 | i386 | Working | Used in Lites Docker |
| Mach4 | UK22 | i386 | Historical | 1996 Utah release |

### Supported Build Platforms

| Host OS | Architecture | MIG Source | Mach Headers | Status |
|---------|--------------|------------|--------------|--------|
| Debian Bookworm | x86_64 | Package (mig) | Package (gnumach-dev) | ✓ Supported |
| Ubuntu 20.04+ | x86_64 | Package (mig) | Package (gnumach-dev) | ✓ Supported |
| Arch Linux | x86_64 | Build from source | Clone from GitHub | ○ Manual setup |
| Docker (Debian) | Any | Package (mig) | Pre-installed | ✓ Recommended |

### Architecture Targets

| Target | Toolchain | Mach Support | Lites Status | Complexity |
|--------|-----------|--------------|--------------|------------|
| i386 | Complete | Full | Working | Low |
| i686 | Complete | Full | Working | Low |
| x86_64 | Partial | Limited | Experimental | Medium |
| ARM | Experimental | Limited | Untested | High |
| PowerPC | Historical | Limited | Untested | High |
| PA-RISC | Historical | Buggy | Broken | Very High |
| RISC-V | Modern | None | Future | Very High |

---

## References

### Official Documentation

1. **GNU MIG Official Page**
   - https://www.gnu.org/software/hurd/microkernel/mach/mig.html
   - https://www.gnu.org/software/hurd/microkernel/mach/mig/gnu_mig/building.html

2. **GNU Mach Official Page**
   - https://www.gnu.org/software/gnumach/
   - https://www.gnu.org/software/hurd/microkernel/mach/gnumach/building.html

3. **CMU Mach Documentation**
   - http://www.cs.cmu.edu/afs/cs.cmu.edu/project/mach/public/www/mach.html
   - http://www.cs.cmu.edu/afs/cs/project/mach/public/doc/unpublished/mig.ps

### Source Code Repositories

4. **OSF Mach 6.1 (Recommended)**
   - https://github.com/nmartin0/osfmk6.1
   - Alternative versions: osfmk7.3, mach-mk74, mach4ALPHA

5. **OpenMach / Mach4**
   - https://github.com/openmach/mach4
   - https://github.com/openmach/openmach
   - https://www-old.cs.utah.edu/flux/mach4/html/Mach4-proj.html

6. **xMach (Mach4 + Lites)**
   - https://github.com/neozeed/xMach
   - https://sourceforge.net/projects/xmach/

### Historical Documentation

7. **University of Utah Flux Project**
   - https://www.cs.utah.edu/flux/lites/html/
   - https://www.cs.utah.edu/flux/mach4/html/
   - https://www-old.cs.utah.edu/flux/mach4/html/install.html

8. **Lites Historical Links**
   - http://web.archive.org/web/20031205031618/http://www.cs.hut.fi/~jvh/lites.html
   - http://ftp.funet.fi/pub/mach/lites/
   - http://ftp.lip6.fr/pub/mach/mach4/mach/lites/

### Build Guides and Tutorials

9. **VirtuallyFun Lites Guides**
   - https://virtuallyfun.com/2021/06/09/xmach/
   - https://virtuallyfun.com/category/lites/

10. **Mach on PA-RISC**
    - https://www.openpa.net/mach_3_mach_4-lites_utah_pa-risc.html

### Package Sources

11. **Debian Packages**
    - https://packages.debian.org/search?keywords=mig
    - https://packages.debian.org/bullseye/mig
    - https://packages.debian.org/bullseye/gnumach-dev

12. **Arch Linux Resources**
    - https://aur.archlinux.org/ (AUR home)
    - https://wiki.archlinux.org/title/GNU_Compiler_Collection

### Academic Papers

13. **Lites Design Thesis**
    - Johannes Helander: "Unix Under Mach: The Lites Server"
    - Available in repository: `johannes_helander-unix_under_mach-the_lites_server.pdf`

14. **Mach 3 Server Writer's Guide**
    - http://shakthimaan.com/downloads/hurd/server_writer.pdf

### Related Projects

15. **GNU Hurd**
    - https://www.gnu.org/software/hurd/
    - https://darnassus.sceen.net/~hurd-web/

16. **Apple XNU (OSF Mach derivative)**
    - https://github.com/opensource-apple/xnu
    - https://github.com/slp/osfmk-mklinux

---

## Quick Start Checklist

### For Debian/Ubuntu Users:
- [ ] Install mig package: `sudo apt install mig`
- [ ] Install gnumach-dev: `sudo apt install gnumach-dev`
- [ ] Install gcc-multilib: `sudo apt install gcc-multilib g++-multilib`
- [ ] Clone Lites repository
- [ ] Configure CMake with LITES_MACH_DIR
- [ ] Build with ninja

### For Arch Linux Users:
- [ ] Install base-devel: `sudo pacman -S base-devel`
- [ ] Clone GNU MIG source and build manually
- [ ] Clone OSF Mach headers from GitHub
- [ ] Set LITES_MACH_DIR environment variable
- [ ] **OR** Use Docker environment (recommended)

### For Docker Users (Any OS):
- [ ] Install Docker and docker-compose
- [ ] Clone Lites repository
- [ ] Run: `make -f Makefile.docker docker-all`
- [ ] Everything is pre-configured

---

## Appendix: Common Build Commands

### Building MIG from Source
```bash
# Clone MIG
git clone https://git.savannah.gnu.org/git/hurd/mig.git
cd mig

# Generate configure script
./autogen.sh

# Configure for 32-bit (if on 64-bit host)
./configure --prefix=/usr/local --target-arch=i386 \
  TARGET_CPPFLAGS=-I/path/to/mach/headers

# Build and install
make
sudo make install
```

### Setting Up Cross-Compilation Environment
```bash
# Create i586-linux toolchain directory structure
mkdir -p /usr/local/i586-linux2/{bin,lib,include}

# Build binutils
./configure --target=i586-linux --prefix=/usr/local/i586-linux2
make
sudo make install

# Build GCC (after binutils)
./configure --target=i586-linux --prefix=/usr/local/i586-linux2 \
  --with-gnu-as --with-gnu-ld --disable-libgcc
make
sudo make install

# Add to PATH
export PATH=/usr/local/i586-linux2/bin:$PATH
```

### Verifying Installation
```bash
# Check MIG version
mig --version

# Check for Mach headers
ls ${LITES_MACH_DIR}/mach/*.h

# Test cross-compiler
i686-linux-gnu-gcc --version
i686-linux-gnu-gcc -m32 -march=i386 -E - < /dev/null

# Verify MIG can process .defs files
mig /usr/share/doc/mig/examples/test.defs
```

---

**Document Version**: 1.0
**Last Updated**: 2025-11-14
**Maintainer**: Lites Development Team
**License**: BSD (same as Lites project)
