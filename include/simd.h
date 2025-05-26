#pragma once
#include <stddef.h>
#ifdef __AVX2__
#define LITES_SIMD_AVX2 1
#else
#define LITES_SIMD_AVX2 0
#endif
#ifdef __SSE2__
#define LITES_SIMD_SSE2 1
#else
#define LITES_SIMD_SSE2 0
#endif
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define LITES_SIMD_NEON 1
#else
#define LITES_SIMD_NEON 0
#endif
#ifdef __cplusplus
extern "C" {
#endif
int simd_bcmp(const void *a, const void *b, size_t len);
#ifdef __cplusplus
}
#endif

#define UNROLL8(stmt)                                                                              \
    do {                                                                                           \
        stmt;                                                                                      \
        stmt;                                                                                      \
        stmt;                                                                                      \
        stmt;                                                                                      \
        stmt;                                                                                      \
        stmt;                                                                                      \
        stmt;                                                                                      \
        stmt;                                                                                      \
    } while (0)

#ifdef ENABLE_PROFILING
#include <time.h>
static inline long profiler_ticks(void) { return clock(); }
#define PROF_START(var) long var##_start = profiler_ticks();
#define PROF_END(var)                                                                              \
    long var##_time = profiler_ticks() - var##_start;                                              \
    (void)var##_time
#else
#define PROF_START(var)
#define PROF_END(var)
#endif
