# Use the modern POSIX server source tree
# servers/posix/ contains the harmonized source code with proper structure:
#   core/, serv/, net/, netinet/, ufs/, nfs/, miscfs/, etc.
SRCDIR ?= servers/posix
CC ?= clang
CLANG_TIDY ?= clang-tidy
TIDY ?= 0

# Optional Mach kernel source tree for headers
MACHDIR ?= $(LITES_MACH_DIR)
ifeq ($(MACHDIR),)
ifneq ($(wildcard openmach/include),)
MACHDIR := openmach
else
ifneq ($(wildcard core/mach_kernel/include),)
MACHDIR := core/mach_kernel
endif
endif
endif

ifeq ($(MACHDIR),)
ifneq ($(wildcard localmach/include),)
MACH_INCDIR := -Ilocalmach/include
else ifneq ($(wildcard /opt/mach/osfmk/kernel/src),)
MACH_INCDIR := -I/opt/mach/osfmk/kernel/src
else
$(warning Mach headers not found. Set LITES_MACH_DIR, add openmach submodule, or run scripts/setup-build-environment.sh. Build may not complete without proper headers.)
MACH_INCDIR := -Ilocalmach/include
endif
else
MACH_INCDIR := -I$(MACHDIR)/include
endif
MACHLIBDIR ?= $(LITES_MACH_LIB_DIR)
ifeq ($(MACHLIBDIR),)
ifneq ($(MACHDIR),)
ifneq ($(wildcard $(MACHDIR)/lib),)
MACHLIBDIR := $(MACHDIR)/lib
endif
endif
endif
ifneq ($(MACHLIBDIR),)
MACH_LIBS := $(wildcard $(MACHLIBDIR)/*.a)
endif

# Target architecture (x86_64 or i686). Determines -m32/-m64 flags.
ARCH ?= x86_64

# Base optimisation flags
# Use C17 for better compatibility with legacy BSD code (per user request: "maximal C17")
# Allow gnu17 for compatibility with GCC extensions and K&R legacy code
C17_FLAG := $(shell $(CC) -std=gnu17 -E -x c /dev/null >/dev/null 2>&1 && echo -std=gnu17 || echo -std=c17)
CFLAGS ?= -O2 $(C17_FLAG)
# Enable warnings but allow legacy code to compile
# Note: -Werror disabled for Items1 legacy source (K&R function declarations)
# Note: -pedantic removed to allow GCC extensions and K&R syntax
CFLAGS += -Wall -Wextra
# Define KERNEL for BSD kernel compilation
CFLAGS += -DKERNEL

# System header isolation for kernel builds (Phase 10.2)
# Prevent conflicts between BSD and system headers (uid_t, pid_t, etc.)
# -nostdinc: Don't use standard system include directories
CFLAGS += -nostdinc
# Re-add compiler's internal include directory for stddef.h, stdarg.h, etc.
GCC_INCLUDE := $(shell $(CC) -print-file-name=include)
ifneq ($(GCC_INCLUDE),include)
CFLAGS += -I$(GCC_INCLUDE)
endif

# K&R compatibility flags - allow legacy code to build
# These will be removed after systematic ANSI-fication
CFLAGS += -Wno-implicit-int
CFLAGS += -Wno-int-conversion
CFLAGS += -Wno-incompatible-pointer-types
CFLAGS += -Wno-return-mismatch
# Harden binaries by disallowing executable stacks by appending the
# noexecstack flag to the existing linker options. This ensures the
# generated binaries do not allow execution from the stack.
LDFLAGS := $(LDFLAGS) -Wl,-z,noexecstack

# Optional address sanitizer
SANITIZE ?= 0
ifeq ($(SANITIZE),1)
CFLAGS += -fsanitize=address
LDFLAGS += -fsanitize=address
endif

# Multicore scheduler support
MULTICORE_SCHED ?= 0
ifeq ($(MULTICORE_SCHED),1)
CFLAGS += -DCONFIG_SCHED_MULTICORE=1
else
CFLAGS += -DCONFIG_SCHED_MULTICORE=0
endif


# Translate ARCH into compiler options and per-architecture includes
ifeq ($(ARCH),i686)
CFLAGS += -m32
LDFLAGS += -m32
ARCH_INCDIR := -Iinclude/i386
else ifeq ($(ARCH),x86_64)
CFLAGS += -m64
LDFLAGS += -m64
ARCH_INCDIR := -Iinclude/x86_64
else ifeq ($(ARCH),arm)
CFLAGS += -marm
ARCH_INCDIR := -Iinclude/arm
else ifeq ($(ARCH),riscv64)
CFLAGS += -march=rv64imac -mabi=lp64
ARCH_INCDIR := -Iinclude/riscv64
else ifeq ($(ARCH),powerpc)
CFLAGS += -m32
LDFLAGS += -m32
ARCH_INCDIR := -Iinclude/powerpc
else ifeq ($(ARCH),ia16)
CFLAGS += -m16
LDFLAGS += -m16
ARCH_INCDIR := -Iinclude/ia16
endif

# Map ARCH to the corresponding directory name
ARCH_DIR := $(ARCH)
ifeq ($(ARCH),i686)
ARCH_DIR := i386
else ifeq ($(ARCH),ia16)
ARCH_DIR := ia16
endif

# Directories in the modern POSIX server tree (architecture independent)
# Note: core/ contains kern_*.c files (modern organized kernel code)
SERVER_COMMON_DIRS := \
  $(SRCDIR)/core \
  $(SRCDIR)/miscfs \
  $(SRCDIR)/netiso \
  $(SRCDIR)/serv \
  $(SRCDIR)/netinet \
  $(SRCDIR)/ufs \
  $(SRCDIR)/nfs \
  $(SRCDIR)/netccitt \
  $(SRCDIR)/net \
  $(SRCDIR)/netns

# Source files and include directories for the selected architecture
SERVER_DIRS := $(SRCDIR)/$(ARCH_DIR) $(SERVER_COMMON_DIRS)
SERVER_SRC_ALL := $(foreach d,$(SERVER_DIRS),$(shell find $(d) -name \*.c -o -name \*.S))
# Exclude problematic files with K&R function declarations that don't compile with modern C
# Exclude entire netns/ directory (Xerox NS protocol) due to header parsing issues with cthreads/setjmp
EXCLUDE_PATTERNS := libc.opendir.c
SERVER_SRC := $(filter-out $(foreach p,$(EXCLUDE_PATTERNS),$(filter %$(p),$(SERVER_SRC_ALL))),$(SERVER_SRC_ALL))
SERVER_SRC := $(filter-out $(filter %/netns/%,$(SERVER_SRC)),$(SERVER_SRC))
# Add SRCDIR to include path for <serv/header.h> style includes
# Also add repository root include/ and each source directory
SERVER_INCDIRS := -Iinclude -I$(SRCDIR) $(addprefix -I,$(SERVER_DIRS))
# Scattered core/ files (38 files identified in audit) plus subdirectories
CORE_SCATTERED_SRC := $(wildcard core/*.c) $(wildcard core/*.S)
CORE_PROTOCOLS_SRC := $(wildcard core/protocols/*.c)
CORE_MACH_KERNEL_SRC := $(wildcard core/mach_kernel/*.c)
# IOMMU enabled for x86_64 architecture - modern code from merged branches
ifeq ($(ARCH),x86_64)
CORE_IOMMU_SRC := $(wildcard core/iommu/*.c)
CORE_IOMMU_INCDIR := -Icore/iommu
else
CORE_IOMMU_SRC :=
CORE_IOMMU_INCDIR :=
endif
# Legacy kern/ and posix/ directories (if they exist at repository root)
KERN_SRC := $(wildcard kern/*.c) $(wildcard posix/*.c)
LIBOS_SRC := $(wildcard libos/*.c)
# Combined core sources
CORE_SRC := $(CORE_SCATTERED_SRC) $(CORE_PROTOCOLS_SRC) $(CORE_MACH_KERNEL_SRC) $(CORE_IOMMU_SRC) $(KERN_SRC)
CORE_INCDIRS := -Iinclude -Icore -Icore/protocols -Icore/mach_kernel $(CORE_IOMMU_INCDIR) -Ikern -Iposix -Ilibos

ifneq ($(wildcard $(SRCDIR)/emulator),)
EMULATOR_SRC := $(shell find $(SRCDIR)/emulator -name '*.c' -o -name '*.S')
EMULATOR_INCDIRS := -I$(SRCDIR)/emulator
endif

TARGETS := lites_server
ifneq ($(EMULATOR_SRC),)
TARGETS += lites_emulator
endif

TEST_SUBDIRS := $(sort $(dir $(wildcard tests/*/Makefile)))

