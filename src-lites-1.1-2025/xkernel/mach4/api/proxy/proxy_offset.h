/*     
 * $RCSfile: proxy_offset.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/04/08 17:28:17 $
 */

#ifndef PROXY_OFFSET_H
#define PROXY_OFFSET_H


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


typedef struct {
	mach_msg_header_t Head;
	mach_msg_type_t llsType;
	xobj_ext_id_t lls;
	mach_msg_type_t xmsgType;
	char xmsg[4000];
} XkPushRequest;

#define XK_PUSH_REQ_OFFSET	(offsetof(XkPushRequest, xmsg))


typedef struct {
	mach_msg_header_t Head;
	mach_msg_type_t RetCodeType;
	kern_return_t RetCode;
	mach_msg_type_t retValType;
	xkern_return_t retVal;
	mach_msg_type_t repmsgType;
	char repmsg[4000];
} XkCallReply;

#define XK_CALL_REP_OFFSET	(offsetof(XkCallReply, repmsg))


#endif /* ! PROXY_OFFSET_H */
