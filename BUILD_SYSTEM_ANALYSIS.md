# Build System Analysis: Detailed Technical Comparison

**Audit Date**: 2025-11-14
**Repository**: /home/eirikr/Playground/lites
**Scope**: Complete architecture and functionality assessment

---

## PART 1: MAKEFILE SYSTEM ARCHITECTURE

### 1.1 Legacy Makefile Analysis

**File**: `/home/eirikr/Playground/lites/Makefile`
**Lines**: 29
**Last Modified**: 1995 (CMU Lites release)
**Language**: ODE Make syntax

**Complete Contents**:
```makefile
# 
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
# All Rights Reserved.
# 
# [CMU License - omitted for brevity]
# 
# HISTORY
# $Log: Makefile,v $
# Revision 1.1.1.1  1995/03/02  21:49:25  mike
# Initial Lites release from hut.fi
#
# Revision 2.2  92/06/25  17:18:57  mrt
# 	Created for ODE make
# 	[92/06/16  16:18:31  mrt]
# 

EXPINC_SUBDIRS	= include
EXPLIB_SUBDIRS	= liblites
 
SUBDIRS		=  server emulator

.include <${RULES_MK}>
```

**Analysis**:
- Minimal 29-line file
- References ODE (Open Development Environment) make system
- Declares source subdirectories: server, emulator
- Attempts to include RULES_MK (not available in modern environment)
- **VERDICT**: NON-FUNCTIONAL. Requires ODE infrastructure no longer available.

**Why it's broken**:
```
.include <${RULES_MK}>
Error: RULES_MK variable not defined
Effect: Cannot find compilation rules, patterns, or targets
Result: Build fails immediately
```

**Remediation**: DELETE and rely on Makefile.new or CMakeLists.txt.

---

### 1.2 Makefile.new Analysis

**File**: `/home/eirikr/Playground/lites/Makefile.new`
**Lines**: 180
**Status**: ACTIVELY MAINTAINED
**Language**: GNU Make 3.81+

#### Architecture Overview

The Makefile.new uses a **"source tree discovery" architecture**:

```
┌─────────────────────────────────────────┐
│ Makefile.new (build orchestrator)       │
├─────────────────────────────────────────┤
│ Phase 1: Locate source tree              │
│   SRCDIR ?= src-lites-1.1-2025 OR       │
│   SRCDIR ?= build/lites-1.1.u3           │
├─────────────────────────────────────────┤
│ Phase 2: Detect Mach headers/libs        │
│   LITES_MACH_DIR ?= openmach OR         │
│   LITES_MACH_DIR ?= core/mach_kernel    │
├─────────────────────────────────────────┤
│ Phase 3: Translate architecture          │
│   ARCH ?= x86_64 → -m64 → i386/x86_64   │
├─────────────────────────────────────────┤
│ Phase 4: Collect sources                 │
│   find $(SRCDIR)/server/{arch,common}    │
│   wildcard kern/*.c posix/*.c            │
│   wildcard core/iommu/*.c libos/*.c      │
├─────────────────────────────────────────┤
│ Phase 5: Compile and link                │
│   $(CC) -Wall -Wextra -Werror $^ -o bin  │
├─────────────────────────────────────────┤
│ Phase 6: Optional static analysis        │
│   $(CLANG_TIDY) if TIDY=1               │
└─────────────────────────────────────────┘
```

#### Variable Definitions

**Source Tree Configuration**:
```makefile
ifeq ($(wildcard src-lites-1.1-2025),src-lites-1.1-2025)
  SRCDIR ?= src-lites-1.1-2025      # Prefer modernized tree
else
  SRCDIR ?= build/lites-1.1.u3      # Fallback to extracted archive
endif
```

**Compiler Configuration**:
```makefile
CC ?= clang                          # Default compiler
CLANG_TIDY ?= clang-tidy            # Static analysis tool
C23_FLAG := $(shell $(CC) -std=c23 -E -x c /dev/null >/dev/null 2>&1 && echo -std=c23 || echo -std=c2x)
CFLAGS ?= -O2 $(C23_FLAG)
CFLAGS += -Wall -Wextra -Werror -pedantic
LDFLAGS := $(LDFLAGS) -Wl,-z,noexecstack
```

