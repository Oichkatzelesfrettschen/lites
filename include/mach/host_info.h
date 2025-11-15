/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_HOST_INFO_H_
#define	_MACH_HOST_INFO_H_

#include <mach/machine/vm_types.h>

typedef	natural_t	host_flavor_t;
typedef integer_t	*host_info_t;		/* varying array of int */

#define	HOST_INFO_MAX	(1024)		/* maximum array size */
typedef integer_t	host_info_data_t[HOST_INFO_MAX];

typedef natural_t	host_info_count_t;

#define HOST_BASIC_INFO		1	/* basic info */
#define HOST_PROCESSOR_SLOTS	2	/* processor slot numbers */
#define HOST_SCHED_INFO		3	/* scheduling info */

struct host_basic_info {
	integer_t	max_cpus;	/* max number of cpus possible */
	integer_t	avail_cpus;	/* number of cpus now available */
	natural_t	memory_size;	/* size of memory in bytes */
	integer_t	cpu_type;	/* cpu type */
	integer_t	cpu_subtype;	/* cpu subtype */
};

typedef	struct host_basic_info	host_basic_info_data_t;
typedef struct host_basic_info	*host_basic_info_t;
#define HOST_BASIC_INFO_COUNT \
	(sizeof(host_basic_info_data_t)/sizeof(integer_t))

#endif	/* _MACH_HOST_INFO_H_ */
