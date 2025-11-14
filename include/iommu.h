#ifndef IOMMU_H
#define IOMMU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct iommu_map_entry {
    uintptr_t iova;
    uintptr_t pa;
    size_t len;
};

struct iommu_mapping {
    struct iommu_map_entry entry;
    struct iommu_mapping *next;
};

/*
 * Simple lock type for kernel context.
 * In a real implementation, this would use Mach locks or kernel mutexes.
 * For now, use a simple integer lock (0 = unlocked, 1 = locked).
 */
typedef int iommu_lock_t;

typedef struct iommu_dom {
    iommu_lock_t lock;
    struct iommu_mapping *mappings;
} iommu_dom_t;

static inline void iommu_dom_init(struct iommu_dom *dom) {
    dom->lock = 0;  /* unlocked */
    dom->mappings = NULL;
}

static inline void iommu_dom_lock(struct iommu_dom *dom) {
    /* Simple spinlock-style lock (not production quality, but compiles) */
    while (__sync_lock_test_and_set(&dom->lock, 1)) {
        /* spin */
    }
}

static inline void iommu_dom_unlock(struct iommu_dom *dom) {
    __sync_lock_release(&dom->lock);
}

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len);
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *dom, const struct iommu_map_entry *list, size_t count);

#endif /* IOMMU_H */