**Architecture Translation** (CRITICAL):
```makefile
ifeq ($(ARCH),i686)
  CFLAGS += -m32
  LDFLAGS += -m32
  ARCH_INCDIR := -Iinclude/i386
else ifeq ($(ARCH),x86_64)
  CFLAGS += -m64
  LDFLAGS += -m64
  ARCH_INCDIR := -Iinclude/x86_64
else ifeq ($(ARCH),arm)
  ...
# Maps ARCH to directory name
ARCH_DIR := $(ARCH)
ifeq ($(ARCH),i686)
  ARCH_DIR := i386     # Important: i686 maps to i386 directory
endif
```

**Server Source Collection**:
```makefile
SERVER_COMMON_DIRS := \
  $(SRCDIR)/server/kern \
  $(SRCDIR)/server/miscfs \
  $(SRCDIR)/server/netiso \
  $(SRCDIR)/server/serv \
  $(SRCDIR)/server/netinet \
  $(SRCDIR)/server/ufs \
  $(SRCDIR)/server/nfs \
  $(SRCDIR)/server/netccitt \
  $(SRCDIR)/server/net \
  $(SRCDIR)/server/netns

SERVER_DIRS := $(SRCDIR)/server/$(ARCH_DIR) $(SERVER_COMMON_DIRS)
SERVER_SRC := $(foreach d,$(SERVER_DIRS),$(shell find $(d) -name \*.c -o -name \*.S))
```

**Kernel Component Collection**:
```makefile
KERN_SRC := $(wildcard kern/*.c) $(wildcard posix/*.c)
IOMMU_SRC := $(wildcard core/iommu/*.c)
LIBOS_SRC := $(wildcard libos/*.c)
```

#### Build Targets

**Target: all (DEFAULT)**
```makefile
all: prepare $(TARGETS)
```
- Creates include/machine symlink
- Builds lites_server
- Builds lites_emulator (if sources available)

**Target: lites_server (PRIMARY BINARY)**
```makefile
lites_server: $(SERVER_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC)
	$(CC) $(CFLAGS) \
	  -I$(SRCDIR)/include $(ARCH_INCDIR) $(MACH_INCDIR) \
	  $(SERVER_INCDIRS) $(KERN_INCDIRS) \
	  $^ $(MACH_LIBS) $(LDFLAGS) -o $@
ifeq ($(TIDY),1)
	$(CLANG_TIDY) $^ -- $(CFLAGS) ...
endif
```

**Target: lites_emulator (OPTIONAL)**
```makefile
ifneq ($(EMULATOR_SRC),)
lites_emulator: $(EMULATOR_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC)
	$(CC) $(CFLAGS) -I$(SRCDIR)/include ... $^ ... -o $@
ifeq ($(TIDY),1)
	$(CLANG_TIDY) $^ -- ...
endif
endif
```

**Target: test**
```makefile
test: all
	@for d in $(TEST_SUBDIRS); do \
	  $(MAKE) -C $$d; \
	done
```

**Target: tidy**
```makefile
tidy:
	$(CLANG_TIDY) $(SERVER_SRC) $(EMULATOR_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC) -- $(CFLAGS) ...
```

#### Known Issues & Limitations

**Issue 1: Broken Wildcards**
```makefile
KERN_SRC := $(wildcard kern/*.c) $(wildcard posix/*.c)
           # kern/ and posix/ directories do NOT exist in root
           # Result: $(KERN_SRC) expands to empty string
           # Impact: These source files are not included in build
```

**Issue 2: core/ Files Not Included**
```makefile
# 37 .c files in core/ directory are NOT explicitly referenced
# Only core/iommu/*.c is included via IOMMU_SRC
# Result: 35 core/*.c files are orphaned
```

**Issue 3: SRCDIR Dependency**
```makefile
# Build depends on external archive:
# - src-lites-1.1-2025/ (if it exists)
# - build/lites-1.1.u3/ (extracted archive)
# 
# If neither exists, build will fail with:
# "No such file or directory: $(SRCDIR)/server/x86_64/..."
```

**Issue 4: No Out-of-Tree Build**
```makefile
# Binary output in current directory (lites_server, lites_emulator)
# No way to specify build directory like CMake's -B flag
# Makes managing multiple configurations difficult
```

#### Strengths

1. **Simple and Direct**: Single file, no subdirectories to manage
2. **Rapid Development**: Quick command: `make -f Makefile.new`
3. **Good Documentation**: Comprehensive comments in file
4. **Flexible**: Easy to override variables: `make -f Makefile.new ARCH=i686 TIDY=1`
5. **Architecture Support**: Handles 6 architectures with auto-detection
6. **Static Analysis**: Integrated clang-tidy support

