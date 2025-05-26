CC ?= clang
# Additional compile flags from the top-level build system are appended.
CFLAGS += -std=gnu23 -Wall -Wextra -Werror

all: test_iommu

.PHONY: all clean

ifndef LITES_SRC_DIR
$(error LITES_SRC_DIR must be set)
endif
SRC_DIR := $(LITES_SRC_DIR)
IOMMU_SRC := $(SRC_DIR)/iommu/iommu.c
ifneq ($(wildcard $(IOMMU_SRC)),)
IOMMU_PATH := $(IOMMU_SRC)
else
IOMMU_PATH := ../../iommu/iommu.c
endif

test_iommu: test_iommu.c $(IOMMU_PATH)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_iommu
