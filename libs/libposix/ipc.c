#include "posix_ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct cap *cap_alloc(unsigned long rights)
{
    struct cap *c = malloc(sizeof(*c));
    if (!c)
        return NULL;
    c->parent = NULL;
    c->children = NULL;
    c->next_sibling = NULL;
    c->epoch = 0;
    c->rights = rights;
    c->flags = 0;
    return c;
}

/* ------------------------ Message queues ------------------------ */
cap_mq_t *cap_mq_open(const char *name, int oflag, mode_t mode,
                      unsigned int maxmsg)
{
    cap_mq_t *q = malloc(sizeof(*q));
    if (!q)
        return NULL;
    struct mq_attr attr = {0};
    if (maxmsg)
        attr.mq_maxmsg = maxmsg;
    if (attr.mq_maxmsg == 0)
        attr.mq_maxmsg = 8;
    attr.mq_msgsize = 128;
    q->mq = mq_open(name, oflag, mode, &attr);
    if (q->mq == (mqd_t)-1) {
        free(q);
        return NULL;
    }
    q->cap = cap_alloc(MQ_RIGHT_SEND | MQ_RIGHT_RECV | MQ_RIGHT_CLOSE);
    if (!q->cap) {
        mq_close(q->mq);
        free(q);
        return NULL;
    }
    return q;
}

int cap_mq_send(cap_mq_t *q, const char *msg, size_t len, unsigned int prio)
{
    if (!q || !q->cap || !(q->cap->rights & MQ_RIGHT_SEND)) {
        errno = EPERM;
        return -1;
    }
    return mq_send(q->mq, msg, len, prio);
}

ssize_t cap_mq_receive(cap_mq_t *q, char *msg, size_t len, unsigned int *prio)
{
    if (!q || !q->cap || !(q->cap->rights & MQ_RIGHT_RECV)) {
        errno = EPERM;
        return -1;
    }
    return mq_receive(q->mq, msg, len, prio);
}

int cap_mq_close(cap_mq_t *q)
{
    if (!q)
        return -1;
    if (q->cap && (q->cap->rights & MQ_RIGHT_CLOSE))
        mq_close(q->mq);
    free(q->cap);
    free(q);
    return 0;
}

/* ------------------------ Semaphores ------------------------ */
cap_sem_t *cap_sem_open(const char *name, int oflag, mode_t mode,
                        unsigned int value)
{
    cap_sem_t *s = malloc(sizeof(*s));
    if (!s)
        return NULL;
    s->sem = sem_open(name, oflag, mode, value);
    if (s->sem == SEM_FAILED) {
        free(s);
        return NULL;
    }
    s->cap = cap_alloc(SEM_RIGHT_WAIT | SEM_RIGHT_POST | SEM_RIGHT_CLOSE);
    if (!s->cap) {
        sem_close(s->sem);
        free(s);
        return NULL;
    }
    return s;
}

int cap_sem_wait(cap_sem_t *s)
{
    if (!s || !s->cap || !(s->cap->rights & SEM_RIGHT_WAIT)) {
        errno = EPERM;
        return -1;
    }
    return sem_wait(s->sem);
}

int cap_sem_post(cap_sem_t *s)
{
    if (!s || !s->cap || !(s->cap->rights & SEM_RIGHT_POST)) {
        errno = EPERM;
        return -1;
    }
    return sem_post(s->sem);
}

int cap_sem_close(cap_sem_t *s)
{
    if (!s)
        return -1;
    if (s->cap && (s->cap->rights & SEM_RIGHT_CLOSE))
        sem_close(s->sem);
    free(s->cap);
    free(s);
    return 0;
}

/* ------------------------ Shared memory ------------------------ */
cap_shm_t *cap_shm_open(const char *name, int oflag, mode_t mode, size_t size)
{
    cap_shm_t *shm = malloc(sizeof(*shm));
    if (!shm)
        return NULL;
    shm->fd = shm_open(name, oflag, mode);
    if (shm->fd < 0) {
        free(shm);
        return NULL;
    }
    if ((oflag & O_CREAT) && size > 0) {
        if (ftruncate(shm->fd, (off_t)size) < 0) {
            close(shm->fd);
            free(shm);
            return NULL;
        }
    }
    shm->cap = cap_alloc(SHM_RIGHT_MAP | SHM_RIGHT_UNMAP | SHM_RIGHT_CLOSE);
    if (!shm->cap) {
        close(shm->fd);
        free(shm);
        return NULL;
    }
    return shm;
}

void *cap_shm_map(cap_shm_t *shm, size_t len, int prot, int flags, off_t off)
{
    if (!shm || !shm->cap || !(shm->cap->rights & SHM_RIGHT_MAP)) {
        errno = EPERM;
        return MAP_FAILED;
    }
    return mmap(NULL, len, prot, flags, shm->fd, off);
}

int cap_shm_unmap(void *addr, size_t len)
{
    return munmap(addr, len);
}

int cap_shm_close(cap_shm_t *shm)
{
    if (!shm)
        return -1;
    if (shm->cap && (shm->cap->rights & SHM_RIGHT_CLOSE))
        close(shm->fd);
    free(shm->cap);
    free(shm);
    return 0;
}

