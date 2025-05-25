#include "xsi_user.h"
#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/mig_errors.h>
#include <mach/mig_support.h>
#include <mach/msg_type.h>
/* LINTLIBRARY */

#ifndef	mig_internal
#define	mig_internal	static
#endif

#ifndef	mig_external
#define mig_external
#endif

#ifndef	TypeCheck
#define	TypeCheck 1
#endif

#ifndef	UseExternRCSId
#define	UseExternRCSId		1
#endif

#ifndef	lint
#if	UseExternRCSId
char xsi_user_rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/socket/RCS/xsi.defs,v 1.6 1993/02/02 00:08:39 menze Exp $";
#else	/* UseExternRCSId */
static char rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/socket/RCS/xsi.defs,v 1.6 1993/02/02 00:08:39 menze Exp $";
#endif	/* UseExternRCSId */
#endif	/* lint */

#define msgh_request_port	msgh_remote_port
#define msgh_reply_port		msgh_local_port

#include <mach/std_types.h>
#include "xsi_types.h"

/* Routine hello */
mig_external kern_return_t xsi_hello
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	mach_port_t signal_port,
	xsi_fdset_t is_xsi_fd,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t signal_portType;
		mach_port_t signal_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t is_xsi_fdType;
		xsi_fdset_t is_xsi_fd;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t signal_portType = {
		/* msgt_name = */		20,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t is_xsi_fdCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->signal_portType = signal_portType;

	InP->signal_port = signal_port;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102171;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 40, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102271) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 76) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->is_xsi_fdType != * (int *) &is_xsi_fdCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	{ typedef struct { char data[32]; } *sp; * (sp) is_xsi_fd = * (sp) OutP->is_xsi_fd; }

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine clone */
mig_external kern_return_t xsi_clone
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int parent_id,
	mach_port_t port,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t parent_idType;
		int parent_id;
		mach_msg_type_t portType;
		mach_port_t port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t parent_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t portType = {
		/* msgt_name = */		20,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->parent_idType = parent_idType;

	InP->parent_id = parent_id;

	InP->portType = portType;

	InP->port = port;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102172;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 48, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102272) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine await_clone_done */
mig_external kern_return_t xsi_await_clone_done
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102173;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 32, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102273) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine accept */
mig_external kern_return_t xsi_accept
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t *addrCnt,
	int ns,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t nsType;
		int ns;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		char addr[16];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->nsType = nsType;

	InP->ns = ns;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102174;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 48, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102274) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size < 44) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->addrType.msgt_inline != TRUE) ||
	    (OutP->addrType.msgt_longform != FALSE) ||
	    (OutP->addrType.msgt_name != 8) ||
	    (OutP->addrType.msgt_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->addrType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size != 44 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->addrType.msgt_number > *addrCnt) {
		memcpy(addr, OutP->addr, *addrCnt);
		*addrCnt = OutP->addrType.msgt_number;
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(addr, OutP->addr, OutP->addrType.msgt_number);
	}

	*addrCnt = OutP->addrType.msgt_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine bind */
mig_external kern_return_t xsi_bind
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t addrCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t addrType;
		char addr[16];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	unsigned int msgh_size;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t addrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->addrType = addrType;

	if (addrCnt > 16) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(InP->addr, addr, addrCnt);
	}

	InP->addrType.msgt_number = addrCnt;

	msgh_size = 44 + ((addrCnt + 3) & ~3);
	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102175;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102275) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine close */
mig_external kern_return_t xsi_close
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102176;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 40, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102276) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine connect */
mig_external kern_return_t xsi_connect
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t addrCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t addrType;
		char addr[16];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	unsigned int msgh_size;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t addrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->addrType = addrType;

	if (addrCnt > 16) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(InP->addr, addr, addrCnt);
	}

	InP->addrType.msgt_number = addrCnt;

	msgh_size = 44 + ((addrCnt + 3) & ~3);
	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102177;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102277) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine dup2 */
mig_external kern_return_t xsi_dup2
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int from,
	int to,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t fromType;
		int from;
		mach_msg_type_t toType;
		int to;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t fromType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t toType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->fromType = fromType;

	InP->from = from;

	InP->toType = toType;

	InP->to = to;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102178;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 48, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102278) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine listen */
mig_external kern_return_t xsi_listen
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int backlog,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t backlogType;
		int backlog;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t backlogType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->backlogType = backlogType;

	InP->backlog = backlog;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102179;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 48, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102279) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine socket */
