/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * File:	mach/port.h
 *
 * Definition of a Mach port
 */

#ifndef	_MACH_PORT_H_
#define	_MACH_PORT_H_

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>

/*
 * Port names are just integers.
 */

typedef natural_t mach_port_t;
typedef mach_port_t *mach_port_array_t;

/*
 * Port right types
 */

typedef natural_t mach_port_right_t;

#define MACH_PORT_RIGHT_SEND		((mach_port_right_t) 0)
#define MACH_PORT_RIGHT_RECEIVE		((mach_port_right_t) 1)
#define MACH_PORT_RIGHT_SEND_ONCE	((mach_port_right_t) 2)
#define MACH_PORT_RIGHT_PORT_SET	((mach_port_right_t) 3)
#define MACH_PORT_RIGHT_DEAD_NAME	((mach_port_right_t) 4)
#define MACH_PORT_RIGHT_NUMBER		((mach_port_right_t) 5)

typedef natural_t mach_port_type_t;
typedef mach_port_type_t *mach_port_type_array_t;

#define MACH_PORT_TYPE(right) \
	((mach_port_type_t)(1 << ((right) + 16)))
#define MACH_PORT_TYPE_NONE	    ((mach_port_type_t) 0)
#define MACH_PORT_TYPE_SEND	    MACH_PORT_TYPE(MACH_PORT_RIGHT_SEND)
#define MACH_PORT_TYPE_RECEIVE	    MACH_PORT_TYPE(MACH_PORT_RIGHT_RECEIVE)
#define MACH_PORT_TYPE_SEND_ONCE    MACH_PORT_TYPE(MACH_PORT_RIGHT_SEND_ONCE)
#define MACH_PORT_TYPE_PORT_SET	    MACH_PORT_TYPE(MACH_PORT_RIGHT_PORT_SET)
#define MACH_PORT_TYPE_DEAD_NAME    MACH_PORT_TYPE(MACH_PORT_RIGHT_DEAD_NAME)

#define MACH_PORT_TYPE_DNREQUEST	0x80000000

/*
 * Dummy type bits that mach_port_type returns.
 */

#define MACH_PORT_TYPE_SEND_RECEIVE					\
	(MACH_PORT_TYPE_SEND|MACH_PORT_TYPE_RECEIVE)
#define MACH_PORT_TYPE_DEAD_NAME_SEND_ONCE				\
	(MACH_PORT_TYPE_DEAD_NAME|MACH_PORT_TYPE_SEND_ONCE)

#define MACH_PORT_TYPE_ALL_RIGHTS					\
	(MACH_PORT_TYPE_SEND|MACH_PORT_TYPE_RECEIVE|			\
	 MACH_PORT_TYPE_SEND_ONCE|MACH_PORT_TYPE_PORT_SET|		\
	 MACH_PORT_TYPE_DEAD_NAME)

/*
 * Reserved port names
 */

#define MACH_PORT_NULL		((mach_port_t) 0)
#define MACH_PORT_DEAD		((mach_port_t) ~0)

#define MACH_PORT_VALID(name)	\
	(((name) != MACH_PORT_NULL) && ((name) != MACH_PORT_DEAD))

/*
 * Port sequence numbers
 */

typedef natural_t mach_port_seqno_t;
typedef natural_t mach_port_mscount_t;
typedef natural_t mach_port_msgcount_t;
typedef natural_t mach_port_rights_t;

#define	MACH_PORT_QLIMIT_DEFAULT	((mach_port_msgcount_t) 5)
#define	MACH_PORT_QLIMIT_MAX		((mach_port_msgcount_t) 16)

/*
 * Old IPC compatibility definitions
 */

#if	MACH_IPC_COMPAT

typedef	natural_t		port_name_t;
typedef	port_name_t		*port_name_array_t;
typedef	port_name_t		port_t;
typedef port_t			*port_array_t;

typedef port_name_t		port_set_name_t;
typedef	port_t			port_set_t;
typedef	port_t			*port_set_array_t;

#define	PORT_NULL		((port_t) 0)

typedef	port_t			port_all_t;
typedef	port_t			port_rcv_t;
typedef	port_t			port_own_t;

#endif	/* MACH_IPC_COMPAT */

#endif	/* _MACH_PORT_H_ */
