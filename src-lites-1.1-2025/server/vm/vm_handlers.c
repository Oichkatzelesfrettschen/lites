#include <serv/import_mach.h>
#include <vm.h>

/*
 * Basic virtual memory handlers. These are placeholders for
 * a future implementation.
 */

kern_return_t vm_fault_entry(aspace_t *as, vm_offset_t addr, vm_prot_t prot) {
    (void)as;
    (void)addr;
    (void)prot;
    /* TODO: handle page faults from Mach */
    return KERN_FAILURE;
}

kern_return_t map_frame(aspace_t *as, vm_offset_t vaddr, mach_port_t frame, vm_prot_t prot,
                        int flags) {
    (void)as;
    (void)vaddr;
    (void)frame;
    (void)prot;
    (void)flags;
    /* TODO: establish a mapping for the frame */
    return KERN_FAILURE;
}

kern_return_t unmap_frame(aspace_t *as, vm_offset_t vaddr) {
    (void)as;
    (void)vaddr;
    /* TODO: remove mapping at the given address */
    return KERN_FAILURE;
}