mig_external kern_return_t xsi_socket
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int domain,
	int conn_type,
	int protocol,
	int s,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t domainType;
		int domain;
		mach_msg_type_t conn_typeType;
		int conn_type;
		mach_msg_type_t protocolType;
		int protocol;
		mach_msg_type_t sType;
		int s;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t domainType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t conn_typeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t protocolType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->domainType = domainType;

	InP->domain = domain;

	InP->conn_typeType = conn_typeType;

	InP->conn_type = conn_type;

	InP->protocolType = protocolType;

	InP->protocol = protocol;

	InP->sType = sType;

	InP->s = s;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102180;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 64, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102280) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine getpeername */
mig_external kern_return_t xsi_getpeername
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t *addrCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		char addr[16];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102181;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 40, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102281) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size < 44) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->addrType.msgt_inline != TRUE) ||
	    (OutP->addrType.msgt_longform != FALSE) ||
	    (OutP->addrType.msgt_name != 8) ||
	    (OutP->addrType.msgt_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->addrType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size != 44 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->addrType.msgt_number > *addrCnt) {
		memcpy(addr, OutP->addr, *addrCnt);
		*addrCnt = OutP->addrType.msgt_number;
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(addr, OutP->addr, OutP->addrType.msgt_number);
	}

	*addrCnt = OutP->addrType.msgt_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine getsockname */
mig_external kern_return_t xsi_getsockname
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t *addrCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addrType;
		char addr[16];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102182;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 40, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102282) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size < 44) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->addrType.msgt_inline != TRUE) ||
	    (OutP->addrType.msgt_longform != FALSE) ||
	    (OutP->addrType.msgt_name != 8) ||
	    (OutP->addrType.msgt_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->addrType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size != 44 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->addrType.msgt_number > *addrCnt) {
		memcpy(addr, OutP->addr, *addrCnt);
		*addrCnt = OutP->addrType.msgt_number;
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(addr, OutP->addr, OutP->addrType.msgt_number);
	}

	*addrCnt = OutP->addrType.msgt_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine getsockopt */
mig_external kern_return_t xsi_getsockopt
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int level,
	int optname,
	xsi_varbuf_t *optval,
	mach_msg_type_number_t *optvalCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t levelType;
		int level;
		mach_msg_type_t optnameType;
		int optname;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t optvalType;
		char optval[2048];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t levelType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t optnameType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->levelType = levelType;

	InP->level = level;

	InP->optnameType = optnameType;

	InP->optname = optname;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102183;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 56, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102283) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size < 52)) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->optvalType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->optvalType.msgtl_name != 8) ||
	    (OutP->optvalType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->optvalType.msgtl_header.msgt_inline) ? (OutP->optvalType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size != 52 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (!OutP->optvalType.msgtl_header.msgt_inline)
	    *optval = *((char **)OutP->optval);
	else if (OutP->optvalType.msgtl_number > *optvalCnt) {
	    mig_allocate((vm_offset_t *)optval,
		OutP->optvalType.msgtl_number);
	    memcpy(*optval, OutP->optval, OutP->optvalType.msgtl_number);
	}
	else {
	    memcpy(*optval, OutP->optval, OutP->optvalType.msgtl_number);
	}

	*optvalCnt = OutP->optvalType.msgtl_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine setsockopt */
mig_external kern_return_t xsi_setsockopt
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int level,
	int optname,
	xsi_varbuf_t optval,
	mach_msg_type_number_t optvalCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t levelType;
		int level;
		mach_msg_type_t optnameType;
		int optname;
		mach_msg_type_long_t optvalType;
		char optval[2048];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t levelType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t optnameType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t optvalType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->levelType = levelType;

	InP->level = level;

	InP->optnameType = optnameType;

	InP->optname = optname;

	InP->optvalType = optvalType;

	if (optvalCnt > 2048) {
		InP->optvalType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->optval) = optval;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->optval, optval, optvalCnt);
	}

	InP->optvalType.msgtl_number = optvalCnt;

	msgh_size = 68 + ((InP->optvalType.msgtl_header.msgt_inline) ? (optvalCnt + 3) & ~3 : sizeof(char *));
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102184;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102284) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine select */
mig_external kern_return_t xsi_select
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int seqno,
	int *nfds,
	xsi_select_mask_t specified,
	xsi_fdset_t readfds,
	xsi_fdset_t writefds,
	xsi_fdset_t exceptfds,
	xsi_timeval_t timeout,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t seqnoType;
		int seqno;
		mach_msg_type_t nfdsType;
		int nfds;
		mach_msg_type_t specifiedType;
		xsi_select_mask_t specified;
		mach_msg_type_t readfdsType;
		xsi_fdset_t readfds;
		mach_msg_type_t writefdsType;
		xsi_fdset_t writefds;
		mach_msg_type_t exceptfdsType;
		xsi_fdset_t exceptfds;
		mach_msg_type_t timeoutType;
		xsi_timeval_t timeout;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t nfdsType;
		int nfds;
		mach_msg_type_t readfdsType;
		xsi_fdset_t readfds;
		mach_msg_type_t writefdsType;
		xsi_fdset_t writefds;
		mach_msg_type_t exceptfdsType;
		xsi_fdset_t exceptfds;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t seqnoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nfdsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t specifiedType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t readfdsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t writefdsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t exceptfdsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t timeoutType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		2,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nfdsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t readfdsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t writefdsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t exceptfdsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->seqnoType = seqnoType;

	InP->seqno = seqno;

	InP->nfdsType = nfdsType;

	InP->nfds = *nfds;

	InP->specifiedType = specifiedType;

	InP->specified = specified;

	InP->readfdsType = readfdsType;

	{ typedef struct { char data[32]; } *sp; * (sp) InP->readfds = * (sp) readfds; }

	InP->writefdsType = writefdsType;

	{ typedef struct { char data[32]; } *sp; * (sp) InP->writefds = * (sp) writefds; }

	InP->exceptfdsType = exceptfdsType;

	{ typedef struct { char data[32]; } *sp; * (sp) InP->exceptfds = * (sp) exceptfds; }

	InP->timeoutType = timeoutType;

	{ typedef struct { char data[8]; } *sp; * (sp) InP->timeout = * (sp) timeout; }

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102185;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 176, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102285) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 156) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->nfdsType != * (int *) &nfdsCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*nfds = OutP->nfds;

