#ifndef LITES_ARCH_X86_64_V1_TLB_H
#define LITES_ARCH_X86_64_V1_TLB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file tlb.h
 * @brief Architecture-specific TLB invalidation primitives.
 *
 * Only x86_64 implementations are currently provided. Architectures such
 * as aarch64, riscv64 or powerpc64 remain unsupported and must supply
 * their own definitions or treat the functions as no-ops.
 */

/**
 * @brief Invalidate the entire translation lookaside buffer.
 */
void arch_tlb_invalidate_all(void);

/**
 * @brief Invalidate the translation for a specific linear address.
 *
 * @param addr Linear address whose TLB entry shall be removed.
 */
void arch_tlb_invalidate_page(uintptr_t addr);

#ifdef __cplusplus
}
#endif

#endif /* LITES_ARCH_X86_64_V1_TLB_H */