all: prepare $(TARGETS)

prepare:
	@if [ ! -e include/machine ]; then \
	  arch_dir=$(ARCH); \
	  [ "$$arch_dir" = "i686" ] && arch_dir=i386; \
	  ln -s $$arch_dir include/machine; \
	fi

lites_server: $(SERVER_SRC) $(CORE_SRC) $(LIBOS_SRC)
	$(CC) $(CFLAGS) $(ARCH_INCDIR) $(MACH_INCDIR) $(SERVER_INCDIRS) $(CORE_INCDIRS) $^ $(MACH_LIBS) $(LDFLAGS) -o $@
ifeq ($(TIDY),1)
	$(CLANG_TIDY) $(SERVER_SRC) $(CORE_SRC) $(LIBOS_SRC) -- $(CFLAGS) $(ARCH_INCDIR) $(MACH_INCDIR) $(SERVER_INCDIRS) $(CORE_INCDIRS)
endif

ifneq ($(EMULATOR_SRC),)
lites_emulator: $(EMULATOR_SRC) $(CORE_SRC) $(LIBOS_SRC)
	$(CC) $(CFLAGS) $(ARCH_INCDIR) $(MACH_INCDIR) $(EMULATOR_INCDIRS) $(CORE_INCDIRS) $^ $(MACH_LIBS) $(LDFLAGS) -o $@
ifeq ($(TIDY),1)
	$(CLANG_TIDY) $(EMULATOR_SRC) $(CORE_SRC) $(LIBOS_SRC) -- $(CFLAGS) $(ARCH_INCDIR) $(MACH_INCDIR) $(EMULATOR_INCDIRS) $(CORE_INCDIRS)
endif
endif

clean:
		rm -f lites_server lites_emulator

test: all
	@for d in $(TEST_SUBDIRS); do \
	$(MAKE) -C $$d; \
	done

tidy:
	$(CLANG_TIDY) $(SERVER_SRC) $(EMULATOR_SRC) $(CORE_SRC) $(LIBOS_SRC) -- $(CFLAGS) $(ARCH_INCDIR) $(MACH_INCDIR) $(SERVER_INCDIRS) $(CORE_INCDIRS)

.PHONY: all prepare clean test tidy
