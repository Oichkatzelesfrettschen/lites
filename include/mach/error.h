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
 * File:	mach/error.h
 * Purpose:
 *	Error code composition and decomposition
 */

#ifndef	_MACH_ERROR_H_
#define _MACH_ERROR_H_

#include <mach/kern_return.h>

/*
 * Error encoding:
 *	31                           16 15      14    0
 *	+--------------------------+------+------+------+
 *	|     system & subsystem   | code |  sub |      |
 *	+--------------------------+------+------+------+
 */

#define	err_none		(mach_error_t)0
#define ERR_SUCCESS		(mach_error_t)0
#define	ERR_ROUTINE_NIL		(mach_error_fn_t)0

#define	err_system(x)		(((x)&0x3f)<<26)
#define err_sub(x)		(((x)&0xfff)<<14)

#define err_get_system(err)	(((err)>>26)&0x3f)
#define err_get_sub(err)	(((err)>>14)&0xfff)
#define err_get_code(err)	((err)&0x3fff)

#define	err_local	        err_system(0x3f)

/* System error codes */
#define	err_kern		err_system(0x0)		/* kernel */
#define	err_us			err_system(0x1)		/* user space library */
#define	err_server		err_system(0x2)		/* user space servers */
#define	err_ipc			err_system(0x3)		/* old ipc errors */
#define err_mach_ipc		err_system(0x4)		/* mach-ipc errors */
#define err_dipc		err_system(0x7)		/* distributed ipc */
#define err_local_kernel	err_system(0x3e)	/* local kernel */
#define err_max_system		0x3f

typedef	kern_return_t		mach_error_t;
typedef mach_error_t		(* mach_error_fn_t)();

/*
 * Error string functions
 */
char		*mach_error_string(mach_error_t);
char		*mach_error_type(mach_error_t);

#endif	/* _MACH_ERROR_H_ */
