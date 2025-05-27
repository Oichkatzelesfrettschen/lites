/* Module xsi */

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
char xsi_server_rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/socket/RCS/xsi.defs,v 1.6 1993/02/02 00:08:39 menze Exp $";
#else	/* UseExternRCSId */
static char rcsid[] = "$Header: /usr/var/tmp/x32/tmpdist/mach3/api/socket/RCS/xsi.defs,v 1.6 1993/02/02 00:08:39 menze Exp $";
#endif	/* UseExternRCSId */
#endif	/* lint */

#define msgh_request_port	msgh_local_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_LOCAL(bits)
#define msgh_reply_port		msgh_remote_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_REMOTE(bits)

#include <mach/std_types.h>
#include "xsi_types.h"

/* Routine hello */
mig_internal void _Xhello
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_hello
		(mach_port_t server_port, xsi_clientid_t client_id, mach_port_t signal_port, xsi_fdset_t is_xsi_fd, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t signal_portCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t is_xsi_fdType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->signal_portType != * (int *) &signal_portCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_hello(In0P->Head.msgh_request_port, In0P->client_id, In0P->signal_port, OutP->is_xsi_fd, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 76;

	OutP->is_xsi_fdType = is_xsi_fdType;

	OutP->errnumType = errnumType;
}

/* Routine clone */
mig_internal void _Xclone
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_clone
		(mach_port_t server_port, xsi_clientid_t client_id, int parent_id, mach_port_t port, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t parent_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t portCheck = {
		/* msgt_name = */		17,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->parent_idType != * (int *) &parent_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->portType != * (int *) &portCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_clone(In0P->Head.msgh_request_port, In0P->client_id, In0P->parent_id, In0P->port, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine await_clone_done */
mig_internal void _Xawait_clone_done
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_await_clone_done
		(mach_port_t server_port, xsi_clientid_t client_id, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_await_clone_done(In0P->Head.msgh_request_port, In0P->client_id, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine accept */
mig_internal void _Xaccept
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_accept
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_sockaddr_t addr, mach_msg_type_number_t *addrCnt, int ns, int *errnum);

	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nsCheck = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t addrCnt;
	int errnum;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 48) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->nsType != * (int *) &nsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	addrCnt = 16;

	OutP->RetCode = do_accept(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, OutP->addr, &addrCnt, In0P->ns, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->addrType = addrType;

	OutP->addrType.msgt_number = addrCnt;
	msgh_size_delta = (addrCnt + 3) & ~3;
	OutP->Head.msgh_size = 44 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;
}

/* Routine bind */
mig_internal void _Xbind
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_bind
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_sockaddr_t addr, mach_msg_type_number_t addrCnt, int *errnum);

	unsigned int msgh_size;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if ((msgh_size < 44) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->addrType.msgt_inline != TRUE) ||
	    (In0P->addrType.msgt_longform != FALSE) ||
	    (In0P->addrType.msgt_name != 8) ||
	    (In0P->addrType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 44 + ((In0P->addrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_bind(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->addr, In0P->addrType.msgt_number, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine close */
mig_internal void _Xclose
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_close
		(mach_port_t server_port, xsi_clientid_t client_id, int s, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_close(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine connect */
mig_internal void _Xconnect
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_connect
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_sockaddr_t addr, mach_msg_type_number_t addrCnt, int *errnum);

	unsigned int msgh_size;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if ((msgh_size < 44) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->addrType.msgt_inline != TRUE) ||
	    (In0P->addrType.msgt_longform != FALSE) ||
	    (In0P->addrType.msgt_name != 8) ||
	    (In0P->addrType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 44 + ((In0P->addrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_connect(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->addr, In0P->addrType.msgt_number, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine dup2 */
mig_internal void _Xdup2
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_dup2
		(mach_port_t server_port, xsi_clientid_t client_id, int from, int to, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t fromCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t toCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->fromType != * (int *) &fromCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->toType != * (int *) &toCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_dup2(In0P->Head.msgh_request_port, In0P->client_id, In0P->from, In0P->to, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine listen */
mig_internal void _Xlisten
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_listen
		(mach_port_t server_port, xsi_clientid_t client_id, int s, int backlog, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t backlogCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->backlogType != * (int *) &backlogCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_listen(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->backlog, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine socket */
mig_internal void _Xsocket
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_socket
		(mach_port_t server_port, xsi_clientid_t client_id, int domain, int conn_type, int protocol, int s, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t domainCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t conn_typeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t protocolCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 64) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->domainType != * (int *) &domainCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->conn_typeType != * (int *) &conn_typeCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->protocolType != * (int *) &protocolCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_socket(In0P->Head.msgh_request_port, In0P->client_id, In0P->domain, In0P->conn_type, In0P->protocol, In0P->s, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine getpeername */
mig_internal void _Xgetpeername
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_getpeername
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_sockaddr_t addr, mach_msg_type_number_t *addrCnt, int *errnum);

	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t addrCnt;
	int errnum;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	addrCnt = 16;

	OutP->RetCode = do_getpeername(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, OutP->addr, &addrCnt, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->addrType = addrType;

	OutP->addrType.msgt_number = addrCnt;
	msgh_size_delta = (addrCnt + 3) & ~3;
	OutP->Head.msgh_size = 44 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;
}

/* Routine getsockname */
mig_internal void _Xgetsockname
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_getsockname
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_sockaddr_t addr, mach_msg_type_number_t *addrCnt, int *errnum);

	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t addrCnt;
	int errnum;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 40) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	addrCnt = 16;

	OutP->RetCode = do_getsockname(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, OutP->addr, &addrCnt, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->addrType = addrType;

	OutP->addrType.msgt_number = addrCnt;
	msgh_size_delta = (addrCnt + 3) & ~3;
	OutP->Head.msgh_size = 44 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;
}

/* Routine getsockopt */
mig_internal void _Xgetsockopt
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_getsockopt
		(mach_port_t server_port, xsi_clientid_t client_id, int s, int level, int optname, xsi_varbuf_t *optval, mach_msg_type_number_t *optvalCnt, int *errnum);

	boolean_t msgh_simple;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t levelCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t optnameCheck = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t optvalCnt;
	int errnum;

	char *optvalP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 56) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->levelType != * (int *) &levelCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->optnameType != * (int *) &optnameCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	optvalP = OutP->optval;
	optvalCnt = 2048;

	OutP->RetCode = do_getsockopt(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->level, In0P->optname, &optvalP, &optvalCnt, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->optvalType = optvalType;
	if (optvalP != OutP->optval) {
		OutP->optvalType.msgtl_header.msgt_inline = FALSE;
		OutP->optvalType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->optval) = optvalP;
		msgh_simple = FALSE;
	}

	OutP->optvalType.msgtl_number = optvalCnt;
	msgh_size_delta = (OutP->optvalType.msgtl_header.msgt_inline) ? (optvalCnt + 3) & ~3 : sizeof(char *);
	OutP->Head.msgh_size = 52 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine setsockopt */
mig_internal void _Xsetsockopt
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_setsockopt
		(mach_port_t server_port, xsi_clientid_t client_id, int s, int level, int optname, xsi_varbuf_t optval, mach_msg_type_number_t optvalCnt, int *errnum);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t levelCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t optnameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if ((msgh_size < 68))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->levelType != * (int *) &levelCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->optnameType != * (int *) &optnameCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->optvalType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->optvalType.msgtl_name != 8) ||
	    (In0P->optvalType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 68 + ((In0P->optvalType.msgtl_header.msgt_inline) ? (In0P->optvalType.msgtl_number + 3) & ~3 : sizeof(char *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_setsockopt(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->level, In0P->optname, (In0P->optvalType.msgtl_header.msgt_inline) ? In0P->optval : *((char **)In0P->optval), In0P->optvalType.msgtl_number, &OutP->errnum);
	if (!In0P->optvalType.msgtl_header.msgt_inline)
		mig_deallocate(* (vm_offset_t *) In0P->optval,  In0P->optvalType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine select */
mig_internal void _Xselect
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_select
		(mach_port_t server_port, xsi_clientid_t client_id, int seqno, int *nfds, xsi_select_mask_t specified, xsi_fdset_t readfds, xsi_fdset_t writefds, xsi_fdset_t exceptfds, xsi_timeval_t timeout, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t seqnoCheck = {
		/* msgt_name = */		2,
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

	static const mach_msg_type_t specifiedCheck = {
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

	static const mach_msg_type_t timeoutCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		2,
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

#if	TypeCheck
	if ((In0P->Head.msgh_size != 176) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->seqnoType != * (int *) &seqnoCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->nfdsType != * (int *) &nfdsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->specifiedType != * (int *) &specifiedCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->readfdsType != * (int *) &readfdsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->writefdsType != * (int *) &writefdsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->exceptfdsType != * (int *) &exceptfdsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->timeoutType != * (int *) &timeoutCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_select(In0P->Head.msgh_request_port, In0P->client_id, In0P->seqno, &In0P->nfds, In0P->specified, In0P->readfds, In0P->writefds, In0P->exceptfds, In0P->timeout, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 156;

	OutP->nfdsType = nfdsType;

	OutP->nfds = In0P->nfds;

	OutP->readfdsType = readfdsType;

	{ typedef struct { char data[32]; } *sp; * (sp) OutP->readfds = * (sp) In0P->readfds; }

	OutP->writefdsType = writefdsType;

	{ typedef struct { char data[32]; } *sp; * (sp) OutP->writefds = * (sp) In0P->writefds; }

	OutP->exceptfdsType = exceptfdsType;

	{ typedef struct { char data[32]; } *sp; * (sp) OutP->exceptfds = * (sp) In0P->exceptfds; }

	OutP->errnumType = errnumType;
}

/* Routine select_cancel */
mig_internal void _Xselect_cancel
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_select_cancel
		(mach_port_t server_port, xsi_clientid_t client_id, int seqno, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t seqnoCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->seqnoType != * (int *) &seqnoCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_select_cancel(In0P->Head.msgh_request_port, In0P->client_id, In0P->seqno, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 40;

	OutP->errnumType = errnumType;
}

/* Routine ioctl */
mig_internal void _Xioctl
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_ioctl
		(mach_port_t server_port, xsi_clientid_t client_id, int s, int request, xsi_varbuf_t inp, mach_msg_type_number_t inpCnt, xsi_varbuf_t *outp, mach_msg_type_number_t *outpCnt, int *errnum);

	boolean_t msgh_simple;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t requestCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_long_t outpType = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t outpCnt;
	int errnum;

	char *outpP;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 60))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->requestType != * (int *) &requestCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->inpType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->inpType.msgtl_name != 8) ||
	    (In0P->inpType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 60 + ((In0P->inpType.msgtl_header.msgt_inline) ? (In0P->inpType.msgtl_number + 3) & ~3 : sizeof(char *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	outpP = OutP->outp;
	outpCnt = 2048;

	OutP->RetCode = do_ioctl(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->request, (In0P->inpType.msgtl_header.msgt_inline) ? In0P->inp : *((char **)In0P->inp), In0P->inpType.msgtl_number, &outpP, &outpCnt, &errnum);
	if (!In0P->inpType.msgtl_header.msgt_inline)
		mig_deallocate(* (vm_offset_t *) In0P->inp,  In0P->inpType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->outpType = outpType;
	if (outpP != OutP->outp) {
		OutP->outpType.msgtl_header.msgt_inline = FALSE;
		OutP->outpType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->outp) = outpP;
		msgh_simple = FALSE;
	}

	OutP->outpType.msgtl_number = outpCnt;
	msgh_size_delta = (OutP->outpType.msgtl_header.msgt_inline) ? (outpCnt + 3) & ~3 : sizeof(char *);
	OutP->Head.msgh_size = 52 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine fcntl */
mig_internal void _Xfcntl
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_fcntl
		(mach_port_t server_port, xsi_clientid_t client_id, int s, int cmd, int *arg, int *errnum);

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t cmdCheck = {
		/* msgt_name = */		2,
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

	static const mach_msg_type_t argType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->cmdType != * (int *) &cmdCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->argType != * (int *) &argCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_fcntl(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, In0P->cmd, &In0P->arg, &OutP->errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 48;

	OutP->argType = argType;

	OutP->arg = In0P->arg;

	OutP->errnumType = errnumType;
}

/* Routine read */
mig_internal void _Xread
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_read
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_varbuf_t *buf, mach_msg_type_number_t *bufCnt, int nbytes, int *errnum);

	boolean_t msgh_simple;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nbytesCheck = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t bufCnt;
	int errnum;

	char *bufP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 48) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->nbytesType != * (int *) &nbytesCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	bufP = OutP->buf;
	bufCnt = 2048;

	OutP->RetCode = do_read(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, &bufP, &bufCnt, In0P->nbytes, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->bufType = bufType;
	if (bufP != OutP->buf) {
		OutP->bufType.msgtl_header.msgt_inline = FALSE;
		OutP->bufType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->buf) = bufP;
		msgh_simple = FALSE;
	}

	OutP->bufType.msgtl_number = bufCnt;
	msgh_size_delta = (OutP->bufType.msgtl_header.msgt_inline) ? (bufCnt + 3) & ~3 : sizeof(char *);
	OutP->Head.msgh_size = 52 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine write */
mig_internal void _Xwrite
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_write
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_varbuf_t buf, mach_msg_type_number_t bufCnt, int *nwritten, int *errnum);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nwrittenType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->bufType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->bufType.msgtl_name != 8) ||
	    (In0P->bufType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 52 + ((In0P->bufType.msgtl_header.msgt_inline) ? (In0P->bufType.msgtl_number + 3) & ~3 : sizeof(char *)))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_write(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, (In0P->bufType.msgtl_header.msgt_inline) ? In0P->buf : *((char **)In0P->buf), In0P->bufType.msgtl_number, &OutP->nwritten, &OutP->errnum);
	if (!In0P->bufType.msgtl_header.msgt_inline)
		mig_deallocate(* (vm_offset_t *) In0P->buf,  In0P->bufType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 48;

	OutP->nwrittenType = nwrittenType;

	OutP->errnumType = errnumType;
}

/* Routine recv */
mig_internal void _Xrecv
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_recv
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_varbuf_t *buf, mach_msg_type_number_t *bufCnt, int maxlen, int flags, int *errnum);

	boolean_t msgh_simple;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t maxlenCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t flagsCheck = {
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

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t bufCnt;
	int errnum;

	char *bufP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 56) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->maxlenType != * (int *) &maxlenCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->flagsType != * (int *) &flagsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	bufP = OutP->buf;
	bufCnt = 2048;

	OutP->RetCode = do_recv(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, &bufP, &bufCnt, In0P->maxlen, In0P->flags, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->bufType = bufType;
	if (bufP != OutP->buf) {
		OutP->bufType.msgtl_header.msgt_inline = FALSE;
		OutP->bufType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->buf) = bufP;
		msgh_simple = FALSE;
	}

	OutP->bufType.msgtl_number = bufCnt;
	msgh_size_delta = (OutP->bufType.msgtl_header.msgt_inline) ? (bufCnt + 3) & ~3 : sizeof(char *);
	OutP->Head.msgh_size = 52 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
}

/* Routine recvfrom */
mig_internal void _Xrecvfrom
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_recvfrom
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_varbuf_t *buf, mach_msg_type_number_t *bufCnt, int maxlen, int flags, xsi_sockaddr_t from_addr, mach_msg_type_number_t *from_addrCnt, int *errnum);

	boolean_t msgh_simple;
	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t maxlenCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t flagsCheck = {
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

	static const mach_msg_type_t from_addrType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		16,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	mach_msg_type_number_t bufCnt;
	char from_addr[16];
	mach_msg_type_number_t from_addrCnt;
	int errnum;

	char *bufP;

#if	TypeCheck
	if ((In0P->Head.msgh_size != 56) ||
	    (In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->maxlenType != * (int *) &maxlenCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->flagsType != * (int *) &flagsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	bufP = OutP->buf;
	bufCnt = 2048;

	from_addrCnt = 16;

	OutP->RetCode = do_recvfrom(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, &bufP, &bufCnt, In0P->maxlen, In0P->flags, from_addr, &from_addrCnt, &errnum);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;

	OutP->bufType = bufType;
	if (bufP != OutP->buf) {
		OutP->bufType.msgtl_header.msgt_inline = FALSE;
		OutP->bufType.msgtl_header.msgt_deallocate = TRUE;
		*((char **)OutP->buf) = bufP;
		msgh_simple = FALSE;
	}

	OutP->bufType.msgtl_number = bufCnt;
	msgh_size_delta = (OutP->bufType.msgtl_header.msgt_inline) ? (bufCnt + 3) & ~3 : sizeof(char *);
	msgh_size = 56 + msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 2048);

	OutP->from_addrType = from_addrType;

	memcpy(OutP->from_addr, from_addr, from_addrCnt);

	OutP->from_addrType.msgt_number = from_addrCnt;
	msgh_size_delta = (from_addrCnt + 3) & ~3;
	msgh_size += msgh_size_delta;
	OutP = (Reply *) ((char *) OutP + msgh_size_delta - 16);

	OutP->errnumType = errnumType;

	OutP->errnum = errnum;

	OutP = (Reply *) OutHeadP;
	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine send */
mig_internal void _Xsend
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_send
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_varbuf_t msg, mach_msg_type_number_t msgCnt, int *nsent, int flags, int *errnum);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t flagsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nsentType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->msgType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->msgType.msgtl_name != 8) ||
	    (In0P->msgType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->msgType.msgtl_header.msgt_inline) ? (In0P->msgType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size != 60 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 2048);

#if	TypeCheck
	if (* (int *) &In1P->flagsType != * (int *) &flagsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_send(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, (In0P->msgType.msgtl_header.msgt_inline) ? In0P->msg : *((char **)In0P->msg), In0P->msgType.msgtl_number, &OutP->nsent, In1P->flags, &OutP->errnum);
	if (!In0P->msgType.msgtl_header.msgt_inline)
		mig_deallocate(* (vm_offset_t *) In0P->msg,  In0P->msgType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 48;

	OutP->nsentType = nsentType;

	OutP->errnumType = errnumType;
}

/* Routine sendto */
mig_internal void _Xsendto
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
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

	register Request *In0P = (Request *) InHeadP;
	register Request *In1P;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t do_sendto
		(mach_port_t server_port, xsi_clientid_t client_id, int s, xsi_varbuf_t msg, mach_msg_type_number_t msgCnt, int *nsent, int flags, xsi_sockaddr_t toaddr, mach_msg_type_number_t toaddrCnt, int *errnum);

#if	TypeCheck
	boolean_t msgh_simple;
#endif	/* TypeCheck */

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

	static const mach_msg_type_t client_idCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t sCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t flagsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t nsentType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};

	static const mach_msg_type_t errnumType = {
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
	if ((msgh_size < 64))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->client_idType != * (int *) &client_idCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (* (int *) &In0P->sType != * (int *) &sCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In0P->msgType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->msgType.msgtl_name != 8) ||
	    (In0P->msgType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	msgh_size_delta = (In0P->msgType.msgtl_header.msgt_inline) ? (In0P->msgType.msgtl_number + 3) & ~3 : sizeof(char *);
#if	TypeCheck
	if (msgh_size < 64 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
	msgh_size -= msgh_size_delta;
#endif	/* TypeCheck */

	In1P = (Request *) ((char *) In0P + msgh_size_delta - 2048);

#if	TypeCheck
	if (* (int *) &In1P->flagsType != * (int *) &flagsCheck)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if ((In1P->toaddrType.msgt_inline != TRUE) ||
	    (In1P->toaddrType.msgt_longform != FALSE) ||
	    (In1P->toaddrType.msgt_name != 8) ||
	    (In1P->toaddrType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

#if	TypeCheck
	if (msgh_size != 64 + ((In1P->toaddrType.msgt_number + 3) & ~3))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	/* TypeCheck */

	OutP->RetCode = do_sendto(In0P->Head.msgh_request_port, In0P->client_id, In0P->s, (In0P->msgType.msgtl_header.msgt_inline) ? In0P->msg : *((char **)In0P->msg), In0P->msgType.msgtl_number, &OutP->nsent, In1P->flags, In1P->toaddr, In1P->toaddrType.msgt_number, &OutP->errnum);
	if (!In0P->msgType.msgtl_header.msgt_inline)
		mig_deallocate(* (vm_offset_t *) In0P->msg,  In0P->msgType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	OutP->Head.msgh_size = 48;

	OutP->nsentType = nsentType;

	OutP->errnumType = errnumType;
}

static mig_routine_t xsi_server_routines[] = {
		_Xhello,
		_Xclone,
		_Xawait_clone_done,
		_Xaccept,
		_Xbind,
		_Xclose,
		_Xconnect,
		_Xdup2,
		_Xlisten,
		_Xsocket,
		_Xgetpeername,
		_Xgetsockname,
		_Xgetsockopt,
		_Xsetsockopt,
		_Xselect,
		_Xselect_cancel,
		_Xioctl,
		_Xfcntl,
		_Xread,
		_Xwrite,
		_Xrecv,
		_Xrecvfrom,
		_Xsend,
		_Xsendto,
};

mig_external boolean_t xsi_server
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

	if ((InP->msgh_id > 102194) || (InP->msgh_id < 102171) ||
	    ((routine = xsi_server_routines[InP->msgh_id - 102171]) == 0)) {
		OutP->RetCode = MIG_BAD_ID;
		return FALSE;
	}
	(*routine) (InP, &OutP->Head);
	return TRUE;
}

mig_external mig_routine_t xsi_server_routine
	(const mach_msg_header_t *InHeadP)
{
	register int msgh_id;

	msgh_id = InHeadP->msgh_id - 102171;

	if ((msgh_id > 23) || (msgh_id < 0))
		return 0;

	return xsi_server_routines[msgh_id];
}