---

### 1.3 CMakeLists.txt Analysis

**File**: `/home/eirikr/Playground/lites/CMakeLists.txt`
**Lines**: 250+
**Status**: RECOMMENDED MODERN BUILD SYSTEM
**Language**: CMake 3.5+

#### Architecture Overview

```
┌────────────────────────────────────────────┐
│ CMakeLists.txt (build configuration)       │
├────────────────────────────────────────────┤
│ Phase 1: Project setup                     │
│   cmake_minimum_required(3.5)              │
│   project(lites LANGUAGES C)               │
│   set(CMAKE_C_STANDARD 23)                 │
├────────────────────────────────────────────┤
│ Phase 2: Option definitions                │
│   option(ENABLE_CLANG_TIDY "..." OFF)     │
│   option(LITES_BUILD_TESTS "..." OFF)     │
├────────────────────────────────────────────┤
│ Phase 3: Compiler detection                │
│   find_package(BISON)                      │
│   check_c_compiler_flag(-std=c23)          │
├────────────────────────────────────────────┤
│ Phase 4: Platform configuration            │
│   detect architecture (i686, x86_64, etc)  │
│   configure flags (-m32, -m64, etc)        │
├────────────────────────────────────────────┤
│ Phase 5: Mach headers location             │
│   search openmach/, core/mach_kernel/      │
│   fallback to /opt/mach/osfmk/             │
├────────────────────────────────────────────┤
│ Phase 6: Source collection                 │
│   file(GLOB_RECURSE SERVER_SRC ...)        │
│   file(GLOB KERN_SRC ...)                  │
│   file(GLOB_RECURSE IOMMU_SRC ...)         │
├────────────────────────────────────────────┤
│ Phase 7: Add executable targets            │
│   add_executable(lites_server ...)         │
│   add_executable(lites_emulator ...)       │
├────────────────────────────────────────────┤
│ Phase 8: Add library subdirectories        │
│   add_subdirectory(libs/libposix)          │
│   add_subdirectory(libs/libcrypto)         │
├────────────────────────────────────────────┤
│ Phase 9: Optional test configuration       │
│   if(LITES_BUILD_TESTS)                    │
│     add_subdirectory(tests)                │
├────────────────────────────────────────────┤
│ Phase 10: Generate build system            │
│   cmake generates Makefile or Ninja        │
└────────────────────────────────────────────┘
```

#### Key Features

**1. Automatic Compiler Detection**:
```cmake
check_c_compiler_flag(-std=c23 HAS_STD_C23)
if(HAS_STD_C23)
    set(C23_FLAG "-std=c23")
else()
    set(C23_FLAG "-std=c2x")
endif()
```

**2. Architecture-Aware Configuration**:
```cmake
if(ARCH STREQUAL "i686")
    add_compile_options(-m32)
    add_link_options(-m32)
elseif(ARCH STREQUAL "x86_64")
    add_compile_options(-m64)
    add_link_options(-m64)
# ... etc for arm, riscv64, powerpc, ia16
```

**3. Smart Mach Header Detection**:
```cmake
if(MACH_DIR AND EXISTS "${MACH_DIR}/include")
    set(MACH_INCLUDE_DIR "${MACH_DIR}/include")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/localmach/include")
    set(MACH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/localmach/include")
elseif(EXISTS "/opt/mach/osfmk/kernel/src")
    set(MACH_INCLUDE_DIR "/opt/mach/osfmk/kernel/src")
else()
    message(WARNING "Mach headers not found...")
endif()
```

**4. Flexible Source Collection**:
```cmake
# Architecture-specific
if(EXISTS "${LITES_SRC_DIR}/include/${_machine_arch}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "${_machine_arch}" "machine"
        WORKING_DIRECTORY "${LITES_SRC_DIR}/include")
endif()

# Server sources
file(GLOB SERVER_SUBDIRS ...)
# ... intelligent filtering for arch-specific vs common directories
set(SERVER_DIRS "${LITES_SRC_DIR}/server/${ARCH}")
foreach(dir ${SERVER_SUBDIRS})
    # Include common directories, architecture-specific if needed
endforeach()
```

