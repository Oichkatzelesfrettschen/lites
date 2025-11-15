/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_THREAD_INFO_H_
#define	_MACH_THREAD_INFO_H_

#include <mach/machine/vm_types.h>

typedef	natural_t	thread_flavor_t;
typedef	integer_t	*thread_info_t;		/* varying array of int */

#define	THREAD_INFO_MAX		(1024)		/* maximum array size */
typedef	integer_t	thread_info_data_t[THREAD_INFO_MAX];

typedef natural_t	thread_info_count_t;

#define THREAD_BASIC_INFO	1		/* basic information */

struct thread_basic_info {
	natural_t	user_time;	/* user run time */
	natural_t	system_time;	/* system run time */
	integer_t	cpu_usage;	/* scaled cpu usage percentage */
	integer_t	base_priority;	/* base scheduling priority */
	integer_t	cur_priority;	/* current scheduling priority */
	integer_t	run_state;	/* run state (see below) */
	integer_t	flags;		/* various flags (see below) */
	integer_t	suspend_count;	/* suspend count for thread */
	integer_t	sleep_time;	/* number of seconds that thread
					   has been sleeping */
};

typedef struct thread_basic_info	thread_basic_info_data_t;
typedef struct thread_basic_info	*thread_basic_info_t;
#define THREAD_BASIC_INFO_COUNT	\
	(sizeof(thread_basic_info_data_t)/sizeof(natural_t))

/* Thread run states */

#define TH_STATE_RUNNING	1	/* thread is running normally */
#define TH_STATE_STOPPED	2	/* thread is stopped */
#define TH_STATE_WAITING	3	/* thread is waiting normally */
#define TH_STATE_UNINTERRUPTIBLE 4	/* thread is in an uninterruptible wait */
#define TH_STATE_HALTED		5	/* thread is halted at a clean point */

/* Thread flags */

#define TH_FLAGS_SWAPPED	0x1	/* thread is swapped out */
#define TH_FLAGS_IDLE		0x2	/* thread is an idle thread */

#endif	/* _MACH_THREAD_INFO_H_ */
