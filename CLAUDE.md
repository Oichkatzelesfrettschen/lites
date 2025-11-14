# LITES PROJECT MEMORY (Team Standards)

**Version**: 2025-11-13
**Scope**: Lites project team (versioned with repository)
**Policy**: ASCII only, warnings as errors, reproducible builds

---

## PROJECT OVERVIEW

Lites is a Unix server running atop the Mach microkernel, providing BSD 4.4-Lite compatibility. This modernized version targets contemporary hardware with CMake build system, C23 standard, and comprehensive code quality tooling.

**Repository**: ~/Playground/lites
**Primary Branch**: master
**Build System**: CMake (canonical), legacy Makefiles archived
**Language**: C23 (fallback: C2x) with GCC 13+ or Clang 17+

---

## BUILD INSTRUCTIONS

### Quick Start

```bash
# 1. Install Mach headers (required)
export LITES_MACH_DIR=/path/to/openmach
# OR place openmach/ at repository root
# OR follow docs/setup.md for offline setup

# 2. Build with CMake (canonical)
cmake -B build -DARCH=x86_64
cmake --build build

# 3. Alternative: Makefile.new (development/testing)
make -f Makefile.new ARCH=x86_64

# 4. Run tests
cd build && ctest
```

### Architecture Support

- **x86_64**: Primary target (64-bit)
- **i686**: 32-bit x86 (use -DARCH=i686)
- **arm, riscv64, powerpc, ia16**: Experimental

### Build Targets

- `lites_server`: Main Unix server binary
- `lites_emulator`: User-space emulation testbed
- `tests`: Unit and integration tests

---

## CODE QUALITY STANDARDS

### Compiler Warnings: Treat as Errors

**WHY**: Prevent drift, catch bugs early, maintain consistency.

**WHAT**: All compiler warnings, linter issues, and pre-commit checks must pass.

**HOW**:
```bash
# CMake enforces: -Wall -Wextra -Werror -pedantic
cmake -B build
cmake --build build   # Will fail on any warning

# Pre-commit hooks
pre-commit install    # Runs clang-format + clang-tidy
```

### Code Formatting

**Standard**: `.clang-format` (LLVM base, 4-space indent, 100 col limit)

```bash
# Format all tracked files
scripts/format-code.sh

# Or manual
clang-format -i file.c
```

### Static Analysis

**Tools**: `.clang-tidy` (bugprone, performance, clang-analyzer checks)

```bash
# Run on all sources
make -f Makefile.new tidy

# Or specific file
clang-tidy file.c -- -Iinclude -Icore/iommu
```

---

## PROJECT STRUCTURE

```
lites/
├── core/               # Kernel-level core (allocation, IPC, scheduling, iommu)
├── servers/            # POSIX, NetBSD, SunOS personalities
├── libs/               # Shared libraries (libposix, libc, libmach)
├── drivers/            # Hardware drivers (serial, keyboard, framebuffer, disk, net)
├── include/            # Public headers (mach/, posix.h, iommu.h, etc.)
├── examples/           # Example programs and demos
├── tests/              # Unit, integration, userspace tests
├── scripts/            # Build, analysis, and utility scripts
│   ├── analysis/       # dependency_graph.sh, inventory.sh, functional_map.sh
│   └── reorg/          # Reorganization automation
├── docs/               # Documentation (INDEX.md is navigation hub)
├── cmake/              # CMake modules and configuration
├── docker/             # Docker-based i386 development environment
├── flattened/          # Alternative directory layout (reference, not built)
├── legacy_bsd/         # BSD compatibility layer
├── Items1/             # Historical releases (Lites 1.0, 1.1.u3) - for archaeology
└── archives/           # Legacy builds and deprecated code
```

### Key Directories Explained

