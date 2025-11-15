/*	mailbox.h - IPC mailbox structure	*/

#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

/*
 * Mailbox structure for inter-process communication
 */
struct mailbox {
	int	mb_flags;		/* mailbox flags */
	int	mb_count;		/* message count */
	caddr_t	mb_data;		/* message data */
	int	mb_size;		/* mailbox size */
};

/* Mailbox flags */
#define	MB_EMPTY	0x01		/* mailbox is empty */
#define	MB_FULL		0x02		/* mailbox is full */
#define	MB_WAITING	0x04		/* process waiting */

#endif /* _MAILBOX_H_ */
