/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_PROCESSOR_INFO_H_
#define	_MACH_PROCESSOR_INFO_H_

#include <mach/machine/vm_types.h>

typedef	natural_t	processor_flavor_t;
typedef integer_t	*processor_info_t;		/* varying array of int */
typedef integer_t	*processor_info_array_t;	/* varying array of int */

#define	PROCESSOR_INFO_MAX	(1024)		/* maximum array size */
typedef integer_t	processor_info_data_t[PROCESSOR_INFO_MAX];

typedef natural_t	processor_info_count_t;

#define PROCESSOR_BASIC_INFO	1		/* basic information */

struct processor_basic_info {
	integer_t	cpu_type;	/* type of cpu */
	integer_t	cpu_subtype;	/* subtype of cpu */
	integer_t	running;	/* is processor running */
	integer_t	slot_num;	/* slot number */
	boolean_t	is_master;	/* is this the master processor */
};

typedef	struct processor_basic_info	processor_basic_info_data_t;
typedef struct processor_basic_info	*processor_basic_info_t;
#define PROCESSOR_BASIC_INFO_COUNT \
	(sizeof(processor_basic_info_data_t)/sizeof(natural_t))

#endif	/* _MACH_PROCESSOR_INFO_H_ */
