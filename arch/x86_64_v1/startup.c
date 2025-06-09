#include <stdint.h>

/// CPU ID assigned by boot code.
static uint32_t bootstrap_cpu;

/**
 * Perform early architecture initialisation.
 *
 * Currently this simply records the bootstrap CPU identifier.
 */
void arch_init(void) {
    /* Placeholder implementation. */
    bootstrap_cpu = 0;
}
