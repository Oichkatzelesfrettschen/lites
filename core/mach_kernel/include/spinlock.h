#pragma once

#include <stdint.h>

/* Basic spin lock used inside the kernel scheduler. */

typedef struct {
#ifdef CONFIG_TICKET_LOCK
    volatile uint32_t next;
    volatile uint32_t owner;
#else
    volatile int locked;
#endif
} spinlock_t;

static inline void spin_lock_init(spinlock_t *l)
{
#ifdef CONFIG_TICKET_LOCK
    l->next = 0;
    l->owner = 0;
#else
    l->locked = 0;
#endif
}

static inline void spin_lock(spinlock_t *l)
{
#ifdef CONFIG_TICKET_LOCK
    uint32_t ticket = __sync_fetch_and_add(&l->next, 1);
    while (__sync_val_compare_and_swap(&l->owner, ticket, ticket) != ticket)
        ;
#else
    while (__sync_lock_test_and_set(&l->locked, 1))
        ;
#endif
}

static inline void spin_unlock(spinlock_t *l)
{
#ifdef CONFIG_TICKET_LOCK
    __sync_fetch_and_add(&l->owner, 1);
#else
    __sync_lock_release(&l->locked);
#endif
}

