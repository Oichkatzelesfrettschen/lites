#include "xk_lproxy.h"
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
char xk_lproxy_user_rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/proxy/RCS/xk_lproxy.defs,v 1.14 1993/09/16 22:03:39 menze Exp $";
#else	/* UseExternRCSId */
static char rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/proxy/RCS/xk_lproxy.defs,v 1.14 1993/09/16 22:03:39 menze Exp $";
#endif	/* UseExternRCSId */
#endif	/* lint */

#define msgh_request_port	msgh_remote_port
#define msgh_reply_port		msgh_local_port

#include <mach/std_types.h>
#include <mach/mach_types.h>
#include "xk_mig_sizes.h"
#include "xk_mig_t.h"
#include "assert.h"
#include "proxy_offset.h"

/* Routine dumpXObj */
mig_external kern_return_t call_lproxy_dumpXObj
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t obj,
	xk_xobj_dump_t *dump
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t objType;
		xobj_ext_id_t obj;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retValType;
		xkern_return_t retVal;
		mach_msg_type_t dumpType;
		xk_xobj_dump_t dump;
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

	static const mach_msg_type_t objType = {
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

	static const mach_msg_type_t retValCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t dumpCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		44,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->objType = objType;

	InP->obj = obj;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reqPort;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 844315;

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

	if (OutP->Head.msgh_id != 844415) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 220) &&
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
	if (* (int *) &OutP->retValType != * (int *) &retValCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*retVal = OutP->retVal;

#if	TypeCheck
	if (* (int *) &OutP->dumpType != * (int *) &dumpCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*dump = OutP->dump;

	return KERN_SUCCESS;
}

/* Routine xOpenDone */
mig_external kern_return_t call_lproxy_xOpenDone
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xobj_ext_id_t llp,
	xobj_ext_id_t hlpRcv,
	xobj_ext_id_t hlpType
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t llsType;
		xobj_ext_id_t lls;
		mach_msg_type_t llpType;
		xobj_ext_id_t llp;
		mach_msg_type_t hlpRcvType;
		xobj_ext_id_t hlpRcv;
		mach_msg_type_t hlpTypeType;
		xobj_ext_id_t hlpType;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retValType;
		xkern_return_t retVal;
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

	static const mach_msg_type_t llsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t llpType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t hlpRcvType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t hlpTypeType = {
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

	static const mach_msg_type_t retValCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->llsType = llsType;

	InP->lls = lls;

	InP->llpType = llpType;

	InP->llp = llp;

	InP->hlpRcvType = hlpRcvType;

	InP->hlpRcv = hlpRcv;

	InP->hlpTypeType = hlpTypeType;

	InP->hlpType = hlpType;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reqPort;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 844316;

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

	if (OutP->Head.msgh_id != 844416) {
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
	if (* (int *) &OutP->retValType != * (int *) &retValCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*retVal = OutP->retVal;

	return KERN_SUCCESS;
}

/* Routine ping */
mig_external kern_return_t call_lproxy_ping
(
	mach_port_t reqPort
)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;

	static const mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reqPort;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 844319;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, 24, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
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

	if (OutP->Head.msgh_id != 844419) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    (OutP->Head.msgh_size != 32))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	return OutP->RetCode;
}
/* Routine xDemux */
mig_external kern_return_t call_lproxy_xDemux
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xk_msg_data_t req,
	mach_msg_type_number_t reqCnt,
	int msgStart,
	xk_large_msg_data_t msgOol,
	mach_msg_type_number_t msgOolCnt,
	xk_msg_attr_t reqAttr,
	mach_msg_type_number_t reqAttrCnt
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t llsType;
		xobj_ext_id_t lls;
		mach_msg_type_t reqType;
		char req[4000];
		mach_msg_type_t msgStartType;
		int msgStart;
		mach_msg_type_long_t msgOolType;
		char msgOol[2048];
		mach_msg_type_t reqAttrType;
		char reqAttr[200];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retValType;
		xkern_return_t retVal;
	} Reply;

