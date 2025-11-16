/*
 * Mach C Threads internal definitions
 * Internal structures for cthread implementation
 */

#ifndef _MACH_CTHREAD_INTERNALS_H_
#define _MACH_CTHREAD_INTERNALS_H_

#include <mach/mach.h>
#include <cthreads.h>

/* Internal cthread structure - for library use only */
struct cthread {
	struct cthread *next;		/* link in thread list */
	struct cthread *prev;
	mach_port_t thread;		/* Mach thread port */
	int state;			/* thread state */
	void *(*func)(void *);		/* thread function */
	void *arg;			/* thread argument */
	void *result;			/* thread result */
	char *name;			/* thread name */
	void *private_data;		/* thread-specific data */
};

/* Thread states */
#define CTHREAD_STATE_RUNNING	1
#define CTHREAD_STATE_BLOCKED	2
#define CTHREAD_STATE_TERMINATED 3

#endif /* _MACH_CTHREAD_INTERNALS_H_ */
