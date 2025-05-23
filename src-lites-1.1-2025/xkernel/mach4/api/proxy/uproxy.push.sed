#     
# $RCSfile: uproxy.push.sed,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
#
# $Revision: 1.4 $
# $Date: 1993/11/09 18:00:33 $
#

#
# Sed Hacks to modify the xPush routine of the 
# xk_uproxyUser MIG output 
#



#
# As an efficiency hack, we may just build the mach message around the
# x-kernel message buffer.  If we do this, we need to record the
# address of this buffer and catch all assignments to InP or OutP in
# the stub.

/	unsigned int msgh_size_delta;/a\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	char	*machMsgStart; \
\/* \
\ * End x-kernel Sed hack\
\ */


/	InP->llsType/i\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	if ( ((xk_and_mach_msg_t *)xmsg)->machMsg ) {	\
\	\	xAssert( offsetof(Request,xmsg) == XK_PUSH_REQ_OFFSET );  \
\	\	InP = (Request *)((xk_and_mach_msg_t *)xmsg)->machMsg;	\
\	\	OutP = (Reply *)((xk_and_mach_msg_t *)xmsg)->machMsg;	\
\	\	xAssert( ! ((xk_and_mach_msg_t *)xmsg)->xkMsg );	\
\	} \
\	machMsgStart = (char *)InP; \
\/* \
\ * End x-kernel Sed hack\
\ */\

/	InP = &Mess.In;/c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	InP = (Request *)machMsgStart;\
\/* \
\ * End x-kernel Sed hack\
\ */


#mips
/^		bcopy((char \*) xmsg, (char \*) InP->xmsg, xmsgCnt);/c\
/* \
\ * Begin x-kernel Sed hack - Mips \
\ */\
\	\	if ( ((xk_and_mach_msg_t *)xmsg)->xkMsg ) {\
\	\	\	msg2buf( ((xk_and_mach_msg_t *)xmsg)->xkMsg, (char *)InP->xmsg);\
\	\	}\
\/* \
\ * End x-kernel Sed hack\
\ */

#alternate mig version
/^		memcpy(InP->xmsg, xmsg, xmsgCnt);/c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	\	if ( ((xk_and_mach_msg_t *)xmsg)->xkMsg ) {\
\	\	\	msg2buf( ((xk_and_mach_msg_t *)xmsg)->xkMsg, (char *)InP->xmsg);\
\	\	}\
\/* \
\ * End x-kernel Sed hack\
\ */