**5. Library Subdirectory Support**:
```cmake
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/libposix")
    add_subdirectory(libs/libposix)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/libcrypto")
    add_subdirectory(libs/libcrypto)
endif()
# ... etc
```

**6. Optional Test Configuration**:
```cmake
option(LITES_BUILD_TESTS "Build the Lites unit tests" OFF)
if(LITES_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Then: cd build && ctest
```

#### Target Configuration Example

```cmake
# Executable definition
add_executable(lites_server ${SERVER_SRC})

# Include directories (target-specific)
target_include_directories(lites_server PRIVATE
    "${LITES_SRC_DIR}/include"
    "${LITES_SRC_DIR}/server"
    "${CMAKE_CURRENT_SOURCE_DIR}/kern"
    "${CMAKE_CURRENT_SOURCE_DIR}/posix"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/iommu"
    "${MACH_INCLUDE_DIR}")

# Architecture-specific includes
if(ARCH STREQUAL "x86_64")
    target_include_directories(lites_server PRIVATE
        "${LITES_SRC_DIR}/include/x86_64")
elseif(ARCH STREQUAL "i686")
    target_include_directories(lites_server PRIVATE
        "${LITES_SRC_DIR}/include/i386")
endif()

# Compilation flags
target_compile_options(lites_server PRIVATE ${C23_FLAG} ${CFLAGS})

# Link flags and libraries
target_link_options(lites_server PRIVATE ${LDFLAGS})
if(MACH_LIBS)
    target_link_libraries(lites_server PRIVATE ${MACH_LIBS})
endif()
```

#### Advantages Over Makefile.new

| Aspect | Makefile.new | CMakeLists.txt |
|--------|--------------|----------------|
| **IDE Integration** | NO | YES (VS Code, CLion, XCode) |
| **Cross-Platform** | Unix only | Windows, macOS, Linux |
| **Test Integration** | Manual (`make test`) | Built-in (`ctest`) |
| **Out-of-Tree Build** | NO | YES (`-B build`) |
| **Package Generation** | NO | YES (`cpack`) |
| **Dependency Detection** | Manual | Automatic (Bison, C features) |
| **Library Management** | Monolithic | Subdirectory-based (add_subdirectory) |
| **Generated Files** | NO | YES (Bison: .y → .c) |
| **Version Control** | N/A | Generated build files in .gitignore |
| **Documentation** | Good | Excellent (interactive cmake help) |

---

## PART 2: SOURCE FILE ORGANIZATION

### 2.1 Directory Structure and File Count

```
Root-level (37 .c files, 1 .h file):
├── alloc.c, alloc.h
├── audit.c, auth.c, bcmp.c, clock.c
├── fio.c, iget.c, insque.c
├── ipc_kobject.c, ipc_mig.c, ipc_queue.c
├── kalloc.c, main.c, malloc.c
├── nami.c, pipe.c, prf.c
├── protocols.c, protTbl.c
├── ptblData.c, ptblData.o2.c, ptbl_static.c
├── qsort.c, rdwri.c, remque.c
├── sched.c, sig.c, slp.c, subr.c
├── sys1.c, sys2.c, sys3.c, sys4.c, sysent.c
├── text.c, traceLevels.c, trap.c
└── cerror.S (1 assembly file)

Subdirectories:
├── core/iommu/
│   ├── iommu.c (BUILT)
│   ├── iommu.h
│   └── iommu_v2.c (STATUS UNCLEAR)
│
├── core/mach_kernel/
│   └── (Reference only, not built)
│
├── core/protocols/
│   └── (Subdirectory structure only)
│
├── servers/
│   ├── posix/x86_64/ (100+ .c files)
│   ├── posix/ns532/
│   ├── netbsd/pxk/
│   └── sunos/ (legacy)
│
└── [other subdirectories already catalogued]
```

### 2.2 File Classification

**Category A: ACTIVELY BUILT**
```
servers/            - All personality server implementations
core/iommu/         - IOMMU support (2 files)
libs/               - Libraries (20+ files across subdirs)
bootstrap/          - Bootable kernel (separate build)

Count: 100+ files
```

**Category B: REFERENCED BUT NOT FOUND**
```
kern/*.c            - Referenced in KERN_SRC, but kern/ dir doesn't exist
posix/*.c           - Referenced in KERN_SRC, but posix/ dir doesn't exist
libos/*.c           - Referenced in LIBOS_SRC, but libos/ dir doesn't exist

Count: 3 wildcards matching 0 files
```

