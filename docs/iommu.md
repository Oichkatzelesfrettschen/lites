# IOMMU Module

This module provides a simple in-memory implementation of an IOMMU domain.
It allows mapping I/O virtual addresses (IOVA) to physical addresses. The
interface is defined in `include/iommu.h`.

## Location

The IOMMU implementation is organized as follows:
- Header: `include/iommu.h`
- Implementation: `core/iommu/iommu.c`

## Data Structures

```c
struct iommu_map_entry {
    uintptr_t iova;
    uintptr_t pa;
    size_t len;
};

typedef struct iommu_dom {
    pthread_mutex_t lock;
    struct iommu_mapping *mappings;
} iommu_dom_t;
```

## Functions

```c
void iommu_dom_init(struct iommu_dom *dom);
int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len);
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *dom, const struct iommu_map_entry *list, size_t count);
```

## Usage

1. Initialize a domain:
   ```c
   iommu_dom_t dom;
   iommu_dom_init(&dom);
   ```

2. Map or unmap regions:
   ```c
   iommu_map(&dom, iova, phys_addr, length);
   iommu_unmap(&dom, iova, length);
   ```

3. Bulk mappings are possible with `iommu_bulk_map` which takes an array of
   `iommu_map_entry` structures:
   ```c
   struct iommu_map_entry entries[2] = {
       {.iova = 0x1000, .pa = 0x2000, .len = 4096},
       {.iova = 0x3000, .pa = 0x4000, .len = 4096}
   };
   iommu_bulk_map(&dom, entries, 2);
   ```

## Implementation Details

Mappings are protected by an internal mutex (`pthread_mutex_t`). The current
implementation stores entries in a linked list and checks for overlaps on each
map operation. Returns -1 on error (overlapping mappings, allocation failure,
or invalid parameters), 0 on success.
