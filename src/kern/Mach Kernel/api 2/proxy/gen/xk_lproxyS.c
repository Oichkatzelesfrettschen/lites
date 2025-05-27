/* Module xk_lproxy */

#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/mig_support.h>

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
char xk_lproxy_server_rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/proxy/RCS/xk_lproxy.defs,v 1.14 1993/09/16 22:03:39 menze Exp $";
#else	/* UseExternRCSId */
static char rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/proxy/RCS/xk_lproxy.defs,v 1.14 1993/09/16 22:03:39 menze Exp $";
#endif	/* UseExternRCSId */
#endif	/* lint */

#define msgh_request_port	msgh_local_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_LOCAL(bits)
#define msgh_reply_port		msgh_remote_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_REMOTE(bits)

#include <mach/std_types.h>
#include <mach/mach_types.h>
#include "xk_mig_sizes.h"
#include "xk_mig_t.h"
#include "assert.h"
#include "lproxy.h"

/* Routine dumpXObj */
mig_internal void _XdumpXObj
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_lproxy_dumpXObj
		(mach_port_t reqPort, xkern_return_t *retVal, xobj_ext_id_t obj, xk_xobj_dump_t *dump);

	static const mach_msg_type_t objCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t retValType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t dumpType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		44,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->objType != * (int *) &objCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_lproxy_dumpXObj(In0P->Head.msgh_request_port, &OutP->retVal, In0P->obj, &OutP->dump);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 220;

	OutP->retValType = retValType;

	OutP->dumpType = dumpType;
}

/* Routine xOpenDone */
mig_internal void _XxOpenDone
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_lproxy_xOpenDone
		(mach_port_t reqPort, xkern_return_t *retVal, xobj_ext_id_t lls, xobj_ext_id_t llp, xobj_ext_id_t hlpRcv, xobj_ext_id_t hlpType);

	static const mach_msg_type_t llsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t llpCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t hlpRcvCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t hlpTypeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t retValType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 56) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llsType != * (int *) &llsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llpType != * (int *) &llpCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->hlpRcvType != * (int *) &hlpRcvCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->hlpTypeType != * (int *) &hlpTypeCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_lproxy_xOpenDone(In0P->Head.msgh_request_port, &OutP->retVal, In0P->lls, In0P->llp, In0P->hlpRcv, In0P->hlpType);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retValType = retValType;
}

/* Routine xDemux */
mig_internal void _XxDemux
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Request *In2P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_lproxy_xDemux
		(mach_port_t reqPort, xkern_return_t *retVal, xobj_ext_id_t lls, xk_msg_data_t req, mach_msg_type_number_t reqCnt, int msgStart, xk_large_msg_data_t msgOol, mach_msg_type_number_t msgOolCnt, boolean_t msgOolSCopy, xk_msg_attr_t reqAttr, mach_msg_type_number_t reqAttrCnt);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t llsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t msgStartCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t retValType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 60))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llsType != * (int *) &llsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->reqType.msgt_inline != TRUE) ||
	    (In0P->reqType.msgt_longform != FALSE) ||
	    (In0P->reqType.msgt_name != 8) ||
	    (In0P->reqType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->reqType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size < 60 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 4000);

