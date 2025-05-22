#ifndef _GENERIC_BITOPS_H
#define _GENERIC_BITOPS_H

/* Portable C implementations of basic bit operations. */

static inline int set_bit(int nr, void *addr) {
    unsigned char *p = (unsigned char *)addr + (nr >> 3);
    unsigned char mask = (unsigned char)1U << (nr & 7);
    unsigned char old = *p;
    *p = old | mask;
    return (old & mask) != 0;
}

static inline int clear_bit(int nr, void *addr) {
    unsigned char *p = (unsigned char *)addr + (nr >> 3);
    unsigned char mask = (unsigned char)1U << (nr & 7);
    unsigned char old = *p;
    *p = old & ~mask;
    return (old & mask) != 0;
}

static inline int change_bit(int nr, void *addr) {
    unsigned char *p = (unsigned char *)addr + (nr >> 3);
    unsigned char mask = (unsigned char)1U << (nr & 7);
    unsigned char old = *p;
    *p = old ^ mask;
    return (old & mask) != 0;
}

static inline int test_bit(int nr, void *addr) {
    unsigned char *p = (unsigned char *)addr + (nr >> 3);
    return ((*p >> (nr & 7)) & 1U) != 0;
}

static inline unsigned long ffz(unsigned long word) {
    for (unsigned long i = 0; i < sizeof(unsigned long) * 8; i++) {
        if (!(word & (1UL << i)))
            return i;
    }
    return sizeof(unsigned long) * 8;
}

static inline int find_first_zero_bit(void *addr, unsigned size) {
    unsigned long *p = (unsigned long *)addr;
    unsigned bits = size;
    unsigned index = 0;
    while (bits >= sizeof(unsigned long) * 8) {
        if (~(*p))
            return index + ffz(*p);
        p++;
        index += sizeof(unsigned long) * 8;
        bits -= sizeof(unsigned long) * 8;
    }
    if (bits) {
        unsigned long last = *p | (~0UL << bits);
        if (~last)
            return index + ffz(last);
        index += bits;
    }
    return index;
}

static inline int find_next_zero_bit(void *addr, int size, int offset) {
    for (int bit = offset; bit < size; bit++) {
        if (!test_bit(bit, addr))
            return bit;
    }
    return size;
}

#endif /* _GENERIC_BITOPS_H */
