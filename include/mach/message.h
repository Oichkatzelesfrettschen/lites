/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_MESSAGE_H_
#define	_MACH_MESSAGE_H_

/*
 * Mach IPC message and primitive types.
 */

#include <mach/port.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>

/*
 * Message data types
 */

typedef natural_t mach_msg_timeout_t;
typedef natural_t mach_msg_size_t;
typedef integer_t mach_msg_return_t;

typedef	unsigned int mach_msg_bits_t;
typedef	unsigned int mach_msg_option_t;
typedef natural_t mach_msg_id_t;

/*
 * Message header
 */
typedef struct {
	mach_msg_bits_t	msgh_bits;
	mach_msg_size_t	msgh_size;
	mach_port_t	msgh_remote_port;
	mach_port_t	msgh_local_port;
	mach_msg_size_t msgh_reserved;
	mach_msg_id_t	msgh_id;
} mach_msg_header_t;

/*
 * Message options
 */
#define MACH_MSG_OPTION_NONE		0x00000000

#define MACH_SEND_MSG			0x00000001
#define MACH_RCV_MSG			0x00000002

#define MACH_SEND_TIMEOUT		0x00000010
#define MACH_SEND_INTERRUPT		0x00000040
#define MACH_SEND_CANCEL		0x00000080
#define MACH_SEND_ALWAYS		0x00010000

#define MACH_RCV_TIMEOUT		0x00000100
#define MACH_RCV_NOTIFY			0x00000200
#define MACH_RCV_INTERRUPT		0x00000400
#define MACH_RCV_LARGE			0x00000800

/*
 * Message return codes
 */
#define MACH_MSG_SUCCESS		0x00000000

#define MACH_MSG_MASK			0x00003e00
#define MACH_MSG_IPC_SPACE		0x00002000
#define MACH_MSG_VM_SPACE		0x00001000
#define MACH_MSG_IPC_KERNEL		0x00000800
#define MACH_MSG_VM_KERNEL		0x00000400

#define MACH_SEND_IN_PROGRESS		0x10000001
#define MACH_SEND_INVALID_DATA		0x10000002
#define MACH_SEND_INVALID_DEST		0x10000003
#define MACH_SEND_TIMED_OUT		0x10000004
#define MACH_SEND_INTERRUPTED		0x10000007
#define MACH_SEND_MSG_TOO_SMALL		0x10000008
#define MACH_SEND_INVALID_REPLY		0x10000009
#define MACH_SEND_INVALID_RIGHT		0x1000000a
#define MACH_SEND_INVALID_NOTIFY	0x1000000b
#define MACH_SEND_INVALID_MEMORY	0x1000000c
#define MACH_SEND_NO_BUFFER		0x1000000d
#define MACH_SEND_TOO_LARGE		0x1000000e
#define MACH_SEND_INVALID_TYPE		0x1000000f
#define MACH_SEND_INVALID_HEADER	0x10000010

#define MACH_RCV_IN_PROGRESS		0x10004001
#define MACH_RCV_INVALID_NAME		0x10004002
#define MACH_RCV_TIMED_OUT		0x10004003
#define MACH_RCV_TOO_LARGE		0x10004004
#define MACH_RCV_INTERRUPTED		0x10004005
#define MACH_RCV_PORT_CHANGED		0x10004006
#define MACH_RCV_INVALID_NOTIFY		0x10004007
#define MACH_RCV_INVALID_DATA		0x10004008
#define MACH_RCV_PORT_DIED		0x10004009
#define MACH_RCV_IN_SET			0x1000400a
#define MACH_RCV_HEADER_ERROR		0x1000400b
#define MACH_RCV_BODY_ERROR		0x1000400c

/*
 * Compatibility definitions
 */
#if	MACH_IPC_COMPAT

typedef struct {
	msg_size_t	msg_size;
	msg_id_t	msg_type;
	port_t		msg_local_port;
	port_t		msg_remote_port;
	long		msg_id;
} msg_header_t;

typedef long		msg_timeout_t;
typedef long		msg_size_t;
typedef long		msg_id_t;
typedef long		msg_return_t;

#endif	/* MACH_IPC_COMPAT */

#endif	/* _MACH_MESSAGE_H_ */
