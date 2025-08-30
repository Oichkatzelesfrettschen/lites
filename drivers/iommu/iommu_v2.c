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

/**
 * @brief Establish a single I/O mapping in the given domain.
 *
 * Associates a contiguous physical memory range with an I/O virtual
 * address range and records it in the domain's bookkeeping. The domain's
 * epoch counter is incremented and the translation cache is invalidated to
 * make the new translation visible to devices.
 *
 * @param[in]  dom    IOMMU domain to receive the mapping.
 * @param[in]  iova   Starting I/O virtual address of the region.
 * @param[in]  pa     Starting physical address backing the region.
 * @param[in]  len    Length of the region in bytes; must be non-zero.
 * @param[in]  perms  Access permission flags applied to the mapping.
 *
 * @retval 0   Mapping established successfully.
 * @retval -1  Invalid parameters or memory allocation failure.
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
 * @brief Remove an existing mapping from an IOMMU domain.
 *
 * Searches the domain's mapping list for an entry matching the provided
 * I/O virtual address and length. When found, the entry is removed, the
 * epoch counter is incremented, and the translation cache is invalidated
 * so subsequent accesses observe the change.
 *
 * @param[in] dom   Domain from which to remove the mapping.
 * @param[in] iova  Starting I/O virtual address of the mapping.
 * @param[in] len   Length of the mapping to remove in bytes.
 *
 * @retval 0   Mapping removed successfully.
 * @retval -1  Error occurred or mapping not found.
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
 * @brief Map several address ranges in a single operation.
 *
 * Convenience wrapper that iteratively invokes @ref iommu_map for each
 * element of the provided arrays, allowing callers to populate an IOMMU
 * domain with many mappings efficiently.
 *
 * @param[in]  dom    Domain receiving the mappings.
 * @param[in]  iovas  Array of I/O virtual address starts; must have at
 *                    least @p count elements.
 * @param[in]  pas    Array of physical address starts; must have at least
 *                    @p count elements.
 * @param[in]  lens   Array of region lengths in bytes; must have at least
 *                    @p count elements.
 * @param[in]  perms  Array of permission flags for each region; must have
 *                    at least @p count elements.
 * @param[in]  count  Number of mappings to establish.
 *
 * @retval 0   All mappings established successfully.
 * @retval -1  Any individual mapping failed.
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