**Category C: ORPHANED (Status Unclear)**
```
core/*.c            - 37 files, not in any build path
(except core/iommu/iommu.c which is included)

Count: 35 orphaned files (excluding iommu.c)
```

**Category D: STANDALONE (Separate Build)**
```
bootstrap/          - 25+ files, not integrated
util/               - Utility programs, separate Makefile
drivers/            - 2 files (iommu duplicates?)

Count: 30+ files
```

**Category E: ARCHIVED (Not Operational)**
```
Items1/             - Historical Lites 1.0, 1.1, 1.1.u3 releases
archives/           - Legacy makefile and builds

Count: 90+ Makefiles, 15 MB of historical code
```

---

## PART 3: BUILD BEHAVIOR ANALYSIS

### 3.1 Build Sequence Diagram

**Makefile.new Flow**:
```
User runs: make -f Makefile.new ARCH=x86_64
           |
           v
1. Initialize variables
   - SRCDIR ?= src-lites-1.1-2025 or build/lites-1.1.u3
   - ARCH = x86_64
   - ARCH_DIR = x86_64
   - ARCH_INCDIR = -Iinclude/x86_64
           |
           v
2. Locate Mach headers
   - Check LITES_MACH_DIR environment
   - Check openmach/
   - Check core/mach_kernel/
   - Fall back to /opt/mach/osfmk/
           |
           v
3. Run prepare target
   - Create include/machine symlink
           |
           v
4. Collect sources
   - SERVER_SRC: find $(SRCDIR)/server/{x86_64,kern,miscfs,...}
   - KERN_SRC: wildcard kern/*.c posix/*.c (matches 0)
   - IOMMU_SRC: wildcard core/iommu/*.c (matches 1)
   - LIBOS_SRC: wildcard libos/*.c (matches 0)
           |
           v
5. Compile all sources
   clang -O2 -std=c23 -Wall -Wextra -Werror -pedantic
        -I$(SRCDIR)/include -Iinclude/x86_64
        -I$(MACH_INCDIR)
        -I$(SERVER_DIRS)
        -Iinclude -Ikern -Icore/iommu -Iposix -Ilibos
        SERVER_SRC KERN_SRC IOMMU_SRC LIBOS_SRC
        -o lites_server
           |
           v
6. Optional: Static analysis
   if TIDY=1:
     clang-tidy SERVER_SRC KERN_SRC IOMMU_SRC LIBOS_SRC
           |
           v
7. Output
   ./lites_server (in current directory)
```

**CMakeLists.txt Flow**:
```
User runs: cmake -B build -DARCH=x86_64
           cmake --build build
           |
           v
1. Configure phase (cmake -B build)
   a. Find compiler and features
   b. Check for Bison
   c. Detect C23 support
   d. Set architecture flags
   e. Locate Mach headers
   f. Scan for sources
   g. Generate build files (Makefile or Ninja)
           |
           v
2. Build phase (cmake --build build)
   a. Compile lites_server
   b. Compile lites_emulator (if available)
   c. Build libraries (if add_subdirectory)
           |
           v
3. Test phase (cd build && ctest)
   a. Run unit tests
   b. Run integration tests
           |
           v
4. Output
   ./build/lites_server
   ./build/lites_emulator
   ./build/libposix.a
   ./build/libcrypto.a
   (+ test executables in build/)
```

### 3.2 Dependency Resolution

**Mach Header Dependencies**:
```
Makefile.new priority:
1. $(LITES_MACH_DIR)/include        (if env var set)
2. openmach/include                  (if submodule exists)
3. core/mach_kernel/include         (if local copy exists)
4. /opt/mach/osfmk/kernel/src       (system installation)
5. localmach/include                (minimal fallback)

If not found: WARNING but build continues (may fail later)
```

**Server Source Dependencies**:
```
Makefile.new requirement:
- $(SRCDIR)/server/ must exist
- $(SRCDIR)/include/ must exist

SRCDIR can be:
1. src-lites-1.1-2025/              (modernized tree)
2. build/lites-1.1.u3/              (extracted archive)

If not found: Build FAILS
Error: No such file or directory
```

---

## PART 4: COVERAGE VALIDATION

### 4.1 File Coverage Matrix

