#include "tlb.h"

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64)

/**
 * @brief Invalidate the entire translation lookaside buffer.
 */
void arch_tlb_invalidate_all(void) {
    uintptr_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile("mov %0, %%cr3" ::"r"(cr3) : "memory");
}

/**
 * @brief Invalidate the translation for a specific linear address.
 *
 * @param addr Linear address whose TLB entry shall be removed.
 */
void arch_tlb_invalidate_page(uintptr_t addr) {
    __asm__ volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

#else
#warning "TLB invalidation not supported on this architecture"
/**
 * @brief Fallback no-op for unsupported targets.
 */
void arch_tlb_invalidate_all(void) {}

/**
 * @brief Fallback no-op for unsupported targets.
 *
 * @param addr Ignored.
 */
void arch_tlb_invalidate_page(uintptr_t addr) { (void)addr; }
#endif
