#ifndef LITES_PROFILE_H
#define LITES_PROFILE_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    const char *name;
    uint64_t calls;
    uint64_t total_ns;
} profile_entry_t;

#define PROFILE_DECLARE(name) \
    static profile_entry_t profile_##name = {#name, 0, 0};

#define PROFILE_START(name)                                            \
    struct timespec __start_##name;                                    \
    clock_gettime(CLOCK_MONOTONIC, &__start_##name);                   \
    profile_##name.calls++;

#define PROFILE_END(name)                                              \
    do {                                                               \
        struct timespec __end_##name;                                  \
        clock_gettime(CLOCK_MONOTONIC, &__end_##name);                 \
        profile_##name.total_ns +=                                     \
            (uint64_t)(__end_##name.tv_sec - __start_##name.tv_sec) * 1000000000ULL + \
            (uint64_t)(__end_##name.tv_nsec - __start_##name.tv_nsec); \
    } while (0)

#define PROFILE_REPORT(name)                                           \
    do {                                                               \
        printf("%s: %lu calls, %llu ns\n", profile_##name.name,          \
               (unsigned long)profile_##name.calls,                    \
               (unsigned long long)profile_##name.total_ns);           \
    } while (0)

#endif /* LITES_PROFILE_H */
