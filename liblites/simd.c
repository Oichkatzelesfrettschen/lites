#include "simd.h"
#include <string.h>
#if LITES_SIMD_AVX2
#include <immintrin.h>
#endif
#if LITES_SIMD_SSE2
#include <emmintrin.h>
#endif
#if LITES_SIMD_NEON
#include <arm_neon.h>
#endif

int simd_bcmp(const void *a, const void *b, size_t len) {
#if LITES_SIMD_AVX2
    const unsigned char *p1 = a, *p2 = b;
    size_t i = 0;
    for (; i + 32 <= len; i += 32) {
        __m256i va = _mm256_loadu_si256((const __m256i *)(p1 + i));
        __m256i vb = _mm256_loadu_si256((const __m256i *)(p2 + i));
        __m256i cmp = _mm256_cmpeq_epi8(va, vb);
        if (_mm256_movemask_epi8(cmp) != -1)
            return memcmp(p1 + i, p2 + i, len - i);
    }
    return memcmp(p1 + i, p2 + i, len - i);
#elif LITES_SIMD_SSE2
    const unsigned char *p1 = a, *p2 = b;
    size_t i = 0;
    for (; i + 16 <= len; i += 16) {
        __m128i va = _mm_loadu_si128((const __m128i *)(p1 + i));
        __m128i vb = _mm_loadu_si128((const __m128i *)(p2 + i));
        __m128i cmp = _mm_cmpeq_epi8(va, vb);
        if (_mm_movemask_epi8(cmp) != 0xFFFF)
            return memcmp(p1 + i, p2 + i, len - i);
    }
    return memcmp(p1 + i, p2 + i, len - i);
#elif LITES_SIMD_NEON
    const unsigned char *p1 = a, *p2 = b;
    size_t i = 0;
    for (; i + 16 <= len; i += 16) {
        uint8x16_t va = vld1q_u8(p1 + i);
        uint8x16_t vb = vld1q_u8(p2 + i);
        uint8x16_t cmp = vceqq_u8(va, vb);
        if (vgetq_lane_u64(vreinterpretq_u64_u8(cmp), 0) != ~0ULL ||
            vgetq_lane_u64(vreinterpretq_u64_u8(cmp), 1) != ~0ULL)
            return memcmp(p1 + i, p2 + i, len - i);
    }
    return memcmp(p1 + i, p2 + i, len - i);
#else
    return memcmp(a, b, len);
#endif
}
