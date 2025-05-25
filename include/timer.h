#define _POSIX_C_SOURCE 200809L
#pragma once

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

/* Initialize timer tracking. Call once at startup. */
void timer_init(void);

/* Sleep for the requested interval, tracking per-process usage. */
int k_nanosleep(const struct timespec *req, struct timespec *rem);

/* Return total nanoseconds slept by the given process. */
uint64_t timer_get_total_ns(pid_t pid);

/* Reset the accumulated sleep time for the given process. */
void timer_reset(pid_t pid);

