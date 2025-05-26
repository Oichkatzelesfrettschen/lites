#ifndef LITES_SIMD_UTILS_H
#define LITES_SIMD_UTILS_H

#include <stddef.h>
#include "simd.h"

static inline int simd_bcmp(const void *a, const void *b, size_t len) {
#if LITES_HAVE_AVX2
    const unsigned char *pa = a, *pb = b;
    while (len >= 32) {
        __m256i va = _mm256_loadu_si256((const __m256i*)pa);
        __m256i vb = _mm256_loadu_si256((const __m256i*)pb);
        __m256i cmp = _mm256_cmpeq_epi8(va, vb);
        if (_mm256_movemask_epi8(cmp) != -1)
            return 1;
        pa += 32; pb += 32; len -= 32;
    }
#endif
#if LITES_HAVE_SSE
    const unsigned char *pa = a, *pb = b;
    while (len >= 16) {
        __m128i va = _mm_loadu_si128((const __m128i*)pa);
        __m128i vb = _mm_loadu_si128((const __m128i*)pb);
        __m128i cmp = _mm_cmpeq_epi8(va, vb);
        if (_mm_movemask_epi8(cmp) != 0xffff)
            return 1;
        pa += 16; pb += 16; len -= 16;
    }
#endif
#if LITES_HAVE_NEON
    const unsigned char *pa = a, *pb = b;
    while (len >= 16) {
        uint8x16_t va = vld1q_u8(pa);
        uint8x16_t vb = vld1q_u8(pb);
        uint8x16_t cmp = vceqq_u8(va, vb);
        if (vminvq_u8(cmp) != 0xff)
            return 1;
        pa += 16; pb += 16; len -= 16;
    }
#endif
    const unsigned char *ca = a, *cb = b;
    while (len--) {
        if (*ca++ != *cb++)
            return 1;
    }
    return 0;
}

#endif /* LITES_SIMD_UTILS_H */
