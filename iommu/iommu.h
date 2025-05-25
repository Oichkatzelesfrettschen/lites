#ifndef IOMMU_H
#define IOMMU_H

#include <stddef.h>
#include <stdint.h>

#ifndef SPINLOCK_T_DEFINED
#define SPINLOCK_T_DEFINED
typedef struct {
    volatile int locked;
} spinlock_t;

static inline void spin_lock_init(spinlock_t *l) { l->locked = 0; }
static inline void spin_lock(spinlock_t *l) {
    while (__sync_lock_test_and_set(&l->locked, 1))
        ;
}
static inline void spin_unlock(spinlock_t *l) { __sync_lock_release(&l->locked); }
#endif

struct iommu_dom {
    uintptr_t pml_root;
    uint16_t asid;
    unsigned long epoch;
    uint32_t flags;
    spinlock_t lock;
};

int iommu_map(struct iommu_dom *, uintptr_t iova, uintptr_t pa, size_t len, uint32_t perms);
int iommu_unmap(struct iommu_dom *, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *, const uintptr_t *iovas, const uintptr_t *pas,
                   const size_t *lens, const uint32_t *perms, size_t count);

#endif /* IOMMU_H */
