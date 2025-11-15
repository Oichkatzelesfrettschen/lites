/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_TASK_INFO_H_
#define	_MACH_TASK_INFO_H_

#include <mach/machine/vm_types.h>

/*
 * Task information structures and definitions.
 */

typedef	natural_t	task_flavor_t;
typedef	integer_t	*task_info_t;		/* varying array of int */

#define	TASK_INFO_MAX		(1024)		/* maximum array size */
typedef	integer_t	task_info_data_t[TASK_INFO_MAX];

/*
 * Generic information structure to allow for expansion.
 */
typedef natural_t	task_info_count_t;

#define TASK_BASIC_INFO         1       /* basic information */

struct task_basic_info {
	integer_t	suspend_count;	/* suspend count for task */
	natural_t	virtual_size;	/* virtual memory size (bytes) */
	natural_t	resident_size;	/* resident memory size (bytes) */
	natural_t	user_time;	/* total user run time for
					   terminated threads */
	natural_t	system_time;	/* total system run time for
					   terminated threads */
};

typedef struct task_basic_info		task_basic_info_data_t;
typedef struct task_basic_info		*task_basic_info_t;
#define TASK_BASIC_INFO_COUNT	\
	(sizeof(task_basic_info_data_t)/sizeof(natural_t))

#endif	/* _MACH_TASK_INFO_H_ */
