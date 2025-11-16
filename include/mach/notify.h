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
 * File:	mach/notify.h
 *
 * Kernel notification message definitions.
 */

#ifndef	_MACH_NOTIFY_H_
#define _MACH_NOTIFY_H_

/*
 * Notification types
 */

#define MACH_NOTIFY_FIRST		0100
#define MACH_NOTIFY_PORT_DELETED	(MACH_NOTIFY_FIRST + 001)
#define MACH_NOTIFY_MSG_ACCEPTED	(MACH_NOTIFY_FIRST + 002)
#define MACH_NOTIFY_PORT_DESTROYED	(MACH_NOTIFY_FIRST + 005)
#define MACH_NOTIFY_NO_SENDERS		(MACH_NOTIFY_FIRST + 006)
#define MACH_NOTIFY_SEND_ONCE		(MACH_NOTIFY_FIRST + 007)
#define MACH_NOTIFY_DEAD_NAME		(MACH_NOTIFY_FIRST + 010)
#define MACH_NOTIFY_LAST		(MACH_NOTIFY_FIRST + 015)

#endif	/* _MACH_NOTIFY_H_ */