#if	TypeCheck
	if (* (int *) &OutP->readfdsType != * (int *) &readfdsCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	{ typedef struct { char data[32]; } *sp; * (sp) readfds = * (sp) OutP->readfds; }

#if	TypeCheck
	if (* (int *) &OutP->writefdsType != * (int *) &writefdsCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	{ typedef struct { char data[32]; } *sp; * (sp) writefds = * (sp) OutP->writefds; }

#if	TypeCheck
	if (* (int *) &OutP->exceptfdsType != * (int *) &exceptfdsCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	{ typedef struct { char data[32]; } *sp; * (sp) exceptfds = * (sp) OutP->exceptfds; }

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine select_cancel */
mig_external kern_return_t xsi_select_cancel
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int seqno,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t seqnoType;
		int seqno;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t seqnoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->seqnoType = seqnoType;

	InP->seqno = seqno;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102186;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 40, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102286) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine ioctl */
mig_external kern_return_t xsi_ioctl
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int request,
	xsi_varbuf_t inp,
	mach_msg_type_number_t inpCnt,
	xsi_varbuf_t *outp,
	mach_msg_type_number_t *outpCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t requestType;
		int request;
		mach_msg_type_long_t inpType;
		char inp[2048];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t outpType;
		char outp[2048];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t requestType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t inpType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->requestType = requestType;

	InP->request = request;

	InP->inpType = inpType;

	if (inpCnt > 2048) {
		InP->inpType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->inp) = inp;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->inp, inp, inpCnt);
	}

	InP->inpType.msgtl_number = inpCnt;

	msgh_size = 60 + ((InP->inpType.msgtl_header.msgt_inline) ? (inpCnt + 3) & ~3 : sizeof(char *));
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102187;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102287) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size < 52)) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->outpType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->outpType.msgtl_name != 8) ||
	    (OutP->outpType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->outpType.msgtl_header.msgt_inline) ? (OutP->outpType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size != 52 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (!OutP->outpType.msgtl_header.msgt_inline)
	    *outp = *((char **)OutP->outp);
	else if (OutP->outpType.msgtl_number > *outpCnt) {
	    mig_allocate((vm_offset_t *)outp,
		OutP->outpType.msgtl_number);
	    memcpy(*outp, OutP->outp, OutP->outpType.msgtl_number);
	}
	else {
	    memcpy(*outp, OutP->outp, OutP->outpType.msgtl_number);
	}

	*outpCnt = OutP->outpType.msgtl_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine fcntl */
mig_external kern_return_t xsi_fcntl
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int cmd,
	int *arg,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t cmdType;
		int cmd;
		mach_msg_type_t argType;
		int arg;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t argType;
		int arg;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t cmdType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t argType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t argCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->cmdType = cmdType;

	InP->cmd = cmd;

	InP->argType = argType;

	InP->arg = *arg;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102188;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 56, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102288) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 48) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->argType != * (int *) &argCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*arg = OutP->arg;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine read */
