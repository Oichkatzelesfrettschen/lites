#include "../../src-lites-1.1-2025/iommu/iommu.h"
#include <assert.h>
#include <stdlib.h>

int main(void) {
    struct iommu_dom d = {0};
    d.asid = 1;
    spin_lock_init(&d.lock);

    assert(iommu_map(&d, 0x1000, 0x2000, 4096, 0) == 0);
    assert(d.epoch == 1);
    assert(iommu_unmap(&d, 0x1000, 4096) == 0);
    assert(d.epoch == 2);

    uintptr_t iovas[2] = {0x3000, 0x4000};
    uintptr_t pas[2] = {0x5000, 0x6000};
    size_t lens[2] = {4096, 4096};
    uint32_t perms[2] = {0, 0};
    assert(iommu_bulk_map(&d, iovas, pas, lens, perms, 2) == 0);
    assert(d.epoch == 4);

    assert(iommu_unmap(&d, 0x3000, 4096) == 0);
    assert(iommu_unmap(&d, 0x4000, 4096) == 0);

    return 0;
}
