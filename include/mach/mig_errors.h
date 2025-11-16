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
 * File:	mach/mig_errors.h
 * Purpose:
 *	Definitions of error codes returned by MIG (Mach Interface Generator)
 */

#ifndef	_MACH_MIG_ERRORS_H_
#define _MACH_MIG_ERRORS_H_

#include <mach/kern_return.h>
#include <mach/message.h>

/*
 * MIG error codes
 */

#define MIG_TYPE_ERROR		-300	/* client/server type mismatch */
#define MIG_REPLY_MISMATCH	-301	/* wrong reply message ID */
#define MIG_REMOTE_ERROR	-302	/* server raised exception */
#define MIG_BAD_ID		-303	/* bad msg or RPC id */
#define MIG_BAD_ARGUMENTS	-304	/* bad argument to RPC */
#define MIG_NO_REPLY		-305	/* no reply from server */
#define MIG_EXCEPTION		-306	/* server raised exception */
#define MIG_ARRAY_TOO_LARGE	-307	/* array size exceeds limit */
#define MIG_SERVER_DIED		-308	/* server died */
#define MIG_DESTROY_REQUEST	-309	/* destroy request with no reply */

/*
 * Whenever MIG detects an error, it sends back a generic
 * mig_reply_error_t format message.  Clients must accept
 * these in addition to the expected reply message format.
 */

typedef struct {
	mach_msg_header_t	Head;
	mach_msg_type_t		RetCodeType;
	kern_return_t		RetCode;
} mig_reply_error_t;

/*
 * MIG type descriptor for inline data
 */
typedef struct {
	unsigned int		msgt_name : 8,
				msgt_size : 8,
				msgt_number : 12,
				msgt_inline : 1,
				msgt_longform : 1,
				msgt_deallocate : 1,
				msgt_unused : 1;
} mach_msg_type_t;

#endif	/* _MACH_MIG_ERRORS_H_ */