/* 
 * Begin x-kernel Sed hack
 */
	typedef union {
		Request In;
		Reply Out;
	} union_mess;

	union_mess	*Mess = (union_mess *)((xk_and_mach_msg_t *)req)->machMsg; 
/* 
 * End x-kernel Sed hack
 */

	register Request *InP = &Mess->In;
	register Reply *OutP = &Mess->Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t llsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t reqType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		4000,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t msgStartType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t msgOolType = {
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

	static const mach_msg_type_t reqAttrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		200,
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

	static const mach_msg_type_t retValCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->llsType = llsType;

	InP->lls = lls;

	InP->reqType = reqType;

	if (reqCnt > 4000) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
/* 
 * Begin x-kernel Sed hack
 */
		xAssert( offsetof(Request, req) == XK_DEMUX_REQ_OFFSET); 
		if ( ((xk_and_mach_msg_t *)req)->xkMsg ) { 
		    msg2buf(((xk_and_mach_msg_t *)req)->xkMsg, (char *) InP->req); 
		} 
/* 
 * End x-kernel Sed hack
 */
	}

	InP->reqType.msgt_number = reqCnt;

	msgh_size_delta = (reqCnt + 3) & ~3;
	msgh_size = 60 + msgh_size_delta;
	InP = (Request *) ((char *) InP + msgh_size_delta - 4000);
	InP->msgStartType = msgStartType;

	InP->msgStart = msgStart;

	InP->msgOolType = msgOolType;

	if (msgOolCnt > 2048) {
		InP->msgOolType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->msgOol) = msgOol;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->msgOol, msgOol, msgOolCnt);
	}

	InP->msgOolType.msgtl_number = msgOolCnt;

	msgh_size_delta = (InP->msgOolType.msgtl_header.msgt_inline) ? (msgOolCnt + 3) & ~3 : sizeof(char *);
	msgh_size += msgh_size_delta;
	InP = (Request *) ((char *) InP + msgh_size_delta - 2048);
	InP->reqAttrType = reqAttrType;

	if (reqAttrCnt > 200) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(InP->reqAttr, reqAttr, reqAttrCnt);
	}

	InP->reqAttrType.msgt_number = reqAttrCnt;

	msgh_size += (reqAttrCnt + 3) & ~3;
	InP = &Mess->In;
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reqPort;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 844317;

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

	if (OutP->Head.msgh_id != 844417) {
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
	if (* (int *) &OutP->retValType != * (int *) &retValCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*retVal = OutP->retVal;

	return KERN_SUCCESS;
}

/* Routine xCallDemux */
mig_external kern_return_t call_lproxy_xCallDemux
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xk_msg_data_t req,
	mach_msg_type_number_t reqCnt,
	int reqMsgStart,
	xk_large_msg_data_t reqool,
	mach_msg_type_number_t reqoolCnt,
	xk_msg_data_t rep,
	mach_msg_type_number_t *repCnt,
	xk_large_msg_data_t *repool,
	mach_msg_type_number_t *repoolCnt,
	xk_msg_attr_t reqAttr,
	mach_msg_type_number_t reqAttrCnt
)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t llsType;
		xobj_ext_id_t lls;
		mach_msg_type_t reqType;
		char req[4000];
		mach_msg_type_t reqMsgStartType;
		int reqMsgStart;
		mach_msg_type_long_t reqoolType;
		char reqool[2048];
		mach_msg_type_t reqAttrType;
		char reqAttr[200];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retValType;
		xkern_return_t retVal;
		mach_msg_type_t repType;
		char rep[4000];
		mach_msg_type_long_t repoolType;
		char repool[2048];
	} Reply;

/* 
 * Begin x-kernel Sed hack
 */
	typedef union {
		Request In;
		Reply Out;
	} union_mess;

	union_mess	*Mess = (union_mess *)((xk_and_mach_msg_t *)req)->machMsg; 
