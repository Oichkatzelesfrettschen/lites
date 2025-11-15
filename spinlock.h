/*
 * spinlock.h - Spinlock primitives for multiprocessor synchronization
 */

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

/* Simple spinlock structure */
typedef struct {
	volatile int lock;
} spinlock_t;

/* Spinlock operations - stubs for uniprocessor */
#define SPINLOCK_INIT	{ 0 }

#define spin_lock_init(lock)	do { (lock)->lock = 0; } while (0)
#define spin_lock(lock)		do { } while (0)
#define spin_unlock(lock)	do { } while (0)
#define spin_trylock(lock)	(1)

#endif /* _SPINLOCK_H_ */