mig_external kern_return_t xsi_read
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t *buf,
	mach_msg_type_number_t *bufCnt,
	int nbytes,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t nbytesType;
		int nbytes;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t bufType;
		char buf[2048];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nbytesType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->nbytesType = nbytesType;

	InP->nbytes = nbytes;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102189;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 48, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102289) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size < 52)) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->bufType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->bufType.msgtl_name != 8) ||
	    (OutP->bufType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->bufType.msgtl_header.msgt_inline) ? (OutP->bufType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size != 52 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (!OutP->bufType.msgtl_header.msgt_inline)
	    *buf = *((char **)OutP->buf);
	else if (OutP->bufType.msgtl_number > *bufCnt) {
	    mig_allocate((vm_offset_t *)buf,
		OutP->bufType.msgtl_number);
	    memcpy(*buf, OutP->buf, OutP->bufType.msgtl_number);
	}
	else {
	    memcpy(*buf, OutP->buf, OutP->bufType.msgtl_number);
	}

	*bufCnt = OutP->bufType.msgtl_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine write */
mig_external kern_return_t xsi_write
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t buf,
	mach_msg_type_number_t bufCnt,
	int *nwritten,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_long_t bufType;
		char buf[2048];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t nwrittenType;
		int nwritten;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t bufType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nwrittenCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->bufType = bufType;

	if (bufCnt > 2048) {
		InP->bufType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->buf) = buf;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->buf, buf, bufCnt);
	}

	InP->bufType.msgtl_number = bufCnt;

	msgh_size = 52 + ((InP->bufType.msgtl_header.msgt_inline) ? (bufCnt + 3) & ~3 : sizeof(char *));
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102190;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102290) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 48) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->nwrittenType != * (int *) &nwrittenCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*nwritten = OutP->nwritten;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine recv */
mig_external kern_return_t xsi_recv
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t *buf,
	mach_msg_type_number_t *bufCnt,
	int maxlen,
	int flags,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t maxlenType;
		int maxlen;
		mach_msg_type_t flagsType;
		int flags;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t bufType;
		char buf[2048];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t maxlenType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t flagsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->maxlenType = maxlenType;

	InP->maxlen = maxlen;

	InP->flagsType = flagsType;

	InP->flags = flags;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102191;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 56, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102291) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size < 52)) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->bufType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->bufType.msgtl_name != 8) ||
	    (OutP->bufType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->bufType.msgtl_header.msgt_inline) ? (OutP->bufType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size != 52 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (!OutP->bufType.msgtl_header.msgt_inline)
	    *buf = *((char **)OutP->buf);
	else if (OutP->bufType.msgtl_number > *bufCnt) {
	    mig_allocate((vm_offset_t *)buf,
		OutP->bufType.msgtl_number);
	    memcpy(*buf, OutP->buf, OutP->bufType.msgtl_number);
	}
	else {
	    memcpy(*buf, OutP->buf, OutP->bufType.msgtl_number);
	}

	*bufCnt = OutP->bufType.msgtl_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine recvfrom */
mig_external kern_return_t xsi_recvfrom
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t *buf,
	mach_msg_type_number_t *bufCnt,
	int maxlen,
	int flags,
	xsi_sockaddr_t from_addr,
	mach_msg_type_number_t *from_addrCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_t maxlenType;
		int maxlen;
		mach_msg_type_t flagsType;
		int flags;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t bufType;
		char buf[2048];
		mach_msg_type_t from_addrType;
		char from_addr[16];
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */
#if	TypeCheck
	unsigned int msgh_size;
#endif	/* TypeCheck */
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t maxlenType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t flagsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->maxlenType = maxlenType;

	InP->maxlen = maxlen;

	InP->flagsType = flagsType;

	InP->flags = flags;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102192;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 56, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102292) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size < 56)) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->bufType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->bufType.msgtl_name != 8) ||
	    (OutP->bufType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->bufType.msgtl_header.msgt_inline) ? (OutP->bufType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 56 + msgh_size_delta)
		return MIG_TYPE_ERROR;
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	if (!OutP->bufType.msgtl_header.msgt_inline)
	    *buf = *((char **)OutP->buf);
	else if (OutP->bufType.msgtl_number > *bufCnt) {
	    mig_allocate((vm_offset_t *)buf,
		OutP->bufType.msgtl_number);
	    memcpy(*buf, OutP->buf, OutP->bufType.msgtl_number);
	}
	else {
	    memcpy(*buf, OutP->buf, OutP->bufType.msgtl_number);
	}

	*bufCnt = OutP->bufType.msgtl_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

