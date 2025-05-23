#
# $RCSfile: lproxy.demux.sed,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
#
# $Revision: 1.6 $
# $Date: 1993/11/09 18:00:33 $
#

#
# Sed hacks for the xDemux and xCallDemux stubs of the xk_lproxyUser
# MIG output. 


#mips
/^		bcopy((char \*) req, (char \*) InP->req, reqCnt);/c\
/* \
\ * Begin x-kernel Sed hack - Mips\
\ */\
\	\	xAssert( offsetof(Request, req) == XK_DEMUX_REQ_OFFSET); \
\	\	if ( ((xk_and_mach_msg_t *)req)->xkMsg ) { \
\	\	    msg2buf(((xk_and_mach_msg_t *)req)->xkMsg, (char *) InP->req); \
\	\	} \
\/* \
\ * End x-kernel Sed hack\
\ */

#alternate mig version
/^		memcpy(InP->req, req, reqCnt);/c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	\	xAssert( offsetof(Request, req) == XK_DEMUX_REQ_OFFSET); \
\	\	if ( ((xk_and_mach_msg_t *)req)->xkMsg ) { \
\	\	    msg2buf(((xk_and_mach_msg_t *)req)->xkMsg, (char *) InP->req); \
\	\	} \
\/* \
\ * End x-kernel Sed hack\
\ */


/^	union {/c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	typedef union {

/^	} Mess;/c\
\	} union_mess;\
\
\	union_mess	*Mess = (union_mess *)((xk_and_mach_msg_t *)req)->machMsg; \
\/* \
\ * End x-kernel Sed hack\
\ */

/^		bcopy((char \*) OutP->rep, (char \*) rep, \*repCnt);/d

/^		\*repCnt = OutP->repType.msgt_number;/d

#mips
/^		bcopy((char \*) OutP->rep, (char \*) rep, OutP->repType.msgt_number);/c\
/* \
\ * x-kernel Sed hack deleted bcopy - Mips \
\ *		bcopy((char \*) OutP->rep, (char \*) rep, OutP->repType.msgt_number);\
\ */\


#alternate mig version
/^		memcpy(rep, OutP->rep, OutP->repType.msgt_number);/c\
/* \
\ * x-kernel Sed hack deleted memcopy\
\ *		memcpy(rep, OutP->rep, OutP->repType.msgt_number);\
\ */\


/&Mess\./s/Mess\./Mess->/


