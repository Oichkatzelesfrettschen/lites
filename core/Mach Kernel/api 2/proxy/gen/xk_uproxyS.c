/* Module xk_uproxy */

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
char xk_uproxy_server_rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/proxy/RCS/xk_uproxy.defs,v 1.10 1993/09/16 22:04:16 menze Exp $";
#else	/* UseExternRCSId */
static char rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/proxy/RCS/xk_uproxy.defs,v 1.10 1993/09/16 22:04:16 menze Exp $";
#endif	/* UseExternRCSId */
#endif	/* lint */

#define msgh_request_port	msgh_local_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_LOCAL(bits)
#define msgh_reply_port		msgh_remote_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_REMOTE(bits)

#include <mach/std_types.h>
#include "xk_mig_sizes.h"
#include "xk_mig_t.h"
#include "uproxy.h"

/* Routine abort */
mig_internal void _Xabort
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t lproxyPortType;
		mach_port_t lproxyPort;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_abort
		(mach_port_t reqPort, mach_port_t lproxyPort);

	static const mach_msg_type_t lproxyPortCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 32) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->lproxyPortType != * (int *) &lproxyPortCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_abort(In0P->Head.msgh_request_port, In0P->lproxyPort);
}

/* Routine xGetProtlByName */
mig_internal void _XxGetProtlByName
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t nameType;
		xk_string_80_t name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t protlType;
		xobj_ext_id_t protl;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_xGetProtlByName
		(mach_port_t reqPort, xk_string_80_t name, xobj_ext_id_t *protl);

	static const mach_msg_type_t protlType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 116) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->nameType.msgtl_header.msgt_inline != TRUE) ||
	    (In0P->nameType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->nameType.msgtl_name != 12) ||
	    (In0P->nameType.msgtl_number != 1) ||
	    (In0P->nameType.msgtl_size != 640))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xGetProtlByName(In0P->Head.msgh_request_port, In0P->name, &OutP->protl);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->protlType = protlType;
}

/* Routine xControl */
mig_internal void _XxControl
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t objType;
		xobj_ext_id_t obj;
		mach_msg_type_t opcodeType;
		int opcode;
		mach_msg_type_t bufType;
		xk_ctl_buf_t buf;
		mach_msg_type_t bufLenType;
		int bufLen;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t bufType;
		xk_ctl_buf_t buf;
		mach_msg_type_t bufLenType;
		int bufLen;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_xControl
		(mach_port_t reqPort, xobj_ext_id_t obj, int opcode, xk_ctl_buf_t buf, int *bufLen);

	static const mach_msg_type_t objCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t opcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t bufCheck = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		200,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t bufLenCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t bufType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		200,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t bufLenType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 252) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->objType != * (int *) &objCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->opcodeType != * (int *) &opcodeCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->bufType != * (int *) &bufCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->bufLenType != * (int *) &bufLenCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xControl(In0P->Head.msgh_request_port, In0P->obj, In0P->opcode, In0P->buf, &In0P->bufLen);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 244;

	OutP->bufType = bufType;

	{ typedef struct { char data[200]; } *sp; * (sp) OutP->buf = * (sp) In0P->buf; }

	OutP->bufLenType = bufLenType;

	OutP->bufLen = In0P->bufLen;
}

