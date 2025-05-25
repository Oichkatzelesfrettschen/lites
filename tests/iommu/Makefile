ifeq ($(origin CC),default)
CC := $(shell command -v clang >/dev/null 2>&1 && echo clang || echo gcc)
endif
CFLAGS ?= -std=c23 -Wall -Werror

all: test_iommu

.PHONY: all clean

test_iommu: test_iommu.c ../../src-lites-1.1-2025/iommu/iommu.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f test_iommu
