# IOMMU helpers

A minimal IOMMU abstraction lives in `iommu/` within the source tree.

```c
struct iommu_dom {
    uintptr_t    pml_root; /* internal mapping list head */
    uint16_t     asid;     /* address space identifier */
    unsigned long epoch;   /* incremented on updates */
    uint32_t     flags;
    spinlock_t   lock;
};
```

## Functions

```c
int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa,
              size_t len, uint32_t perms);
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *dom, const uintptr_t *iovas,
                   const uintptr_t *pas, const size_t *lens,
                   const uint32_t *perms, size_t count);
```

`iommu_map` installs a single mapping. `iommu_unmap` removes it. `iommu_bulk_map`
conveniently installs multiple mappings using the same semantics as the single
call. Every change increments `dom.epoch` and triggers a domain TLB flush via
`tlb_invalidate_domain()`.

## Example

```c
struct iommu_dom dom = { .asid = 1 };
spin_lock_init(&dom.lock);

iommu_map(&dom, 0x1000, 0x2000, 4096, 0);
iommu_unmap(&dom, 0x1000, 4096);

uintptr_t iovas[2] = {0x3000, 0x4000};
uintptr_t pas[2] = {0x5000, 0x6000};
size_t lens[2] = {4096, 4096};
uint32_t perms[2] = {0, 0};
iommu_bulk_map(&dom, iovas, pas, lens, perms, 2);
```