/* Routine xOpenEnable */
mig_internal void _XxOpenEnable
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t lproxyPortType;
		mach_port_t lproxyPort;
		mach_msg_type_t hlpRcvType;
		xobj_ext_id_t hlpRcv;
		mach_msg_type_t hlpTypeType;
		xobj_ext_id_t hlpType;
		mach_msg_type_t llpType;
		xobj_ext_id_t llp;
		mach_msg_type_t partType;
		xk_part_t part;
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
	mig_external kern_return_t do_uproxy_xOpenEnable
		(mach_port_t reqPort, mach_port_t lproxyPort, xkern_return_t *retVal, xobj_ext_id_t hlpRcv, xobj_ext_id_t hlpType, xobj_ext_id_t llp, xk_part_t part);

	static const mach_msg_type_t lproxyPortCheck = {
		/* msgt_name = */		17,
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

	static const mach_msg_type_t llpCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t partCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1224,
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
	if ((In0P->Head.msgh_size != 4956) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->lproxyPortType != * (int *) &lproxyPortCheck)
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

#if	TypeCheck
	if (* (int *) &In0P->llpType != * (int *) &llpCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->partType != * (int *) &partCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xOpenEnable(In0P->Head.msgh_request_port, In0P->lproxyPort, &OutP->retVal, In0P->hlpRcv, In0P->hlpType, In0P->llp, In0P->part);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retValType = retValType;
}

/* Routine xOpenDisable */
mig_internal void _XxOpenDisable
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t lproxyPortType;
		mach_port_t lproxyPort;
		mach_msg_type_t hlpRcvType;
		xobj_ext_id_t hlpRcv;
		mach_msg_type_t hlpTypeType;
		xobj_ext_id_t hlpType;
		mach_msg_type_t llpType;
		xobj_ext_id_t llp;
		mach_msg_type_t partType;
		xk_part_t part;
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
	mig_external kern_return_t do_uproxy_xOpenDisable
		(mach_port_t reqPort, mach_port_t lproxyPort, xkern_return_t *retVal, xobj_ext_id_t hlpRcv, xobj_ext_id_t hlpType, xobj_ext_id_t llp, xk_part_t part);

	static const mach_msg_type_t lproxyPortCheck = {
		/* msgt_name = */		17,
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

	static const mach_msg_type_t llpCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t partCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1224,
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
	if ((In0P->Head.msgh_size != 4956) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->lproxyPortType != * (int *) &lproxyPortCheck)
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

#if	TypeCheck
	if (* (int *) &In0P->llpType != * (int *) &llpCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->partType != * (int *) &partCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xOpenDisable(In0P->Head.msgh_request_port, In0P->lproxyPort, &OutP->retVal, In0P->hlpRcv, In0P->hlpType, In0P->llp, In0P->part);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retValType = retValType;
}

/* Routine xOpen */
mig_internal void _XxOpen
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t lproxyPortType;
		mach_port_t lproxyPort;
		mach_msg_type_t hlpRcvType;
		xobj_ext_id_t hlpRcv;
		mach_msg_type_t hlpTypeType;
		xobj_ext_id_t hlpType;
		mach_msg_type_t llpType;
		xobj_ext_id_t llp;
		mach_msg_type_t partType;
		xk_part_t part;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retObjType;
		xobj_ext_id_t retObj;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_xOpen
		(mach_port_t reqPort, mach_port_t lproxyPort, xobj_ext_id_t *retObj, xobj_ext_id_t hlpRcv, xobj_ext_id_t hlpType, xobj_ext_id_t llp, xk_part_t part);

	static const mach_msg_type_t lproxyPortCheck = {
		/* msgt_name = */		17,
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

	static const mach_msg_type_t llpCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t partCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1224,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t retObjType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 4956) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->lproxyPortType != * (int *) &lproxyPortCheck)
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

#if	TypeCheck
	if (* (int *) &In0P->llpType != * (int *) &llpCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->partType != * (int *) &partCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xOpen(In0P->Head.msgh_request_port, In0P->lproxyPort, &OutP->retObj, In0P->hlpRcv, In0P->hlpType, In0P->llp, In0P->part);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retObjType = retObjType;
}

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
	mig_external kern_return_t do_uproxy_dumpXObj
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

	OutP->RetCode = do_uproxy_dumpXObj(In0P->Head.msgh_request_port, &OutP->retVal, In0P->obj, &OutP->dump);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 220;

	OutP->retValType = retValType;

	OutP->dumpType = dumpType;
}

/* Routine xDuplicate */
mig_internal void _XxDuplicate
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t lproxyPortType;
		mach_port_t lproxyPort;
		mach_msg_type_t hlpType;
		xobj_ext_id_t hlp;
		mach_msg_type_t objType;
		xobj_ext_id_t obj;
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
	mig_external kern_return_t do_uproxy_xDuplicate
		(mach_port_t reqPort, mach_port_t lproxyPort, xkern_return_t *retVal, xobj_ext_id_t hlp, xobj_ext_id_t obj);

	static const mach_msg_type_t lproxyPortCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t hlpCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

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

