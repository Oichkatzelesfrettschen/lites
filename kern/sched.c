#include "sched.h"
#include "spinlock.h"
#include <mach/cthreads.h>
#include <mach/mach.h>
#include <time.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef CONFIG_SCHED_MULTICORE
#define CONFIG_SCHED_MULTICORE 0
#endif

#ifndef MAX_CPUS
#define MAX_CPUS 8
#endif

/* ---------------------------------------------------------------------
 * MCS lock implementation
 * --------------------------------------------------------------------- */

void mcs_lock_init(mcs_lock_t *lock) { lock->tail = NULL; }

void mcs_lock_acquire(mcs_lock_t *lock, mcs_lock_node_t *node) {
    node->next = NULL;
    node->locked = 0;
    mcs_lock_node_t *pred = __sync_lock_test_and_set(&lock->tail, node);
    if (pred) {
        node->locked = 1;
        __sync_synchronize();
        pred->next = node;
        while (node->locked)
            cthread_yield();
    }
}

void mcs_lock_release(mcs_lock_t *lock, mcs_lock_node_t *node) {
    if (!node->next) {
        if (__sync_bool_compare_and_swap(&lock->tail, node, NULL))
            return;
        while (!node->next)
            cthread_yield();
    }
    node->next->locked = 0;
}

struct thread_entry {
    cthread_t thread;
    TAILQ_ENTRY(thread_entry) link;
};

struct run_queue {
    TAILQ_HEAD(, thread_entry) queue;
    spinlock_t lock;
};

spinlock_t sched_lock;
static struct run_queue run_queues[MAX_CPUS];
static int sched_num_cpus = 1;
static int next_cpu = 0;

static struct thread_entry *attempt_work_steal(int cpu);

void scheduler_init(int num_cores) {
    if (num_cores < 1)
        num_cores = 1;
    if (num_cores > MAX_CPUS)
        num_cores = MAX_CPUS;
    sched_num_cpus = num_cores;
    spin_lock_init(&sched_lock);
    for (int i = 0; i < sched_num_cpus; ++i) {
        TAILQ_INIT(&run_queues[i].queue);
        spin_lock_init(&run_queues[i].lock);
    }
#if CONFIG_SCHED_MULTICORE
    for (int i = 0; i < sched_num_cpus; ++i) {
        cthread_detach(cthread_fork((cthread_fn_t)scheduler_loop, (void *)(long)i));
    }
#endif
}

void schedule_enqueue(cthread_t thread) {
    struct thread_entry *te = malloc(sizeof(*te));
    if (!te)
        return;
    te->thread = thread;
    int cpu = 0;
#if CONFIG_SCHED_MULTICORE
    cpu = __sync_fetch_and_add(&next_cpu, 1) % sched_num_cpus;
#endif
    spin_lock(&run_queues[cpu].lock);
    TAILQ_INSERT_TAIL(&run_queues[cpu].queue, te, link);
    spin_unlock(&run_queues[cpu].lock);
}

void schedule_dequeue(cthread_t thread) {
    for (int cpu = 0; cpu < sched_num_cpus; ++cpu) {
        spin_lock(&run_queues[cpu].lock);
        struct thread_entry *it;
        TAILQ_FOREACH(it, &run_queues[cpu].queue, link) {
            if (it->thread == thread) {
                TAILQ_REMOVE(&run_queues[cpu].queue, it, link);
                spin_unlock(&run_queues[cpu].lock);
                free(it);
                return;
            }
        }
        spin_unlock(&run_queues[cpu].lock);
    }
}

static struct thread_entry *attempt_work_steal(int cpu) {
    for (int i = 0; i < sched_num_cpus; ++i) {
        if (i == cpu)
            continue;
        spin_lock(&run_queues[i].lock);
        struct thread_entry *it = TAILQ_FIRST(&run_queues[i].queue);
        if (it) {
            TAILQ_REMOVE(&run_queues[i].queue, it, link);
            spin_unlock(&run_queues[i].lock);
            return it;
        }
        spin_unlock(&run_queues[i].lock);
    }
    return NULL;
}

void *scheduler_loop(void *arg) {
    int cpu = (int)(long)arg;
    while (1) {
        struct thread_entry *te = NULL;
        spin_lock(&run_queues[cpu].lock);
        te = TAILQ_FIRST(&run_queues[cpu].queue);
        if (te)
            TAILQ_REMOVE(&run_queues[cpu].queue, te, link);
        spin_unlock(&run_queues[cpu].lock);

        if (!te) {
            te = attempt_work_steal(cpu);
            if (!te) {
                struct timespec idle = {0, 1000000};
                nanosleep(&idle, NULL);
                continue;
            }
        }

        /*
         * There is no direct user level context switch interface
         * available here.  Simply yield in the hope that the newly
         * queued thread will run.
         */
        free(te);
        /* timer based preemption */
        struct timespec slice = {0, 10000000};
        nanosleep(&slice, NULL);
        cthread_yield();
    }
    return NULL;
}
