#include "ipc_queue.h"
#include <string.h>
#include <time.h>

/* Global mailbox used by tests */
struct mailbox ipcs;
static int ipcs_init_done;
static struct timespec timed[MAILBOX_BUFSZ];

static void ensure_ipcs_init(void)
{
    if (!ipcs_init_done) {
        ipc_queue_init(&ipcs);
        ipcs_init_done = 1;
    }
}

void ipc_queue_init(struct mailbox *mb)
{
    spin_lock_init(&mb->lock);
    mb->head = 0;
    mb->tail = 0;
}

int kernel_ipc_queue_send(struct mailbox *mb, const exo_msg_t *msg)
{
    int ret = EXO_SUCCESS;
    spin_lock(&mb->lock);
    int next = (mb->head + 1) % MAILBOX_BUFSZ;
    if (next == mb->tail) {
        ret = EXO_OVERFLOW;
    } else {
        int idx = mb->head;
        mb->buf[idx] = *msg;
        clock_gettime(CLOCK_MONOTONIC, &timed[idx]);
        mb->head = next;
    }
    spin_unlock(&mb->lock);
    return ret;
}

static int queue_recv(struct mailbox *mb, exo_msg_t *out)
{
    if (mb->tail == mb->head)
        return EXO_TIMEOUT;
    *out = mb->buf[mb->tail];
    mb->tail = (mb->tail + 1) % MAILBOX_BUFSZ;
    return EXO_SUCCESS;
}

int kernel_ipc_queue_recv_timed(struct mailbox *mb, exo_msg_t *out, unsigned int timeout_ms)
{
    if (mb == &ipcs)
        ensure_ipcs_init();
    struct timespec start, now, wait = {0, 1000000};
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (1) {
        spin_lock(&mb->lock);
        int ret = queue_recv(mb, out);
        spin_unlock(&mb->lock);
        if (ret == EXO_SUCCESS)
            return EXO_SUCCESS;
        if (!timeout_ms)
            return EXO_TIMEOUT;
        clock_gettime(CLOCK_MONOTONIC, &now);
        unsigned elapsed = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
        if (timeout_ms != (unsigned)-1 && elapsed >= timeout_ms)
            return EXO_TIMEOUT;
        nanosleep(&wait, NULL);
    }
}