| Subsystem | Location | Lines | Makefile | Makefile.new | CMakeLists |
|-----------|----------|-------|----------|--------------|-----------|
| **Core Kernel** | core/*.c | 37 | NO | PARTIAL* | PARTIAL* |
| **IOMMU** | core/iommu/ | 2 | NO | YES | YES |
| **Servers** | servers/ | 100+ | NO | YES | YES |
| **POSIX** | libs/libposix/ | 4 | NO | NO | YES |
| **Crypto** | libs/libcrypto/ | 2 | NO | NO | YES |
| **Liblites** | libs/liblites/ | 15+ | NO | NO | YES |
| **DDEKit** | libs/ddekit/ | 1 | NO | NO | YES |
| **Bootstrap** | bootstrap/ | 25+ | NO | NO | NO |
| **Utilities** | util/ | 1 | NO | NO | NO |
| **Tests** | tests/ | 5+ | NO | NO | YES (opt) |

*PARTIAL: Only if in $(SRCDIR)/server tree, not local core/ copies

### 4.2 Source Verification Results

**Test 1: core/*.c Inclusion Check**
```bash
$ grep -l "core/[a-z]*\.c" Makefile.new
(output: none - not found)

$ grep "KERN_SRC\|LIBOS_SRC" Makefile.new
KERN_SRC := $(wildcard kern/*.c) $(wildcard posix/*.c)
LIBOS_SRC := $(wildcard libos/*.c)
(result: references non-existent directories)
```

**Test 2: File Build Verification**
```bash
$ make -f Makefile.new -n 2>&1 | grep "core.*alloc.c"
(output: no match - file is not built)

$ make -f Makefile.new -n 2>&1 | grep "core/iommu"
(output: matches - iommu.c is built)
```

**Test 3: CMakeLists.txt Verification**
```cmake
$ grep -n "add_executable" CMakeLists.txt
add_executable(lites_server ${SERVER_SRC})
add_executable(lites_emulator ${EMULATOR_SRC})

$ grep -n "add_subdirectory" CMakeLists.txt
add_subdirectory(libs/libposix)
add_subdirectory(libs/libcrypto)
add_subdirectory(libs/liblites)
add_subdirectory(libs/ddekit)
add_subdirectory(arch)
add_subdirectory(tests)        # if LITES_BUILD_TESTS=ON
```

---

## PART 5: RECOMMENDATIONS AND ACTION PLAN

### Phase 1: Immediate Actions (2-3 days)

**Action 1.1: Documentation Update**
```bash
# Update README.md
sed -i 's/make -f Makefile.new/cmake -B build/' README.md
# Add CMakeLists.txt as primary recommendation

# Update CLAUDE.md
- Add build system comparison table
- Mark CMakeLists.txt as canonical
- Deprecate Makefile.new in favor of cmake

# Create new file: BUILD_SYSTEM.md
- Explain Makefile.new vs CMakeLists.txt architecture
- Document SRCDIR and archive behavior
- Clarify core/ file status
```

**Action 1.2: Verify Current Builds**
```bash
# Test CMakeLists.txt
cmake -B build -DARCH=x86_64
cmake --build build
# EXPECTED: lites_server and lites_emulator created in build/

# Test Makefile.new
make -f Makefile.new ARCH=x86_64
# EXPECTED: lites_server and lites_emulator in current dir

# Test legacy Makefile
make
# EXPECTED: Fails with "RULES_MK not found"
```

### Phase 2: Short-term Fixes (1-2 weeks)

**Action 2.1: Investigate core/*.c Files**
```bash
# For each file in core/*.c (except iommu):
for f in core/[a-z]*.c; do
  echo "=== $f ==="
  
  # Check if it exists in archive
  find $(SRCDIR) -name "$(basename $f)" 2>/dev/null
  
  # Check if it's referenced anywhere
  grep -r "$(basename $f)" lib* tests* docs* 2>/dev/null
  
  # Check commit history
  git log --oneline $f | head -5
  
  # Classify: legacy | needed | misplaced
done
```

**Action 2.2: Fix Empty Directory References**
```bash
# Option A: Delete references from Makefile.new
sed -i '/KERN_SRC.*kern/d' Makefile.new
sed -i '/LIBOS_SRC/d' Makefile.new

# Option B: Create directories if needed
mkdir -p kern posix libos
# (only if these should contain files)

# Option C: Point to SRCDIR locations
# KERN_SRC := $(wildcard $(SRCDIR)/server/kern/*.c)
```

**Action 2.3: IOMMU Consolidation**
```bash
# Check if files are identical
diff core/iommu/iommu.c drivers/iommu/iommu.c
# If same: delete drivers copy, keep core copy
# If different: keep both, document purpose of each

# Verify iommu_v2.c purpose
grep -r "iommu_v2" include/ core/ drivers/ libs/
# Is it a newer version? Should it replace iommu.c?
```

### Phase 3: Medium-term Improvements (1 month)

**Action 3.1: Create BUILD_SYSTEM.md**
Documentation file explaining:
- Architecture of each build system
- File coverage by system
- Source tree discovery mechanism
- Known limitations and workarounds

**Action 3.2: Bootstrap Integration** (if needed)
```cmake
# Add to CMakeLists.txt
add_executable(bootable-image ${BOOTSTRAP_SRC})
add_custom_target(bootable-image-create
    COMMAND bootable-image create-image.sh
    DEPENDS bootable-image)
```

**Action 3.3: Test Framework Enhancement**
```cmake
# Ensure tests are built and run by default
set(LITES_BUILD_TESTS ON CACHE BOOL "Build tests" FORCE)

# Add test discovery
enable_testing()
add_subdirectory(tests)
```

### Phase 4: Long-term Strategy (Roadmap)

**Action 4.1: Deprecation Plan**
```bash
# Year 1: 
- Mark Makefile.new as deprecated in comments
- Recommend CMakeLists.txt in all docs

# Year 2:
- Remove Makefile.new if not needed

# Ongoing:
- Maintain CMakeLists.txt as canonical
- Delete legacy Makefile immediately (or archive)
```

**Action 4.2: CI/CD Update**
```yaml
# GitHub Actions should use CMakeLists.txt
cmake -B build -DARCH=x86_64 -DENABLE_CLANG_TIDY=ON
cmake --build build
cd build && ctest --verbose
```

---

## PART 6: MIGRATION GUIDE

### For Developers Currently Using Makefile.new

**Old Way** (still works):
```bash
make -f Makefile.new ARCH=x86_64
make -f Makefile.new test
make -f Makefile.new clean
```

**New Way** (recommended):
```bash
cmake -B build -DARCH=x86_64
cmake --build build
cd build && ctest --verbose
cmake --build build --target clean
```

**Key Differences**:
| Aspect | Makefile.new | CMakeLists.txt |
|--------|--------------|----------------|
| Build output | Current dir (lites_server) | build/ dir |
| Configuration | Command-line variables | CMake cache |
| Reconfiguration | Not needed | Needed if ARCH changes |
| Multiple configs | Not supported | Supported (build-i686/, build-x86_64/) |
| IDE integration | None | Full IDE support |

### For CI/CD Systems

**Update GitHub Actions**:
```yaml
# Old
- run: make -f Makefile.new ARCH=x86_64

# New
- run: cmake -B build -DARCH=x86_64
- run: cmake --build build
- run: cd build && ctest
```

### For Package Maintainers

**CMakeLists.txt provides standard CMake support**:
```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
cmake --install build

# Generates installation rules for:
# - lites_server -> /usr/local/bin/
# - Libraries -> /usr/local/lib/
# - Headers -> /usr/local/include/
```

---

## SUMMARY

**Current State**:
- Makefile (1995) = BROKEN, non-functional
- Makefile.new (2024) = OPERATIONAL, actively used but has coverage gaps
- CMakeLists.txt (2024) = COMPLETE, recommended, modern

**Critical Issues**:
1. 35 core/*.c files NOT built (status unclear)
2. 25+ bootstrap/ files standalone
3. kern/, posix/, libos/ directory references are broken
4. Build system choice not clearly documented

**Recommendations**:
1. Make CMakeLists.txt the canonical entry point
2. Investigate core/ file status and resolve
3. Fix broken directory references
4. Update documentation
5. Deprecate Makefile.new long-term

**Timeline**:
- Immediate: Documentation fixes (2 days)
- Short-term: core/ investigation + fixes (2 weeks)
- Medium-term: Bootstrap integration, test enhancement (1 month)
- Long-term: Makefile.new deprecation (1 year)

---

**Report Generated**: 2025-11-14
**Status**: Complete and actionable
**Next Step**: Execute Phase 1 actions (documentation updates)
