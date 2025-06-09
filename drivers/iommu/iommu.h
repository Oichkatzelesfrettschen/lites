#ifndef IOMMU_H
#define IOMMU_H

#include <stddef.h>
#include <stdint.h>

#ifndef SPINLOCK_T_DEFINED
#define SPINLOCK_T_DEFINED
/** Simple spinlock primitive for mutual exclusion. */
typedef struct {
    volatile int locked; /**< Non-zero when held. */
} spinlock_t;

/** Initialise a spinlock to the unlocked state. */
static inline void spin_lock_init(spinlock_t *l) { l->locked = 0; }

/** Acquire a spinlock by busy waiting. */
static inline void spin_lock(spinlock_t *l) {
    while (__sync_lock_test_and_set(&l->locked, 1))
        ;
}
/** Release a previously acquired spinlock. */
static inline void spin_unlock(spinlock_t *l) { __sync_lock_release(&l->locked); }
#endif

/** IOMMU domain state used for address translation. */
struct iommu_dom {
    uintptr_t pml_root;
    uint16_t asid;
    unsigned long epoch;
    uint32_t flags;
    spinlock_t lock;
};

/** Map a physical range to an IO virtual address.
 *  \return 0 on success, -1 on failure.
 */
int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len, uint32_t perms);

/** Remove a mapping from an IOMMU domain.
 *  \return 0 on success, -1 on failure.
 */
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);

/** Map multiple ranges at once.
 *  \return 0 on success, -1 on failure.
 */
int iommu_bulk_map(struct iommu_dom *dom, const uintptr_t *iovas, const uintptr_t *pas,
                   const size_t *lens, const uint32_t *perms, size_t count);

#endif /* IOMMU_H */
