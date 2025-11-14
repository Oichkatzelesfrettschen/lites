#include <stddef.h>   /* size_t */
#include <stdbool.h>  /* bool */
#include <stdint.h>   /* uintptr_t */
#include <stdlib.h>   /* malloc, free */
#include <iommu.h>

static struct iommu_mapping *find_mapping(struct iommu_dom *dom, uintptr_t iova, size_t len,
                                          struct iommu_mapping ***prev) {
    struct iommu_mapping **p = &dom->mappings;
    while (*p) {
        struct iommu_mapping *cur = *p;
        if (cur->entry.iova == iova && cur->entry.len == len) {
            if (prev)
                *prev = p;
            return cur;
        }
        p = &(*p)->next;
    }
    return NULL;
}

static bool overlap(const struct iommu_map_entry *a, const struct iommu_map_entry *b) {
    uintptr_t a_end = a->iova + a->len;
    uintptr_t b_end = b->iova + b->len;
    return (a->iova < b_end) && (b->iova < a_end);
}

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len) {
    if (!dom || len == 0)
        return -1;

    struct iommu_map_entry ent = {.iova = iova, .pa = pa, .len = len};

    iommu_dom_lock(dom);
    for (struct iommu_mapping *cur = dom->mappings; cur; cur = cur->next) {
        if (overlap(&ent, &cur->entry)) {
            iommu_dom_unlock(dom);
            return -1;
        }
    }

    struct iommu_mapping *m = malloc(sizeof(*m));
    if (!m) {
        iommu_dom_unlock(dom);
        return -1;
    }
    m->entry = ent;
    m->next = dom->mappings;
    dom->mappings = m;
    iommu_dom_unlock(dom);
    return 0;
}

int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len) {
    if (!dom || len == 0)
        return -1;

    iommu_dom_lock(dom);
    struct iommu_mapping **prev = NULL;
    struct iommu_mapping *m = find_mapping(dom, iova, len, &prev);
    if (!m) {
        iommu_dom_unlock(dom);
        return -1;
    }
    *prev = m->next;
    iommu_dom_unlock(dom);
    free(m);
    return 0;
}

int iommu_bulk_map(struct iommu_dom *dom, const struct iommu_map_entry *list, size_t count) {
    size_t i;
    for (i = 0; i < count; ++i) {
        if (iommu_map(dom, list[i].iova, list[i].pa, list[i].len) != 0)
            break;
    }
    if (i != count) {
        /* rollback previously mapped entries */
        while (i-- > 0)
            iommu_unmap(dom, list[i].iova, list[i].len);
        return -1;
    }
    return 0;
}
