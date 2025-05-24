# IOMMU Module

This module provides a simple in-memory implementation of an IOMMU domain.
It allows mapping I/O virtual addresses (IOVA) to physical addresses. The
interface is defined in `include/iommu.h`.

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
   `iommu_map_entry` structures.

Mappings are protected by an internal mutex. The current implementation stores
entries in a linked list and checks for overlaps on each map operation.
