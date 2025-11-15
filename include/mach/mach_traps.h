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
 * File:	mach/mach_traps.h
 *
 * Definitions of Mach system call (trap) interfaces.
 */

#ifndef	_MACH_MACH_TRAPS_H_
#define	_MACH_MACH_TRAPS_H_

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mach system call prototypes
 * These are the "trap" interfaces - fast system calls into the kernel
 */

/* Task and thread operations */
mach_port_t	mach_task_self(void);
mach_port_t	mach_thread_self(void);
mach_port_t	mach_host_self(void);

/* Message operations */
mach_msg_return_t	mach_msg_trap(
				mach_msg_header_t *msg,
				mach_msg_option_t option,
				mach_msg_size_t send_size,
				mach_msg_size_t rcv_size,
				mach_port_t rcv_name,
				mach_msg_timeout_t timeout,
				mach_port_t notify);

/* Compatibility aliases */
#define mach_task_self_		mach_task_self
#define mach_thread_self_	mach_thread_self
#define mach_host_self_		mach_host_self

/* Thread operations */
kern_return_t	swtch_pri(int pri);
kern_return_t	swtch(void);
kern_return_t	thread_switch(mach_port_t thread_name, int option, int time);

/* Time operations */
kern_return_t	mach_timebase_info(void *info);

#ifdef __cplusplus
}
#endif

#endif	/* _MACH_MACH_TRAPS_H_ */