#if	TypeCheck
	if ((In0P->Head.msgh_size != 48) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->lproxyPortType != * (int *) &lproxyPortCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->hlpType != * (int *) &hlpCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->objType != * (int *) &objCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xDuplicate(In0P->Head.msgh_request_port, In0P->lproxyPort, &OutP->retVal, In0P->hlp, In0P->obj);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retValType = retValType;
}

/* Routine xPush */
mig_internal void _XxPush
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t llsType;
		xobj_ext_id_t lls;
		mach_msg_type_t xmsgType;
		char xmsg[4000];
		mach_msg_type_long_t xmsgoolType;
		char xmsgool[2048];
		mach_msg_type_t attrType;
		char attr[200];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t handleType;
		xmsg_handle_t handle;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Request *In2P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_xPush
		(mach_port_t reqPort, xmsg_handle_t *handle, xobj_ext_id_t lls, xk_msg_data_t xmsg, mach_msg_type_number_t xmsgCnt, xk_large_msg_data_t xmsgool, mach_msg_type_number_t xmsgoolCnt, boolean_t xmsgoolSCopy, xk_msg_attr_t attr, mach_msg_type_number_t attrCnt);

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

	static const mach_msg_type_t handleType = {
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
	if ((msgh_size < 52))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llsType != * (int *) &llsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->xmsgType.msgt_inline != TRUE) ||
	    (In0P->xmsgType.msgt_longform != FALSE) ||
	    (In0P->xmsgType.msgt_name != 8) ||
	    (In0P->xmsgType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->xmsgType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size < 52 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 4000);

