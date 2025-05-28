#pragma once
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>

struct cap {
    struct cap *parent;
    struct cap *children;
    struct cap *next_sibling;
    unsigned long rights;
    unsigned int flags;
    unsigned int epoch;
};

/* Message queue capability wrapper */
typedef struct {
    mqd_t mq;
    struct cap *cap;
} cap_mq_t;

#define MQ_RIGHT_SEND  (1u << 0)
#define MQ_RIGHT_RECV  (1u << 1)
#define MQ_RIGHT_CLOSE (1u << 2)

cap_mq_t *cap_mq_open(const char *name, int oflag, mode_t mode, unsigned int maxmsg);
int cap_mq_send(cap_mq_t *q, const char *msg, size_t len, unsigned int prio);
ssize_t cap_mq_receive(cap_mq_t *q, char *msg, size_t len, unsigned int *prio);
int cap_mq_close(cap_mq_t *q);

/* Semaphore capability wrapper */
typedef struct {
    sem_t *sem;
    struct cap *cap;
} cap_sem_t;

#define SEM_RIGHT_WAIT  (1u << 0)
#define SEM_RIGHT_POST  (1u << 1)
#define SEM_RIGHT_CLOSE (1u << 2)

cap_sem_t *cap_sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
int cap_sem_wait(cap_sem_t *s);
int cap_sem_post(cap_sem_t *s);
int cap_sem_close(cap_sem_t *s);

/* Shared memory capability wrapper */
typedef struct {
    int fd;
    struct cap *cap;
} cap_shm_t;

#define SHM_RIGHT_MAP   (1u << 0)
#define SHM_RIGHT_UNMAP (1u << 1)
#define SHM_RIGHT_CLOSE (1u << 2)

cap_shm_t *cap_shm_open(const char *name, int oflag, mode_t mode, size_t size);
void *cap_shm_map(cap_shm_t *shm, size_t len, int prot, int flags, off_t off);
int cap_shm_unmap(void *addr, size_t len);
int cap_shm_close(cap_shm_t *shm);