/* 
 * End x-kernel Sed hack
 */

	register Request *InP = &Mess->In;
	register Reply *OutP = &Mess->Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t llsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t reqType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		4000,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t reqMsgStartType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t reqoolType = {
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

	static const mach_msg_type_t reqAttrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		200,
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

	static const mach_msg_type_t retValCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	InP->llsType = llsType;

	InP->lls = lls;

	InP->reqType = reqType;

	if (reqCnt > 4000) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
/* 
 * Begin x-kernel Sed hack
 */
		xAssert( offsetof(Request, req) == XK_DEMUX_REQ_OFFSET); 
		if ( ((xk_and_mach_msg_t *)req)->xkMsg ) { 
		    msg2buf(((xk_and_mach_msg_t *)req)->xkMsg, (char *) InP->req); 
		} 
/* 
 * End x-kernel Sed hack
 */
	}

	InP->reqType.msgt_number = reqCnt;

	msgh_size_delta = (reqCnt + 3) & ~3;
	msgh_size = 60 + msgh_size_delta;
	InP = (Request *) ((char *) InP + msgh_size_delta - 4000);
	InP->reqMsgStartType = reqMsgStartType;

	InP->reqMsgStart = reqMsgStart;

	InP->reqoolType = reqoolType;

	if (reqoolCnt > 2048) {
		InP->reqoolType.msgtl_header.msgt_inline = FALSE;
		*((char **)InP->reqool) = reqool;
		msgh_simple = FALSE;
	}
	else {
		memcpy(InP->reqool, reqool, reqoolCnt);
	}

	InP->reqoolType.msgtl_number = reqoolCnt;

	msgh_size_delta = (InP->reqoolType.msgtl_header.msgt_inline) ? (reqoolCnt + 3) & ~3 : sizeof(char *);
	msgh_size += msgh_size_delta;
	InP = (Request *) ((char *) InP + msgh_size_delta - 2048);
	InP->reqAttrType = reqAttrType;

	if (reqAttrCnt > 200) {
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
		memcpy(InP->reqAttr, reqAttr, reqAttrCnt);
	}

	InP->reqAttrType.msgt_number = reqAttrCnt;

	msgh_size += (reqAttrCnt + 3) & ~3;
	InP = &Mess->In;
	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reqPort;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 844318;

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

	if (OutP->Head.msgh_id != 844418) {
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
	if (* (int *) &OutP->retValType != * (int *) &retValCheck)
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	*retVal = OutP->retVal;

#if	TypeCheck
	if ((OutP->repType.msgt_inline != TRUE) ||
	    (OutP->repType.msgt_longform != FALSE) ||
	    (OutP->repType.msgt_name != 8) ||
	    (OutP->repType.msgt_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	msgh_size_delta = (OutP->repType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size < 56 + msgh_size_delta)
		return MIG_TYPE_ERROR;
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	if (OutP->repType.msgt_number > *repCnt) {
		memcpy(rep, OutP->rep, *repCnt);
		return MIG_ARRAY_TOO_LARGE;
	}
	else {
/* 
 * x-kernel Sed hack deleted memcopy
 *		memcpy(rep, OutP->rep, OutP->repType.msgt_number);
 */
	}

	*repCnt = OutP->repType.msgt_number;

	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 4000);

#if	TypeCheck
	if ((OutP->repoolType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->repoolType.msgtl_name != 8) ||
	    (OutP->repoolType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 56 + ((OutP->repoolType.msgtl_header.msgt_inline) ? (OutP->repoolType.msgtl_number + 3) & ~3 : sizeof(char *)))
		return MIG_TYPE_ERROR;
#endif	/* TypeCheck */

	if (!OutP->repoolType.msgtl_header.msgt_inline)
	    *repool = *((char **)OutP->repool);
	else if (OutP->repoolType.msgtl_number > *repoolCnt) {
	    mig_allocate((vm_offset_t *)repool,
		OutP->repoolType.msgtl_number);
	    memcpy(*repool, OutP->repool, OutP->repoolType.msgtl_number);
	}
	else {
	    memcpy(*repool, OutP->repool, OutP->repoolType.msgtl_number);
	}

	*repoolCnt = OutP->repoolType.msgtl_number;

	return KERN_SUCCESS;
}

