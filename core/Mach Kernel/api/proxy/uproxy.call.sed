#     
# $RCSfile: uproxy.call.sed,v $
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
# Sed Hacks to modify the xCall routine of the 
# xk_uproxyUser MIG output 
#


/^	union {/,/^	register Reply/	c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	typedef union {\
\		Request In;\
\		Reply Out;\
\	} union_mess;\
\
\	union_mess	*Mess = (union_mess *)repmsg;\
\	Msg		*xReqMsg = (Msg *)reqmsg;\
\	register Request *InP = &Mess->In;\
\	register Reply 	*OutP = &Mess->Out;\
\
\/* \
\ * End x-kernel Sed hack\
\ */



#mips 
/^		bcopy((char \*) reqmsg, (char \*) InP->reqmsg, reqmsgCnt);/	c\
/* \
\ * Begin x-kernel Sed hack - Mips \
\ */\
\		if ( xReqMsg ) {\
\	    		msg2buf(xReqMsg, (char *)InP->reqmsg);\
\		}\
\/* \
\ * End x-kernel Sed hack\
\ */


#alternate mig version
/^		memcpy(InP->reqmsg, reqmsg, reqmsgCnt);/	c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\		if ( xReqMsg ) {\
\	    		msg2buf(xReqMsg, (char *)InP->reqmsg);\
\		}\
\/* \
\ * End x-kernel Sed hack\
\ */


#mips
/		bcopy((char \*) OutP->repmsg, (char \*) repmsg, \*repmsgCnt)/,/msgt_number);/	c\
/* \
\ * Begin x-kernel Sed hack - Mips \
\ */\
\		return MIG_ARRAY_TOO_LARGE;\
\/* \
\ * End x-kernel Sed hack\
\ */


#alternate mig version
/		memcpy(repmsg, OutP->repmsg, \*repmsgCnt);/c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\		return MIG_ARRAY_TOO_LARGE;\
\/* \
\ * End x-kernel Sed hack\
\ */


#alternate mig version
/		memcpy(repmsg, OutP->repmsg, OutP->repmsgType.msgt_number);/	c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\/*		memcpy(repmsg, OutP->repmsg, OutP->repmsgType.msgt_number);*/\
\/* \
\ * End x-kernel Sed hack\
\ */



/	return KERN_SUCCESS;/i\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	xAssert( offsetof(Reply,repmsg) == XK_CALL_REP_OFFSET );\
\/* \
\ * End x-kernel Sed hack\
\ */


/	InP = &Mess.In;/c\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	InP = &Mess->In;\
\/* \
\ * End x-kernel Sed hack\
\ */


