#include "runqueue.h"
#include <stdlib.h>
#include <sys/queue.h>

static struct run_queue runq;

void runqueue_init(void)
{
    TAILQ_INIT(&runq.queue);
    spin_lock_init(&runq.lock);
}

void setrunqueue(struct proc *p)
{
    struct runq_entry *e = malloc(sizeof(*e));
    if (!e)
        return;
    e->p = p;
    spin_lock(&runq.lock);
    TAILQ_INSERT_TAIL(&runq.queue, e, link);
    spin_unlock(&runq.lock);
}

void remrq(struct proc *p)
{
    spin_lock(&runq.lock);
    struct runq_entry *it;
    TAILQ_FOREACH(it, &runq.queue, link) {
        if (it->p == p) {
            TAILQ_REMOVE(&runq.queue, it, link);
            spin_unlock(&runq.lock);
            free(it);
            return;
        }
    }
    spin_unlock(&runq.lock);
}
