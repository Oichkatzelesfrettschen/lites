#pragma once

#include <sys/queue.h>
#include <sys/proc.h>
#include "spinlock.h"

struct runq_entry {
    struct proc *p;
    TAILQ_ENTRY(runq_entry) link;
};

struct run_queue {
    TAILQ_HEAD(, runq_entry) queue;
    spinlock_t lock;
};

void runqueue_init(void);
void setrunqueue(struct proc *p);
void remrq(struct proc *p);

