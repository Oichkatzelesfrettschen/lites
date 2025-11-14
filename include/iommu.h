#ifndef IOMMU_H
#define IOMMU_H

#include <pthread.h>
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

typedef struct iommu_dom {
    pthread_mutex_t lock;
    struct iommu_mapping *mappings;
} iommu_dom_t;

static inline void iommu_dom_init(struct iommu_dom *dom) {
    pthread_mutex_init(&dom->lock, NULL);
    dom->mappings = NULL;
}

static inline void iommu_dom_lock(struct iommu_dom *dom) { pthread_mutex_lock(&dom->lock); }

static inline void iommu_dom_unlock(struct iommu_dom *dom) { pthread_mutex_unlock(&dom->lock); }

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len);
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *dom, const struct iommu_map_entry *list, size_t count);

#endif /* IOMMU_H */
