
#ifndef _X86_64_BITOPS_H
#define _X86_64_BITOPS_H

/* 64-bit bit operations implemented using GCC built-ins. */

static inline int set_bit(int nr, void *addr)
{
    unsigned long *p = (unsigned long *)addr + (nr >> 6);
    unsigned long mask = 1UL << (nr & 63);
    unsigned long old = __atomic_fetch_or(p, mask, __ATOMIC_SEQ_CST);
    return (old & mask) != 0;
}

static inline int clear_bit(int nr, void *addr)
{
    unsigned long *p = (unsigned long *)addr + (nr >> 6);
    unsigned long mask = 1UL << (nr & 63);
    unsigned long old = __atomic_fetch_and(p, ~mask, __ATOMIC_SEQ_CST);
    return (old & mask) != 0;
}

static inline int change_bit(int nr, void *addr)
{
    unsigned long *p = (unsigned long *)addr + (nr >> 6);
    unsigned long mask = 1UL << (nr & 63);
    unsigned long old, new;
    do {
        old = __atomic_load_n(p, __ATOMIC_SEQ_CST);
        new = old ^ mask;
    } while (!__atomic_compare_exchange_n(p, &old, new, 0,
                                          __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
    return (old & mask) != 0;
}

static inline int test_bit(int nr, void *addr)
{
    unsigned long *p = (unsigned long *)addr + (nr >> 6);
    unsigned long mask = 1UL << (nr & 63);
    return (__atomic_load_n(p, __ATOMIC_SEQ_CST) & mask) != 0;
}

static inline unsigned long ffz(unsigned long word)
{
    return __builtin_ffsl(~word) - 1;
}

static inline int find_first_zero_bit(void *addr, unsigned size)
{
    unsigned long *p = addr;
    unsigned long idx = 0;

    while (size >= 64) {
        if (*p != ~0UL)
            return idx + ffz(*p);
        p++;
        idx += 64;
        size -= 64;
    }

    if (size) {
        unsigned long last = *p | (~0UL << size);
        if (last != ~0UL)
            return idx + ffz(last);
        idx += size;
    }

    return idx;
}

static inline int find_next_zero_bit(void *addr, int size, int offset)
{
    int bit = offset;
    while (bit < size) {
        if (!test_bit(bit, addr))
            return bit;
        bit++;
    }
    return size;
}

static inline char *memscan(void *addr, unsigned char c, int size)
{
    unsigned char *p = addr;
    while (size--) {
        if (*p == c)
            return (char *)p;
        p++;
    }
    return (char *)p;
}

#endif /* _X86_64_BITOPS_H */
