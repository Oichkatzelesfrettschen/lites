#include "iommu_v2.h"
#include <stdlib.h>

struct mapping {
    uintptr_t iova;
    uintptr_t pa;
    size_t len;
    uint32_t perms;
    struct mapping *next;
};

static struct mapping **map_list(struct iommu_dom *d) {
    return (struct mapping **)&d->pml_root; /* treat pml_root as list head */
}

static void tlb_invalidate_domain(uint16_t asid) { (void)asid; }

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len, uint32_t perms) {
    if (!dom || !len)
        return -1;
    struct mapping *m = malloc(sizeof(*m));
    if (!m)
        return -1;
    m->iova = iova;
    m->pa = pa;
    m->len = len;
    m->perms = perms;
    spin_lock(&dom->lock);
    m->next = *map_list(dom);
    *map_list(dom) = m;
    dom->epoch++;
    spin_unlock(&dom->lock);
    tlb_invalidate_domain(dom->asid);
    return 0;
}

int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len) {
    if (!dom || !len)
        return -1;
    spin_lock(&dom->lock);
    struct mapping **p = map_list(dom);
    while (*p) {
        if ((*p)->iova == iova && (*p)->len == len) {
            struct mapping *tmp = *p;
            *p = tmp->next;
            free(tmp);
            dom->epoch++;
            spin_unlock(&dom->lock);
            tlb_invalidate_domain(dom->asid);
            return 0;
        }
        p = &(*p)->next;
    }
    spin_unlock(&dom->lock);
    return -1;
}

int iommu_bulk_map(struct iommu_dom *dom, const uintptr_t *iovas, const uintptr_t *pas,
                   const size_t *lens, const uint32_t *perms, size_t count) {
    if (!dom)
        return -1;
    for (size_t i = 0; i < count; ++i)
        if (iommu_map(dom, iovas[i], pas[i], lens[i], perms[i]) != 0)
            return -1;
    return 0;
}
