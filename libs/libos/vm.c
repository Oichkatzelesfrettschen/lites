#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>

#include "vm.h"

/*
 * Basic virtual memory handlers. These are placeholders for
 * a future implementation.
 */

/**
 * @brief Handle a page fault for the given address space.
 *
 * Notifies an optional pager about the fault and maps a writable page at the
 * faulting address.
 *
 * @param as   Address space receiving the fault.
 * @param addr Faulting virtual address.
 * @param prot Requested protection for the new mapping.
 * @return KERN_SUCCESS on success, otherwise KERN_FAILURE.
 */
kern_return_t vm_fault_entry(aspace_t *as, vm_offset_t addr, vm_prot_t prot) {
    if (!as)
        return KERN_FAILURE;

    pf_info_t info = {.addr = addr, .prot = prot, .flags = 0};
    if (as->pager_cap >= 0) {
        (void)write(as->pager_cap, &info, sizeof(info));
        char ack;
        (void)read(as->pager_cap, &ack, 1);
    }

    size_t psz = (size_t)sysconf(_SC_PAGESIZE);
    void *res = mmap(addr ? (void *)addr : NULL, psz, PROT_READ | PROT_WRITE,
                     MAP_ANON | MAP_PRIVATE | (addr ? MAP_FIXED : 0), -1, 0);
    if (res == MAP_FAILED)
        return KERN_FAILURE;
    return KERN_SUCCESS;
}

/**
 * @brief Map a frame into an address space.
 *
 * Creates a mapping at the specified virtual address.
 *
 * @param as    Target address space.
 * @param vaddr Desired virtual address; may be 0 for kernel choice.
 * @param frame Frame capability to map (unused placeholder).
 * @param prot  Requested memory protection.
 * @param flags Mapping flags (unused placeholder).
 * @return KERN_SUCCESS on success, otherwise KERN_FAILURE.
 */
kern_return_t map_frame(aspace_t *as, vm_offset_t vaddr, mach_port_t frame, vm_prot_t prot,
                        int flags) {
    (void)as;
    (void)frame;
    (void)flags;

    size_t psz = (size_t)sysconf(_SC_PAGESIZE);
    void *res = mmap(vaddr ? (void *)vaddr : NULL, psz, prot,
                     MAP_ANON | MAP_PRIVATE | (vaddr ? MAP_FIXED : 0), -1, 0);
    return res == MAP_FAILED ? KERN_FAILURE : KERN_SUCCESS;
}

/**
 * @brief Unmap a previously mapped frame.
 *
 * @param as    Address space owning the mapping.
 * @param vaddr Virtual address of the mapping to remove.
 * @return KERN_SUCCESS on success, otherwise KERN_FAILURE.
 */
kern_return_t unmap_frame(aspace_t *as, vm_offset_t vaddr) {
    (void)as;
    size_t psz = (size_t)sysconf(_SC_PAGESIZE);
    if (munmap((void *)vaddr, psz) == -1)
        return KERN_FAILURE;
    return KERN_SUCCESS;
}
