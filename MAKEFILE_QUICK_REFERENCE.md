# Makefile System Quick Reference

**Status**: Active audit complete. This guide summarizes key findings.

---

## Which Build System Should I Use?

### For Production / Recommended

```bash
# Modern, standard, cross-platform
cmake -B build -DARCH=x86_64
cmake --build build
cd build && ctest                    # Run tests
```

**Why**: CMakeLists.txt is the canonical modern build system. Use this for:
- Production builds
- IDE integration (CLion, VS Code, etc.)
- Cross-platform development
- Test integration (ctest)

### For Quick Development

```bash
# Simple, single-command alternative
make -f Makefile.new ARCH=x86_64
make -f Makefile.new test
make -f Makefile.new clean
```

**Why**: Makefile.new is simpler for rapid development cycles. Use this for:
- Quick iterative builds
- Simple testing
- No IDE available
- Backward compatibility

### Legacy (DO NOT USE)

```bash
make                                 # BROKEN - don't use
```

---

## Critical Issues Found

| Issue | Impact | Status | Action |
|-------|--------|--------|--------|
| **core/ files (37 .c)** | Orphaned, not built | UNCLEAR | Investigate & clarify status |
| **Empty kern/, posix/, libos/** | Broken build references | LOW | Remove or populate directories |
| **bootstrap/ not integrated** | Standalone components | LOW | Create dedicated target if needed |
| **IOMMU duplication** | Possible duplicate code | LOW | Consolidate sources |

---

## File Coverage Summary

### BUILT by Current System
- **servers/** (all personality servers via SRCDIR)
- **core/iommu/** (IOMMU support: 2 files)
- **libs/** (POSIX, crypto, liblites, ddekit: 20+ files)
- **Total**: 100+ files successfully built

### NOT BUILT (Orphaned)
- **core/*.c** (37 files - status unclear, only iommu built)
- **bootstrap/** (25+ files - standalone)
- **drivers/** (2 files - possible duplicates)
- **kern/, posix/, libos/** (directories empty or non-existent)
- **Total**: 65+ orphaned files

---

## Build Commands Reference

### CMake (Recommended)

```bash
# Configure and build
cmake -B build -DARCH=x86_64
cmake --build build

# Specific features
cmake -B build -DARCH=i686                      # 32-bit
cmake -B build -DENABLE_CLANG_TIDY=ON         # Static analysis
cmake -B build -DSANITIZE=1                    # Memory safety
cmake -B build -DLITES_BUILD_TESTS=ON          # Include tests

# Run tests
cd build && ctest
cd build && ctest -V                           # Verbose
cd build && ctest -R vm_fault                  # Specific test

# Clean
cmake --build build --target clean
rm -rf build                                   # Full clean

# Install
cmake --install build --prefix /usr/local
```

### Makefile.new

```bash
# Build
make -f Makefile.new                           # Default (all targets)
make -f Makefile.new ARCH=x86_64               # Specific architecture
make -f Makefile.new TIDY=1                    # Run static analysis
make -f Makefile.new SANITIZE=1                # AddressSanitizer
make -f Makefile.new SRCDIR=custom/path        # Custom source

# Test
make -f Makefile.new test

# Static analysis
make -f Makefile.new tidy

# Clean
make -f Makefile.new clean
```

---

## Environment Variables

### Required
```bash
export LITES_MACH_DIR=/path/to/openmach       # Mach headers required
```

### Optional
```bash
export LITES_MACH_LIB_DIR=/path/to/openmach/lib
export LITES_SRC_DIR=/custom/source
export ARCH=x86_64                             # Default: x86_64
```

### Build Flags
```bash
export CFLAGS="-O3 -march=native"              # Optimization
export LDFLAGS="-Wl,-z,relro,-z,now"          # Security hardening
```

---

## Troubleshooting

### "mach/mach.h not found"

```bash
# Fix 1: Set environment
export LITES_MACH_DIR=/path/to/openmach

# Fix 2: Clone as submodule
git submodule add https://github.com/osfmk/openmach.git

# Fix 3: Use setup script
./scripts/setup-build-environment.sh
```

### "C23 not supported"

Makefile.new and CMakeLists.txt automatically fall back to C2x.

For CMake override:
```bash
cmake -B build -DCMAKE_C_FLAGS="-std=c2x"
```

### Build succeeds but no binary

Check if binary was created:
```bash
# For CMakeLists.txt
ls -la build/lites_server build/lites_emulator

# For Makefile.new
ls -la lites_server lites_emulator
```

If missing, check:
1. Mach headers installed correctly
2. Source tree exists (SRCDIR defaults to archive)
3. ARCH variable set correctly

---

## What Gets Built By Each System

### CMakeLists.txt (Comprehensive)
```
lites_server     - Main Unix server binary
lites_emulator   - User-space test harness
libposix.a       - POSIX API library
libcrypto.a      - Cryptography support
liblites.a       - Mach compatibility
libddekit.a      - Device driver kit
tests/*/test_*   - Unit and integration tests
```

### Makefile.new (Subset)
```
lites_server     - Main Unix server binary
lites_emulator   - User-space test harness
(libraries not built separately)
```

### Legacy Makefile (BROKEN)
```
(Nothing - ODE make rules not available)
```

---

## Known Coverage Gaps

### Gap 1: core/*.c Files (37 files)
These files are NOT built by default:
```
alloc.c, audit.c, auth.c, bcmp.c, clock.c, fio.c, iget.c, insque.c,
ipc_kobject.c, ipc_mig.c, ipc_queue.c, kalloc.c, main.c, malloc.c,
nami.c, pipe.c, prf.c, protocols.c, protTbl.c, ptblData.c, ptblData.o2.c,
ptbl_static.c, qsort.c, rdwri.c, remque.c, sched.c, sig.c, slp.c, subr.c,
sys1.c, sys2.c, sys3.c, sys4.c, sysent.c, text.c, traceLevels.c, trap.c
```

**Status**: Unclear if these are:
- Legacy cruft (should delete)
- Misplaced files (should move)
- Actively needed (should build)

**Action**: Run analysis to classify these files.

### Gap 2: bootstrap/ Components (25+ files)
Bootstrap loader not integrated into main build system.

**Status**: Standalone components, only needed if creating bootable images.

**Action**: Create dedicated CMake target if needed.

### Gap 3: drivers/iommu/ Duplication
```
drivers/iommu/iommu.c       (unclear if used)
core/iommu/iommu.c          (USED in build)
drivers/iommu/iommu_v2.c    (unclear if used)
```

**Status**: Possible duplicates, needs consolidation.

**Action**: Verify and consolidate to single source.

---

## Documentation References

| Document | Purpose |
|----------|---------|
| README.md | Quick start and overview |
| CLAUDE.md | Project standards and workflow |
| docs/setup.md | Detailed build environment setup |
| MAKEFILE_AUDIT_REPORT.md | Comprehensive audit findings |
| CMakeLists.txt | Modern build configuration |
| Makefile.new | Alternative simple build |

---

## Key Statistics

```
Total C files in repo:        3,090+ (many are test/archived)
Files actively being built:   100+
Files orphaned/status unknown: 65+
Build system options:         3 (1 broken, 1 active, 1 recommended)
Supported architectures:      6 (x86_64, i686, arm, riscv64, powerpc, ia16)
```

---

## Next Steps

1. **Use CMakeLists.txt** for all new work
2. **Check build passes** with both systems
3. **Investigate core/ files** status (CRITICAL)
4. **Document findings** in ARCHITECTURE.md
5. **Update CI/scripts** to use CMakeLists.txt

---

**Last Updated**: 2025-11-14
**Audit Status**: COMPLETE
**Maintainer**: See CLAUDE.md for contacts

For detailed findings, see MAKEFILE_AUDIT_REPORT.md