#if	TypeCheck
	if ((OutP->from_addrType.msgt_inline != TRUE) ||
	    (OutP->from_addrType.msgt_longform != FALSE) ||
	    (OutP->from_addrType.msgt_name != 8) ||
	    (OutP->from_addrType.msgt_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->from_addrType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size != 56 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->from_addrType.msgt_number > *from_addrCnt) {
		memcpy(from_addr, OutP->from_addr, *from_addrCnt);
		*from_addrCnt = OutP->from_addrType.msgt_number;
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(from_addr, OutP->from_addr, OutP->from_addrType.msgt_number);
	}

	*from_addrCnt = OutP->from_addrType.msgt_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine send */
mig_external kern_return_t xsi_send
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t msg,
	mach_msg_type_number_t msgCnt,
	int *nsent,
	int flags,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_long_t msgType;
		char msg[2048];
		mach_msg_type_t flagsType;
		int flags;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t nsentType;
		int nsent;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t msgType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	static const mach_msg_type_t flagsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nsentCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->msgType = msgType;

	if (msgCnt > 2048) {
		InP->msgType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->msg) = msg;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->msg, msg, msgCnt);
	}

	InP->msgType.msgtl_number = msgCnt;

	msgh_size_delta = (InP->msgType.msgtl_header.msgt_inline) ? (msgCnt + 3) & ~3 : sizeof(char *);
	msgh_size = 60 + msgh_size_delta;
	InP = (Request *) ((char *) InP + msgh_size_delta - 2048);
	InP->flagsType = flagsType;

	InP->flags = flags;

	InP = &Mess.In;
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102193;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102293) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 48) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->nsentType != * (int *) &nsentCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*nsent = OutP->nsent;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}

/* Routine sendto */
mig_external kern_return_t xsi_sendto
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t msg,
	mach_msg_type_number_t msgCnt,
	int *nsent,
	int flags,
	xsi_sockaddr_t toaddr,
	mach_msg_type_number_t toaddrCnt,
	int *errnum
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t client_idType;
		xsi_clientid_t client_id;
		mach_msg_type_t sType;
		int s;
		mach_msg_type_long_t msgType;
		char msg[2048];
		mach_msg_type_t flagsType;
		int flags;
		mach_msg_type_t toaddrType;
		char toaddr[16];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t nsentType;
		int nsent;
		mach_msg_type_t errnumType;
		int errnum;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t msgType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	2048,
	};

	static const mach_msg_type_t flagsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t toaddrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nsentCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->client_idType = client_idType;

	InP->client_id = client_id;

	InP->sType = sType;

	InP->s = s;

	InP->msgType = msgType;

	if (msgCnt > 2048) {
		InP->msgType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->msg) = msg;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->msg, msg, msgCnt);
	}

	InP->msgType.msgtl_number = msgCnt;

	msgh_size_delta = (InP->msgType.msgtl_header.msgt_inline) ? (msgCnt + 3) & ~3 : sizeof(char *);
	msgh_size = 64 + msgh_size_delta;
	InP = (Request *) ((char *) InP + msgh_size_delta - 2048);
	InP->flagsType = flagsType;

	InP->flags = flags;

	InP->toaddrType = toaddrType;

	if (toaddrCnt > 16) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(InP->toaddr, toaddr, toaddrCnt);
	}

	InP->toaddrType.msgt_number = toaddrCnt;

	msgh_size += (toaddrCnt + 3) & ~3;
	InP = &Mess.In;
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 102194;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_SEND_INVALID_MEMORY) ||
		    (msg_result == MACH_SEND_INVALID_RIGHT) ||
		    (msg_result == MACH_SEND_INVALID_TYPE) ||
		    (msg_result == MACH_SEND_MSG_TOO_SMALL) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port(InP->Head.msgh_reply_port);
		else
			mig_put_reply_port(InP->Head.msgh_reply_port);
		return msg_result;
	}
	mig_put_reply_port(InP->Head.msgh_reply_port);

	if (OutP->Head.msgh_id != 102294) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 48) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if (* (int *) &OutP->nsentType != * (int *) &nsentCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*nsent = OutP->nsent;

#if	TypeCheck
	if (* (int *) &OutP->errnumType != * (int *) &errnumCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*errnum = OutP->errnum;

	return KERN_SUCCESS;
}
