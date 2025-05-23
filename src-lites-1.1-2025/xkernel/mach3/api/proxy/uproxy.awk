#     
# $RCSfile: uproxy.awk,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
#
# $Revision: 1.2 $
# $Date: 1993/04/05 00:58:44 $
#

# Pick off certain MIG-generate routines and write them to separate
# files, writing the rest of the source to a different file.  

call != 0 && /\/\* Routine/	{ call = 0 }
push != 0 && /\/\* Routine/	{ push = 0 }
/Routine xCall/		{ call = 1 }
/Routine xPush/		{ push = 1 }
call != 0 { print $0 >"uproxy.call.tmp"; next }
push != 0 { print $0 >"uproxy.push.tmp"; next }

{ print $0 >"uproxy.rest.tmp" }
