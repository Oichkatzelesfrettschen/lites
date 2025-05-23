#     
# $RCSfile: lproxy.awk,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
#
# $Revision: 1.1 $
# $Date: 1993/02/18 03:06:28 $
#

# Pick off certain MIG-generate routines and write them to separate
# files, writing the rest of the source to a different file.  

demux != 0 && /\/\* Routine/	{ demux = 0 }
/Routine xDemux/		{ demux = 1 }
/Routine xCallDemux/		{ demux = 1 }
demux != 0 { print $0 >"lproxy.demux.tmp"; next }

{ print $0 >"lproxy.rest.tmp" }
