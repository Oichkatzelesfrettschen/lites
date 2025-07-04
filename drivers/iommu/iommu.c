#include "iommu.h"
#include <stdlib.h>

/** Simple linked list node used for IOMMU mappings. */
struct mapping {
    uintptr_t iova;       /**< IO virtual address start. */
    uintptr_t pa;         /**< Physical address start. */
    size_t len;           /**< Length of the mapping in bytes. */
    uint32_t perms;       /**< Permission bits for the range. */
    struct mapping *next; /**< Next mapping in the domain list. */
};

/** Obtain the mapping list head for a domain. */
static struct mapping **map_list(struct iommu_dom *d) {
    /* Treat pml_root as a pointer to the first mapping entry. */
    return (struct mapping **)&d->pml_root;
}

/**
 * Flush any cached translations for a domain.
 *
 * This implementation is a placeholder and currently does nothing.
 */
static void tlb_invalidate_domain(uint16_t asid) { (void)asid; }

/**
 * Map a physical address range into an IOMMU domain.
 *
 * @param dom   Target IOMMU domain.
 * @param iova  IO virtual address where the range begins.
 * @param pa    Physical address backing the range.
 * @param len   Length of the region to map in bytes.
 * @param perms Access permissions for the range.
 * @return 0 on success or -1 on failure.
 */
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

/**
 * Remove a previously established mapping.
 *
 * @param dom  IOMMU domain containing the mapping.
 * @param iova IO virtual address to unmap.
 * @param len  Length of the mapped region.
 * @return 0 on success or -1 on failure.
 */
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

/**
 * Map multiple ranges into an IOMMU domain.
 *
 * @param dom    IOMMU domain to modify.
 * @param iovas  Array of IO virtual address starts.
 * @param pas    Array of physical address starts.
 * @param lens   Array of mapping lengths.
 * @param perms  Array of permission values.
 * @param count  Number of entries in each array.
 * @return 0 on success or -1 on failure.
 */
int iommu_bulk_map(struct iommu_dom *dom, const uintptr_t *iovas, const uintptr_t *pas,
                   const size_t *lens, const uint32_t *perms, size_t count) {
    if (!dom)
        return -1;
    for (size_t i = 0; i < count; ++i)
        if (iommu_map(dom, iovas[i], pas[i], lens[i], perms[i]) != 0)
            return -1;
    return 0;
}
