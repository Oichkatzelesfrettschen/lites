#
# $RCSfile: lproxyServer.sed,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
#
# $Revision: 1.3 $
# $Date: 1993/04/08 17:27:39 $
#

#
# Sed hacks for the xk_lproxyServer MIG output. 
#

#
# This is a multi-line sed pattern
#

/(*routine) (InP, &OutP->Head);/N
/(*routine) (InP, &OutP->Head);\n	return TRUE;/c\
\	(*routine) (InP, &OutP->Head);\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\#ifdef XK_PROXY_MSG_HACK \
\	if ( routine != _XxDemux && routine != _XxCallDemux ) {\
\	\    xFree((char *)InP);\
\	}\
\#endif\
\/* \
\ * End x-kernel Sed hack\
\ */\
\	return TRUE;



/RetCode = do_lproxy_xDemux/i\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	In1P->msgStart = (int)InHeadP;\
\/* \
\ * End x-kernel Sed hack\
\ */\


/RetCode = do_lproxy_xCallDemux/i\
/* \
\ * Begin x-kernel Sed hack\
\ */\
\	In1P->reqMsgStart = (int)InHeadP;\
\/* \
\ * End x-kernel Sed hack\
\ */\


