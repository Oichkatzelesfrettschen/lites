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
 * @brief Handle a page fault that occurs within an address space.
 *
 * When a fault is received, an optional user-space pager is notified and a
 * single writable page is mapped to satisfy the access. The current
 * implementation is intentionally minimal and intended as a placeholder for a
 * richer pager-driven VM system.
 *
 * @param as   Address space receiving the fault. Must not be @c NULL.
 * @param addr Virtual address that triggered the fault.
 * @param prot Desired protection bits for the new mapping.
 * @return ::KERN_SUCCESS on success; otherwise ::KERN_FAILURE.
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
 * @brief Map a physical frame into the given address space.
 *
 * Establishes a mapping at the supplied virtual address. In this simplified
 * library build, the @p frame and @p flags arguments are placeholders and an
 * anonymous page is allocated instead.
 *
 * @param as    Address space receiving the mapping.
 * @param vaddr Preferred virtual address or @c 0 to let the kernel choose.
 * @param frame Capability designating the physical frame to map (unused).
 * @param prot  Requested protection flags.
 * @param flags Additional mapping flags (unused).
 * @return ::KERN_SUCCESS on success; otherwise ::KERN_FAILURE.
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
 * @brief Remove a previously established mapping.
 *
 * This helper simply unmaps a single page starting at @p vaddr.
 *
 * @param as    Address space owning the mapping (unused).
 * @param vaddr Virtual address of the mapping to remove.
 * @return ::KERN_SUCCESS on success; otherwise ::KERN_FAILURE.
 */
kern_return_t unmap_frame(aspace_t *as, vm_offset_t vaddr) {
    (void)as;
    size_t psz = (size_t)sysconf(_SC_PAGESIZE);
    if (munmap((void *)vaddr, psz) == -1)
        return KERN_FAILURE;
    return KERN_SUCCESS;
}