#if	TypeCheck
	if (* (int *) &In1P->msgStartType != * (int *) &msgStartCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In1P->msgOolType.msgtl_header.msgt_longform != TRUE) ||
	    (In1P->msgOolType.msgtl_name != 8) ||
	    (In1P->msgOolType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In1P->msgOolType.msgtl_header.msgt_inline) ? (In1P->msgOolType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 60 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In2P = (Request *) ((char *) In1P + msgh_size_delta - 2048);

#if	TypeCheck
	if ((In2P->reqAttrType.msgt_inline != TRUE) ||
	    (In2P->reqAttrType.msgt_longform != FALSE) ||
	    (In2P->reqAttrType.msgt_name != 8) ||
	    (In2P->reqAttrType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 60 + ((In2P->reqAttrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

/* 
 * Begin x-kernel Sed hack
 */
	In1P->msgStart = (int)InHeadP;
/* 
 * End x-kernel Sed hack
 */
	OutP->RetCode = do_lproxy_xDemux(In0P->Head.msgh_request_port, &OutP->retVal, In0P->lls, In0P->req, In0P->reqType.msgt_number, In1P->msgStart, (In1P->msgOolType.msgtl_header.msgt_inline) ? In1P->msgOol : *((char **)In1P->msgOol), In1P->msgOolType.msgtl_number, In1P->msgOolType.msgtl_header.msgt_inline, In2P->reqAttr, In2P->reqAttrType.msgt_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retValType = retValType;
}

/* Routine xCallDemux */
mig_internal void _XxCallDemux
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Request *In2P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_lproxy_xCallDemux
		(mach_port_t reqPort, xkern_return_t *retVal, xobj_ext_id_t lls, xk_msg_data_t req, mach_msg_type_number_t reqCnt, int reqMsgStart, xk_large_msg_data_t reqool, mach_msg_type_number_t reqoolCnt, boolean_t reqoolSCopy, xk_msg_data_t rep, mach_msg_type_number_t *repCnt, xk_large_msg_data_t *repool, mach_msg_type_number_t *repoolCnt, xk_msg_attr_t reqAttr, mach_msg_type_number_t reqAttrCnt);

	boolean_t msgh_simple;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t llsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t reqMsgStartCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t retValType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t repType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		4000,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t repoolType = {
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

	mach_msg_type_number_t repCnt;
	char repool[2048];
	mach_msg_type_number_t repoolCnt;

	char *repoolP;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 60))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llsType != * (int *) &llsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->reqType.msgt_inline != TRUE) ||
	    (In0P->reqType.msgt_longform != FALSE) ||
	    (In0P->reqType.msgt_name != 8) ||
	    (In0P->reqType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->reqType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size < 60 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 4000);

#if	TypeCheck
	if (* (int *) &In1P->reqMsgStartType != * (int *) &reqMsgStartCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In1P->reqoolType.msgtl_header.msgt_longform != TRUE) ||
	    (In1P->reqoolType.msgtl_name != 8) ||
	    (In1P->reqoolType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In1P->reqoolType.msgtl_header.msgt_inline) ? (In1P->reqoolType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 60 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In2P = (Request *) ((char *) In1P + msgh_size_delta - 2048);

#if	TypeCheck
	if ((In2P->reqAttrType.msgt_inline != TRUE) ||
	    (In2P->reqAttrType.msgt_longform != FALSE) ||
	    (In2P->reqAttrType.msgt_name != 8) ||
	    (In2P->reqAttrType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 60 + ((In2P->reqAttrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	repCnt = 4000;

	repoolP = repool;
	repoolCnt = 2048;

/* 
 * Begin x-kernel Sed hack
 */
	In1P->reqMsgStart = (int)InHeadP;
/* 
 * End x-kernel Sed hack
 */
	OutP->RetCode = do_lproxy_xCallDemux(In0P->Head.msgh_request_port, &OutP->retVal, In0P->lls, In0P->req, In0P->reqType.msgt_number, In1P->reqMsgStart, (In1P->reqoolType.msgtl_header.msgt_inline) ? In1P->reqool : *((char **)In1P->reqool), In1P->reqoolType.msgtl_number, In1P->reqoolType.msgtl_header.msgt_inline, OutP->rep, &repCnt, &repoolP, &repoolCnt, In2P->reqAttr, In2P->reqAttrType.msgt_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->retValType = retValType;

	OutP->repType = repType;

	OutP->repType.msgt_number = repCnt;
	msgh_size_delta = (repCnt + 3) & ~3;
	msgh_size = 56 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 4000);

	OutP->repoolType = repoolType;

	if (repoolP != repool) {
		OutP->repoolType.msgtl_header.msgt_inline = FALSE;
		*((char **)OutP->repool) = repoolP;
		msgh_simple = FALSE;
	}
	else {
		memcpy(OutP->repool, repool, repoolCnt);
	}

	OutP->repoolType.msgtl_number = repoolCnt;
	msgh_size += (OutP->repoolType.msgtl_header.msgt_inline) ? (repoolCnt + 3) & ~3 : sizeof(char *);

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine ping */
mig_internal void _Xping
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_lproxy_ping
		(mach_port_t reqPort);

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_lproxy_ping(In0P->Head.msgh_request_port);
}

static mig_routine_t xk_lproxy_server_routines[] = {
		_XdumpXObj,
		_XxOpenDone,
		_XxDemux,
		_XxCallDemux,
		_Xping,
};

mig_external boolean_t xk_lproxy_server
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	register mach_msg_header_t *InP =  InHeadP;
	register mig_reply_header_t *OutP = (mig_reply_header_t *) OutHeadP;

	static const mach_msg_type_t RetCodeType = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	register mig_routine_t routine;

	OutP->Head.msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REPLY(InP->msgh_bits), 0);
	OutP->Head.msgh_size = sizeof *OutP;
	OutP->Head.msgh_remote_port = InP->msgh_reply_port;
	OutP->Head.msgh_local_port = MACH_PORT_NULL;
	OutP->Head.msgh_seqno = 0;
	OutP->Head.msgh_id = InP->msgh_id + 100;

	OutP->RetCodeType = RetCodeType;

	if ((InP->msgh_id > 844319) || (InP->msgh_id < 844315) ||
	    ((routine = xk_lproxy_server_routines[InP->msgh_id - 844315]) == 0)) {
		OutP->RetCode = MIG_BAD_ID;
		return FALSE;
	}
	(*routine) (InP, &OutP->Head);
/* 
 * Begin x-kernel Sed hack
 */
#ifdef XK_PROXY_MSG_HACK 
	if ( routine != _XxDemux && routine != _XxCallDemux ) {
	    xFree((char *)InP);
	}
#endif
/* 
 * End x-kernel Sed hack
 */
	return TRUE;
}

mig_external mig_routine_t xk_lproxy_server_routine
	(const mach_msg_header_t *InHeadP)
{
	register int msgh_id;

	msgh_id = InHeadP->msgh_id - 844315;

	if ((msgh_id > 4) || (msgh_id < 0))
		return 0;

	return xk_lproxy_server_routines[msgh_id];
}