#if	TypeCheck
	if ((In1P->xmsgoolType.msgtl_header.msgt_longform != TRUE) ||
	    (In1P->xmsgoolType.msgtl_name != 8) ||
	    (In1P->xmsgoolType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In1P->xmsgoolType.msgtl_header.msgt_inline) ? (In1P->xmsgoolType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 52 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In2P = (Request *) ((char *) In1P + msgh_size_delta - 2048);

#if	TypeCheck
	if ((In2P->attrType.msgt_inline != TRUE) ||
	    (In2P->attrType.msgt_longform != FALSE) ||
	    (In2P->attrType.msgt_name != 8) ||
	    (In2P->attrType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 52 + ((In2P->attrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xPush(In0P->Head.msgh_request_port, &OutP->handle, In0P->lls, In0P->xmsg, In0P->xmsgType.msgt_number, (In1P->xmsgoolType.msgtl_header.msgt_inline) ? In1P->xmsgool : *((char **)In1P->xmsgool), In1P->xmsgoolType.msgtl_number, In1P->xmsgoolType.msgtl_header.msgt_inline, In2P->attr, In2P->attrType.msgt_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->handleType = handleType;
}

/* Routine xCall */
mig_internal void _XxCall
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t llsType;
		xobj_ext_id_t lls;
		mach_msg_type_t reqmsgType;
		char reqmsg[4000];
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
		mach_msg_type_t repmsgType;
		char repmsg[4000];
		mach_msg_type_long_t repoolType;
		char repool[2048];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Request *In2P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_xCall
		(mach_port_t reqPort, xkern_return_t *retVal, xobj_ext_id_t lls, xk_msg_data_t reqmsg, mach_msg_type_number_t reqmsgCnt, xk_large_msg_data_t reqool, mach_msg_type_number_t reqoolCnt, boolean_t reqoolSCopy, xk_msg_data_t repmsg, mach_msg_type_number_t *repmsgCnt, xk_large_msg_data_t *repool, mach_msg_type_number_t *repoolCnt, xk_msg_attr_t reqAttr, mach_msg_type_number_t reqAttrCnt);

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

	static const mach_msg_type_t retValType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t repmsgType = {
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

	mach_msg_type_number_t repmsgCnt;
	char repool[2048];
	mach_msg_type_number_t repoolCnt;

	char *repoolP;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 52))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llsType != * (int *) &llsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->reqmsgType.msgt_inline != TRUE) ||
	    (In0P->reqmsgType.msgt_longform != FALSE) ||
	    (In0P->reqmsgType.msgt_name != 8) ||
	    (In0P->reqmsgType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->reqmsgType.msgt_number + 3) & ~3;
#if	TypeCheck
	if (msgh_size < 52 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 4000);

#if	TypeCheck
	if ((In1P->reqoolType.msgtl_header.msgt_longform != TRUE) ||
	    (In1P->reqoolType.msgtl_name != 8) ||
	    (In1P->reqoolType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In1P->reqoolType.msgtl_header.msgt_inline) ? (In1P->reqoolType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 52 + msgh_size_delta)
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
	if (msgh_size != 52 + ((In2P->reqAttrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	repmsgCnt = 4000;

	repoolP = repool;
	repoolCnt = 2048;

	OutP->RetCode = do_uproxy_xCall(In0P->Head.msgh_request_port, &OutP->retVal, In0P->lls, In0P->reqmsg, In0P->reqmsgType.msgt_number, (In1P->reqoolType.msgtl_header.msgt_inline) ? In1P->reqool : *((char **)In1P->reqool), In1P->reqoolType.msgtl_number, In1P->reqoolType.msgtl_header.msgt_inline, OutP->repmsg, &repmsgCnt, &repoolP, &repoolCnt, In2P->reqAttr, In2P->reqAttrType.msgt_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->retValType = retValType;

	OutP->repmsgType = repmsgType;

	OutP->repmsgType.msgt_number = repmsgCnt;
	msgh_size_delta = (repmsgCnt + 3) & ~3;
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

/* Routine xClose */
mig_internal void _XxClose
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t llsType;
		xobj_ext_id_t lls;
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
	mig_external kern_return_t do_uproxy_xClose
		(mach_port_t reqPort, xkern_return_t *retVal, xobj_ext_id_t lls);

	static const mach_msg_type_t llsCheck = {
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
	if ((In0P->Head.msgh_size != 32) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->llsType != * (int *) &llsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_xClose(In0P->Head.msgh_request_port, &OutP->retVal, In0P->lls);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->retValType = retValType;
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
	mig_external kern_return_t do_uproxy_ping
		(mach_port_t reqPort);

#if	TypeCheck
	if ((In0P->Head.msgh_size != 24) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_ping(In0P->Head.msgh_request_port);
}

/* Routine pingtest */
mig_internal void _Xpingtest
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t repPortType;
		mach_port_t repPort;
		mach_msg_type_t numberType;
		int number;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_uproxy_pingtest
		(mach_port_t reqPort, mach_port_t repPort, int number);

	static const mach_msg_type_t repPortCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t numberCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->repPortType != * (int *) &repPortCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->numberType != * (int *) &numberCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_uproxy_pingtest(In0P->Head.msgh_request_port, In0P->repPort, In0P->number);
}

static mig_routine_t xk_uproxy_server_routines[] = {
		_Xabort,
		_XxGetProtlByName,
		_XxControl,
		_XxOpenEnable,
		_XxOpenDisable,
		_XxOpen,
		_XdumpXObj,
		_XxDuplicate,
		_XxPush,
		_XxCall,
		_XxClose,
		_Xping,
		_Xpingtest,
};

mig_external boolean_t xk_uproxy_server
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

	if ((InP->msgh_id > 844427) || (InP->msgh_id < 844415) ||
	    ((routine = xk_uproxy_server_routines[InP->msgh_id - 844415]) == 0)) {
		OutP->RetCode = MIG_BAD_ID;
		return FALSE;
	}
	(*routine) (InP, &OutP->Head);
	return TRUE;
}

mig_external mig_routine_t xk_uproxy_server_routine
	(const mach_msg_header_t *InHeadP)
{
	register int msgh_id;

	msgh_id = InHeadP->msgh_id - 844415;

	if ((msgh_id > 12) || (msgh_id < 0))
		return 0;

	return xk_uproxy_server_routines[msgh_id];
}

