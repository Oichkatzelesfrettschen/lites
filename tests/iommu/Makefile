CC ?= gcc
CFLAGS ?= -std=c2x -Wall -Wextra

all: test_iommu

.PHONY: all clean

test_iommu: test_iommu.c ../../src-lites-1.1-2025/iommu/iommu.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f test_iommu
