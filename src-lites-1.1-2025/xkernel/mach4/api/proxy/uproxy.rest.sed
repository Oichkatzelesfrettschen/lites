#
# $RCSfile: uproxy.rest.sed,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
#
# $Revision: 1.1 $
# $Date: 1993/02/18 03:07:52 $
#

#
# Should only replace something in the 'xPush' routine
#

/^		bcopy((char \*) xmsg, (char \*) InP->xmsg, xmsgCnt);/c\
\	\	msg2buf((Msg *)xmsg, (char *) InP->xmsg);
