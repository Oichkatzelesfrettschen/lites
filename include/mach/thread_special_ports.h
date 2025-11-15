/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_THREAD_SPECIAL_PORTS_H_
#define	_MACH_THREAD_SPECIAL_PORTS_H_

/*
 *	Defines codes for access to "special" ports in thread_get_special_port
 *	and thread_set_special_port.
 */

#define THREAD_KERNEL_PORT	1	/* Represents the thread to the outside
					   world.*/
#define THREAD_REPLY_PORT	2	/* Default reply port for the thread's
					   user-level messages. */

#endif	/* _MACH_THREAD_SPECIAL_PORTS_H_ */
