#define _POSIX_C_SOURCE 200809L
#include "timer.h"
#include "spinlock.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct proc_timer {
    pid_t pid;
    uint64_t total_ns;
    struct proc_timer *next;
};

static struct proc_timer *timers;
static spinlock_t timer_lock;

static struct proc_timer *find_timer(pid_t pid)
{
    for (struct proc_timer *it = timers; it; it = it->next) {
        if (it->pid == pid)
            return it;
    }
    return NULL;
}

static struct proc_timer *ensure_timer(pid_t pid)
{
    struct proc_timer *pt = find_timer(pid);
    if (!pt) {
        pt = malloc(sizeof(*pt));
        if (!pt)
            return NULL;
        pt->pid = pid;
        pt->total_ns = 0;
        pt->next = timers;
        timers = pt;
    }
    return pt;
}

void timer_init(void)
{
    spin_lock_init(&timer_lock);
}

void timer_reset(pid_t pid)
{
    spin_lock(&timer_lock);
    struct proc_timer *pt = ensure_timer(pid);
    if (pt)
        pt->total_ns = 0;
    spin_unlock(&timer_lock);
}

uint64_t timer_get_total_ns(pid_t pid)
{
    spin_lock(&timer_lock);
    struct proc_timer *pt = find_timer(pid);
    uint64_t ns = pt ? pt->total_ns : 0;
    spin_unlock(&timer_lock);
    return ns;
}

int k_nanosleep(const struct timespec *req, struct timespec *rem)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int ret = nanosleep(req, rem);
    clock_gettime(CLOCK_MONOTONIC, &end);

    uint64_t delta = (uint64_t)(end.tv_sec - start.tv_sec) * 1000000000ULL +
                     (uint64_t)(end.tv_nsec - start.tv_nsec);
    pid_t pid = getpid();
    spin_lock(&timer_lock);
    struct proc_timer *pt = ensure_timer(pid);
    if (pt)
        pt->total_ns += delta;
    spin_unlock(&timer_lock);
    return ret;
}

