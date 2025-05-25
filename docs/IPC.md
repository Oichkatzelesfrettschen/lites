# Interprocess Communication

This document describes the lightweight mailbox mechanism used for user‑level IPC in the modernized sources.

## Per-process mailboxes

Every process owns a small mailbox inside the kernel.  Senders append messages to the destination mailbox while receivers fetch from their own.  Each mailbox holds a fixed number of entries and operates as a circular queue.  When the mailbox is full `exo_send` fails with `EXO_OVERFLOW`.

## Message format

```c
typedef struct {
    uint16_t len;   /* number of bytes in data */
    uint16_t type;  /* application defined */
    pid_t    sender; /* pid of the sender */
} exo_msg_hdr_t;

#define EXO_MSG_DATA_MAX 60

typedef struct {
    exo_msg_hdr_t hdr;
    unsigned char data[EXO_MSG_DATA_MAX];
} exo_msg_t;
```

The header precedes an inline payload of up to `EXO_MSG_DATA_MAX` bytes.  The size field must match the payload length.

## Syscall wrappers

Two simple wrappers provide access to the mailboxes:

```c
int exo_send(pid_t dest, const void *buf, size_t len, unsigned int type);
int exo_recv(exo_msg_t *out, unsigned int timeout_ms);
```

`exo_send` copies `len` bytes from `buf` into the mailbox of `dest`.  The caller supplies an application specific `type`.  On success the call returns `EXO_SUCCESS`.

`exo_recv` waits up to `timeout_ms` milliseconds for the next message.  A zero timeout performs a non‑blocking check while a negative value waits indefinitely.  When a message is available it is stored in `*out` and `EXO_SUCCESS` is returned.  If no message arrives before the timeout elapses the function returns `EXO_TIMEOUT`.

### Status codes

```c
#define EXO_SUCCESS   0  /* operation completed */
#define EXO_TIMEOUT  -1  /* timed out waiting */
#define EXO_OVERFLOW -2  /* receiver mailbox full */
#define EXO_INVALID  -3  /* bad arguments */
```

## Timeout semantics

The timeout passed to `exo_recv` is interpreted in milliseconds.  A value of zero checks the mailbox without blocking.  Negative values block without a limit.  The function wakes as soon as a message is available or when the timeout expires.

## POSIX IPC wrappers

Capability backed wrappers expose POSIX message queues, semaphores and
shared memory.  Each resource is associated with a capability object so
rights can later be refined or revoked.

### Message queues

```c
cap_mq_t *cap_mq_open(const char *name, int oflag, mode_t mode,
                      unsigned int maxmsg);
int cap_mq_send(cap_mq_t *q, const char *msg, size_t len, unsigned int prio);
ssize_t cap_mq_receive(cap_mq_t *q, char *msg, size_t len, unsigned int *prio);
int cap_mq_close(cap_mq_t *q);
```

### Semaphores

```c
cap_sem_t *cap_sem_open(const char *name, int oflag, mode_t mode,
                        unsigned int value);
int cap_sem_wait(cap_sem_t *s);
int cap_sem_post(cap_sem_t *s);
int cap_sem_close(cap_sem_t *s);
```

### Shared memory

```c
cap_shm_t *cap_shm_open(const char *name, int oflag, mode_t mode, size_t size);
void *cap_shm_map(cap_shm_t *shm, size_t len, int prot, int flags, off_t off);
int cap_shm_unmap(void *addr, size_t len);
int cap_shm_close(cap_shm_t *shm);
```

The wrappers merely forward to the underlying POSIX functions while
tracking the capability used to create the object.
