/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * File:	mach/mach_types.h
 * Author:	Avadis Tevanian, Jr., Michael Wayne Young
 * Date:	1986
 *
 * Mach external interface definitions.
 */

#ifndef	_MACH_MACH_TYPES_H_
#define	_MACH_MACH_TYPES_H_

#include <mach/machine/vm_types.h>
#include <mach/boolean.h>
#include <mach/port.h>

/*
 * Common type definitions that lots of old files depend on.
 */

typedef	unsigned int	natural_t;
typedef int		integer_t;

/*
 * Port and Kernel Object definitions
 */

typedef mach_port_t	task_t;
typedef mach_port_t	thread_t;
typedef mach_port_t	vm_task_t;
typedef mach_port_t	ipc_space_t;
typedef mach_port_t	host_t;
typedef mach_port_t	host_priv_t;
typedef mach_port_t	processor_t;
typedef mach_port_t	processor_set_t;
typedef mach_port_t	processor_set_name_t;
typedef mach_port_t	thread_act_t;

/*
 * Memory object types
 */

typedef mach_port_t	memory_object_t;
typedef mach_port_t	memory_object_control_t;
typedef mach_port_t	memory_object_name_t;
typedef mach_port_t	memory_object_default_t;

/*
 * Array types
 */

typedef task_t			*task_array_t;
typedef thread_t		*thread_array_t;
typedef thread_act_t		*thread_act_array_t;
typedef processor_set_t		*processor_set_array_t;
typedef processor_set_t		*processor_set_name_array_t;
typedef processor_t		*processor_array_t;

typedef integer_t		*integer_array_t;

/*
 * Device types
 */

typedef mach_port_t	device_t;

/*
 * Old IPC compatibility definitions
 */

#if	MACH_IPC_COMPAT

/*
 *	Mach used to use int's for send type operations.  These names
 *	are defined for compatibility with old user code.
 */

#define	MSG_TYPE_NORMAL		MACH_MSG_TYPE_MOVE_RECEIVE
#define	MSG_TYPE_EMERGENCY	MACH_MSG_TYPE_MOVE_SEND

typedef	int		msg_timeout_t;

#endif	/* MACH_IPC_COMPAT */

#endif	/* _MACH_MACH_TYPES_H_ */
