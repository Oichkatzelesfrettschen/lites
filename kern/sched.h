#ifndef LITES_SCHED_H
#define LITES_SCHED_H

#include <cthreads.h>

void schedule_enqueue(cthread_t thread);
void schedule_dequeue(cthread_t thread);
void *scheduler_loop(void *arg);
void scheduler_init(int num_cores);

#endif /* LITES_SCHED_H */
