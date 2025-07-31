#include "posix_ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * \brief Allocate and initialise a new capability structure.
 *
 * The returned object has all relationship pointers cleared and the given
 * rights applied. The caller becomes responsible for freeing the capability
 * via \c free().
 *
 * \param rights Bit mask of permitted operations.
 * \return Pointer to the newly allocated capability or NULL on failure.
 */
static struct cap *cap_create(unsigned long rights)
{
    struct cap *cap = malloc(sizeof(*cap));
    if (!cap) {
        return NULL;
    }

    cap->parent = NULL;
    cap->children = NULL;
    cap->next_sibling = NULL;
    cap->epoch = 0;
    cap->rights = rights;
    cap->flags = 0;
    return cap;
}

/* ------------------------ Message queues ------------------------ */
/**
 * \brief Open or create a POSIX message queue capability.
 *
 * Allocates a new capability and associates it with a message queue
 * obtained via \c mq_open(). A default maximum of eight messages is used
 * when \p maxmsg is zero and each message is limited to 128 bytes.
 *
 * \param name   Name of the queue.
 * \param oflag  Flags passed to \c mq_open().
 * \param mode   Permissions used when creating the queue.
 * \param maxmsg Requested maximum number of messages.
 * \return Pointer to a new capability queue or NULL on failure.
 */
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
    q->cap = cap_create(MQ_RIGHT_SEND | MQ_RIGHT_RECV | MQ_RIGHT_CLOSE);
    if (!q->cap) {
        mq_close(q->mq);
        free(q);
        return NULL;
    }
    return q;
}

/**
 * \brief Send a message through a capability queue.
 *
 * Performs a rights check before delegating to \c mq_send().
 *
 * \param q    Capability queue handle.
 * \param msg  Pointer to the message buffer.
 * \param len  Length of the message in bytes.
 * \param prio Message priority.
 * \return Zero on success or -1 on failure with \c errno set.
 */
int cap_mq_send(cap_mq_t *q, const char *msg, size_t len, unsigned int prio)
{
    if (!q || !q->cap || !(q->cap->rights & MQ_RIGHT_SEND)) {
        errno = EPERM;
        return -1;
    }
    return mq_send(q->mq, msg, len, prio);
}

/**
 * \brief Receive a message from a capability queue.
 *
 * Checks that the queue grants receive rights before calling \c mq_receive().
 *
 * \param q    Capability queue handle.
 * \param msg  Buffer to store the received message.
 * \param len  Size of \p msg in bytes.
 * \param prio Optional destination for the message priority.
 * \return Number of bytes received or -1 on failure with \c errno set.
 */
ssize_t cap_mq_receive(cap_mq_t *q, char *msg, size_t len, unsigned int *prio)
{
    if (!q || !q->cap || !(q->cap->rights & MQ_RIGHT_RECV)) {
        errno = EPERM;
        return -1;
    }
    return mq_receive(q->mq, msg, len, prio);
}

/**
 * \brief Close a capability message queue.
 *
 * The underlying queue is closed only when the capability grants that right.
 * All allocated resources are then released.
 *
 * \param q Queue to close.
 * \return Zero on success or -1 if \p q is NULL.
 */
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
/**
 * \brief Open or create a POSIX semaphore capability.
 *
 * Wraps \c sem_open() and attaches a new capability granting wait, post and
 * close rights.
 *
 * \param name  Name of the semaphore.
 * \param oflag Flags passed to \c sem_open().
 * \param mode  Permissions used when creating the semaphore.
 * \param value Initial value if the semaphore is created.
 * \return Pointer to a new semaphore capability or NULL on failure.
 */
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
    s->cap = cap_create(SEM_RIGHT_WAIT | SEM_RIGHT_POST | SEM_RIGHT_CLOSE);
    if (!s->cap) {
        sem_close(s->sem);
        free(s);
        return NULL;
    }
    return s;
}

/**
 * \brief Wait on a semaphore capability.
 *
 * Calls \c sem_wait() after verifying the semaphore allows waiting.
 *
 * \param s Semaphore capability handle.
 * \return Zero on success or -1 on failure with \c errno set.
 */
int cap_sem_wait(cap_sem_t *s)
{
    if (!s || !s->cap || !(s->cap->rights & SEM_RIGHT_WAIT)) {
        errno = EPERM;
        return -1;
    }
    return sem_wait(s->sem);
}

/**
 * \brief Post to a semaphore capability.
 *
 * Verifies posting rights before invoking \c sem_post().
 *
 * \param s Semaphore capability handle.
 * \return Zero on success or -1 on failure with \c errno set.
 */
int cap_sem_post(cap_sem_t *s)
{
    if (!s || !s->cap || !(s->cap->rights & SEM_RIGHT_POST)) {
        errno = EPERM;
        return -1;
    }
    return sem_post(s->sem);
}

/**
 * \brief Close a semaphore capability.
 *
 * Closes the underlying semaphore if permitted and releases allocated
 * resources.
 *
 * \param s Semaphore to close.
 * \return Zero on success or -1 if \p s is NULL.
 */
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
/**
 * \brief Open or create a POSIX shared memory capability.
 *
 * The shared memory object is opened via \c shm_open() and optionally resized
 * with \c ftruncate() when \p oflag includes \c O_CREAT. A capability granting
 * mapping, unmapping and close rights is then attached.
 *
 * \param name  Name of the shared memory object.
 * \param oflag Flags passed to \c shm_open().
 * \param mode  Permissions used when creating the object.
 * \param size  Initial size when creating the object.
 * \return Pointer to a new shared memory capability or NULL on failure.
 */
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
    shm->cap = cap_create(SHM_RIGHT_MAP | SHM_RIGHT_UNMAP | SHM_RIGHT_CLOSE);
    if (!shm->cap) {
        close(shm->fd);
        free(shm);
        return NULL;
    }
    return shm;
}

/**
 * \brief Map a shared memory capability.
 *
 * Invokes \c mmap() after confirming the capability permits mapping.
 *
 * \param shm   Shared memory capability handle.
 * \param len   Length of the mapping.
 * \param prot  Memory protection flags.
 * \param flags Mapping flags passed to \c mmap().
 * \param off   Offset within the object.
 * \return Pointer to the mapped region or \c MAP_FAILED on error.
 */
void *cap_shm_map(cap_shm_t *shm, size_t len, int prot, int flags, off_t off)
{
    if (!shm || !shm->cap || !(shm->cap->rights & SHM_RIGHT_MAP)) {
        errno = EPERM;
        return MAP_FAILED;
    }
    return mmap(NULL, len, prot, flags, shm->fd, off);
}

/**
 * \brief Unmap a region of shared memory.
 *
 * \param addr Address previously returned by \c cap_shm_map().
 * \param len  Length of the mapping.
 * \return Zero on success or -1 on failure with \c errno set.
 */
int cap_shm_unmap(void *addr, size_t len)
{
    return munmap(addr, len);
}

/**
 * \brief Close a shared memory capability.
 *
 * Closes the underlying file descriptor when permitted and frees the
 * associated capability structure.
 *
 * \param shm Shared memory capability to close.
 * \return Zero on success or -1 if \p shm is NULL.
 */
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

