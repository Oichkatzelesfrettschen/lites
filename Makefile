# Prefer the modernised source tree if present
ifeq ($(wildcard src-lites-1.1-2025),src-lites-1.1-2025)
SRCDIR ?= src-lites-1.1-2025
else
SRCDIR ?= build/lites-1.1.u3
endif
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

# Directories under server that are architecture independent
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

# Source files and include directories for the selected architecture
SERVER_DIRS := $(SRCDIR)/server/$(ARCH_DIR) $(SERVER_COMMON_DIRS)
SERVER_SRC_ALL := $(foreach d,$(SERVER_DIRS),$(shell find $(d) -name \*.c -o -name \*.S))
# Exclude problematic files with K&R function declarations that don't compile with modern C
# Exclude entire netns/ directory (Xerox NS protocol) due to header parsing issues with cthreads/setjmp
EXCLUDE_PATTERNS := libc.opendir.c
SERVER_SRC := $(filter-out $(foreach p,$(EXCLUDE_PATTERNS),$(filter %$(p),$(SERVER_SRC_ALL))),$(SERVER_SRC_ALL))
SERVER_SRC := $(filter-out $(filter %/netns/%,$(SERVER_SRC)),$(SERVER_SRC))
# Add server/ directory itself to include path for <serv/header.h> style includes
SERVER_INCDIRS := -I$(SRCDIR)/include -I$(SRCDIR)/server $(addprefix -I,$(SERVER_DIRS))
KERN_SRC := $(wildcard kern/*.c) $(wildcard posix/*.c)
# IOMMU disabled for legacy i686 build - modern code from merged branches
ifeq ($(ARCH),x86_64)
IOMMU_SRC := $(wildcard core/iommu/*.c)
IOMMU_INCDIR := -Icore/iommu
else
IOMMU_SRC :=
IOMMU_INCDIR :=
endif
LIBOS_SRC := $(wildcard libos/*.c)
KERN_INCDIRS := -Iinclude -Ikern $(IOMMU_INCDIR) -Iposix -Ilibos

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
	@if [ ! -e $(SRCDIR)/include/machine ]; then \
	  arch_dir=$(ARCH); \
	  [ "$$arch_dir" = "i686" ] && arch_dir=i386; \
	  ln -s $$arch_dir $(SRCDIR)/include/machine; \
	fi

lites_server: $(SERVER_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC)
	$(CC) $(CFLAGS) -I$(SRCDIR)/include $(ARCH_INCDIR) $(MACH_INCDIR) $(SERVER_INCDIRS) $(KERN_INCDIRS) $^ $(MACH_LIBS) $(LDFLAGS) -o $@
ifeq ($(TIDY),1)
	$(CLANG_TIDY) $(SERVER_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC) -- $(CFLAGS) -I$(SRCDIR)/include $(ARCH_INCDIR) $(MACH_INCDIR) $(SERVER_INCDIRS) $(KERN_INCDIRS)
endif

ifneq ($(EMULATOR_SRC),)
lites_emulator: $(EMULATOR_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC)
	$(CC) $(CFLAGS) -I$(SRCDIR)/include $(ARCH_INCDIR) $(MACH_INCDIR) $(EMULATOR_INCDIRS) $(KERN_INCDIRS) $^ $(MACH_LIBS) $(LDFLAGS) -o $@
ifeq ($(TIDY),1)
	$(CLANG_TIDY) $(EMULATOR_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC) -- $(CFLAGS) -I$(SRCDIR)/include $(ARCH_INCDIR) $(MACH_INCDIR) $(EMULATOR_INCDIRS) $(KERN_INCDIRS)
endif
endif

clean:
		rm -f lites_server lites_emulator

test: all
	@for d in $(TEST_SUBDIRS); do \
	$(MAKE) -C $$d; \
	done

tidy:
	$(CLANG_TIDY) $(SERVER_SRC) $(EMULATOR_SRC) $(KERN_SRC) $(IOMMU_SRC) $(LIBOS_SRC) -- $(CFLAGS) -I$(SRCDIR)/include $(ARCH_INCDIR) $(MACH_INCDIR)

.PHONY: all prepare clean test tidy
