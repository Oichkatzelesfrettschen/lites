/*     
 * $RCSfile: lproxy_user.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/03/24 22:29:25 $
 */

/* 
 * We determine the offsets of certain fields within certain MIG
 * request structures.  We do this because we need to know these
 * offsets before we get to the routines where the actual structure is
 * defined.  If the defs file is modified in such a way that the
 * structure offset changes, assertions will be tripped in the stub
 * code.  This isn't pretty ...
 */


typedef struct {
	mach_msg_header_t Head;
	mach_msg_type_t llsType;
	xobj_ext_id_t lls;
	mach_msg_type_t reqType;
	char req[4000];
	mach_msg_type_t msgStartType;
	int msgStart;
} XkDemuxRequest;

#define XK_DEMUX_REQ_OFFSET	(offsetof(XkDemuxRequest, req))
