#ifndef LITES_SCHED_H
#define LITES_SCHED_H

#include <cthreads.h>
#include "spinlock.h"

/* Simple MCS lock implementation used by the scheduler. */
typedef struct mcs_lock_node {
    struct mcs_lock_node *next;
    volatile int locked;
} mcs_lock_node_t;

typedef struct mcs_lock {
    mcs_lock_node_t *volatile tail;
} mcs_lock_t;

void mcs_lock_init(mcs_lock_t *lock);
void mcs_lock_acquire(mcs_lock_t *lock, mcs_lock_node_t *node);
void mcs_lock_release(mcs_lock_t *lock, mcs_lock_node_t *node);

void schedule_enqueue(cthread_t thread);
void schedule_dequeue(cthread_t thread);
void *scheduler_loop(void *arg);
void scheduler_init(int num_cores);

extern spinlock_t sched_lock;

#endif /* LITES_SCHED_H */
