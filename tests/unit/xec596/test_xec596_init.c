#include <assert.h>
#include <stdint.h>

static inline uint32_t xec596_read32(volatile void *base, unsigned off)
{
    return *((volatile uint32_t *)((volatile char *)base + off));
}

static inline void xec596_write32(volatile void *base, unsigned off, uint32_t val)
{
    *((volatile uint32_t *)((volatile char *)base + off)) = val;
}

int main(void) {
    uint32_t regs[4] = {0};
    xec596_write32(regs, 4, 0x12345678);
    assert(xec596_read32(regs, 4) == 0x12345678);
    return 0;
}