- **core/**: Actively built kernel components (37 C files being reorganized into subdirs)
- **servers/**: Separate POSIX/NetBSD/SunOS server binaries
- **flattened/**: Design reference for future reorganization (exokernel, microkernel, libos, userspace)
- **Items1/**: Historical code, not built by default (15 MB archive)

---

## DEVELOPMENT WORKFLOW

### 1. Feature Branches

```bash
git checkout -b feature/your-feature-name
# Work, commit incrementally
git push -u origin feature/your-feature-name
```

### 2. Pre-commit Checks

```bash
pre-commit run --all-files   # Before committing
```

### 3. Pull Request Requirements

- **Green CI**: All tests and builds pass
- **No warnings**: Compiler warnings treated as errors
- **Formatted**: `clang-format` applied
- **Documented**: Update relevant docs/
- **Tested**: Unit tests for new functionality

### 4. Commit Messages

**Format**: Conventional Commits

```
feat(iommu): Add basic IOMMU support infrastructure
fix(build): Resolve CMakeLists.txt Mach header detection
docs(setup): Clarify offline package installation
refactor(core): Organize 37 loose files into subdirectories
```

---

## DEPENDENCIES & SETUP

### Required

- **Mach Headers**: OpenMach/Mach4 distribution (see docs/setup.md)
- **Compiler**: GCC 13+ or Clang 17+ (C23 support)
- **CMake**: 3.15+
- **Python**: 3.8+ (for scripts)

### Optional

- **clang-format**: Code formatting (LLVM 17+)
- **clang-tidy**: Static analysis
- **pre-commit**: Automated hooks
- **Docker**: i386 development environment (see docker/)
- **QEMU**: Emulation and testing

### Environment Variables

```bash
export LITES_MACH_DIR=/path/to/openmach       # Mach headers
export LITES_MACH_LIB_DIR=/path/to/openmach/lib  # Prebuilt libs (optional)
export LITES_SRC_DIR=/custom/path             # Override source location
```

---

## TESTING

### Unit Tests

```bash
cd build
ctest                      # Run all tests
ctest -R vm_fault          # Run specific test
ctest --verbose            # Detailed output
```

### Integration Tests

See `tests/integration/README.md` for test organization and strategy.

### QEMU Testing

```bash
# Build and run in QEMU
scripts/run-qemu.sh x86_64

# With tmux (build on left, QEMU on right)
scripts/tmux-qemu.sh x86_64
```

---

## DOCUMENTATION

### Navigation

- **docs/INDEX.md**: Main documentation hub
- **docs/setup.md**: Build environment setup
- **docs/PROJECT_PLAN.md**: Modernization roadmap and phases
- **README.md**: Quick start and overview

### Analysis Scripts

```bash
# Subsystem dependency graph
scripts/analysis/dependency_graph.sh

# Functional decomposition
scripts/analysis/functional_map.sh

# File origin classification (BSD/Mach/Lites)
scripts/analysis/inventory.sh
```

---

## COMMON ISSUES

### 1. Mach Headers Not Found

**Symptom**: `fatal error: mach/mach.h: No such file or directory`

**Fix**:
```bash
# Option A: Set environment variable
export LITES_MACH_DIR=/path/to/openmach

# Option B: Clone as submodule
git submodule add https://github.com/osfmk/openmach.git

# Option C: Use setup script
./scripts/setup-build-environment.sh
```

### 2. C23 Not Supported

**Symptom**: `error: unrecognized command line option '-std=c23'`

**Fix**: Makefile.new auto-detects and falls back to `-std=c2x`. For CMake:
```bash
cmake -B build -DCMAKE_C_FLAGS="-std=c2x"
```

### 3. Executable Stack Warning in QEMU

**Symptom**: `warning: executable stack`

**Fix**: Link with noexecstack flag (already in Makefile.new):
```bash
cmake -B build -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,noexecstack"
```

---

## RECENT MAJOR CHANGES (2025-11-13)

### Maximal Branch Synthesis

**Merged 6 branches** with comprehensive conflict resolution:

1. **remove-__p** (139 files): Removed K&R `__P()` macros, modernized to ANSI C
2. **update-noreturn** (5 files): Standard noreturn annotations
3. **openmach-docs** (1 file): Clarified Mach dependency documentation
4. **clang-tidy** (6 conflicts): Added code quality tooling infrastructure
5. **iommu** (4 conflicts): Basic IOMMU support (core/iommu/, include/iommu.h)
6. **restructure** (30 conflicts, 77 files): Analysis scripts, PROJECT_PLAN.md, flattened/ reference structure

### New Capabilities

- **Code Quality**: `.clang-format`, `.clang-tidy`, `.pre-commit-config.yaml`
- **IOMMU Support**: Basic IOMMU module integrated
- **Analysis Tools**: dependency_graph.sh, inventory.sh, functional_map.sh
- **Flattened Structure**: Reference layout for future reorganization

---

## BRANCHING STRATEGY

**Main Branch**: `master`
**Feature Branches**: `feature/*`
**Fix Branches**: `fix/*`
**Docs Branches**: `docs/*`

### Merge Policy

- Squash small fixes (1-3 commits)
- Merge large features with full history
- Always require PR review for master
- Green CI required before merge

---

## PERFORMANCE & OPTIMIZATION

### Address Sanitizer (Development)

```bash
make -f Makefile.new SANITIZE=1 ARCH=x86_64
# Runs with -fsanitize=address
```

### Multicore Scheduler (Experimental)

```bash
make -f Makefile.new MULTICORE_SCHED=1 ARCH=x86_64
# Enables CONFIG_SCHED_MULTICORE
```

---

## REFERENCES

- Helander Thesis: `johannes_helander-unix_under_mach-the_lites_server.pdf`
- Mach Documentation: https://www.gnu.org/software/hurd/microkernel/mach.html
- Build System: docs/BUILD_SYSTEM_MIGRATION.md (when created)
- POSIX API: docs/POSIX.md
- IPC Wrappers: docs/IPC.md

---

## CONTACTS & MAINTENANCE

**Cadence**:
- Weekly: Review open PRs, update dependencies
- Monthly: Refresh analysis scripts output
- Quarterly: Major dependency updates, roadmap review

**Priority fixes**: Build breaks, security issues, CI failures

---

**Last Updated**: 2025-11-13
**Next Review**: 2025-12-01

---

*For global Claude Code settings, see ~/.claude/CLAUDE.md*
*This file is versioned with the repository and shared with the team.*
