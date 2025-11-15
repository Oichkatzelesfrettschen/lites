/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_MEMORY_OBJECT_H_
#define	_MACH_MEMORY_OBJECT_H_

/*
 *	External memory management interface definition.
 */

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/port.h>
#include <mach/vm_prot.h>

/*
 *	Temporary definition for compatibility
 */
typedef vm_offset_t	memory_object_offset_t;
typedef vm_size_t	memory_object_size_t;
typedef vm_offset_t	memory_object_cluster_size_t;

/*
 *	Copy strategy for memory_object_data_write.
 */
typedef int		memory_object_copy_strategy_t;

/* Do not change the numbered values */

#define		MEMORY_OBJECT_COPY_NONE		0
#define		MEMORY_OBJECT_COPY_CALL		1
#define		MEMORY_OBJECT_COPY_DELAY	2
#define		MEMORY_OBJECT_COPY_TEMPORARY	3
#define		MEMORY_OBJECT_COPY_SYMMETRIC	4
#define		MEMORY_OBJECT_COPY_INVALID	5

typedef int		memory_object_return_t;

/* Do not change the numbered values */

#define		MEMORY_OBJECT_RETURN_NONE	0
#define		MEMORY_OBJECT_RETURN_DIRTY	1
#define		MEMORY_OBJECT_RETURN_ALL	2

#endif	/* _MACH_MEMORY_OBJECT_H_ */
