CC ?= clang
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=gnu23 -Wall -Wextra -Werror

all: test_iommu

.PHONY: all clean

test_iommu: test_iommu.c ../../src-lites-1.1-2025/iommu/iommu.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_iommu
