#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Log: Makefile,v $
# Revision 1.14  1993/11/30  19:29:35  menze
# Added LIB_CMUCS and LIB_SERVICE
#
#

DIRS = 
FILES = machmsg_int mach-xfer machnetipc port-maint hdr-utils nns nns_boot \
	nns_procs ssr
LIBS = LIB_NETNAME LIB_CMUCS LIB_SERVICE

WARNINGS =

TMP_CPPFLAGS := -DPORTLOCKS -DMAKESENDCOUNT -DNOMORESENDERS -DPORTBLOCK

include $(MAKE_INCLUDE)/Makefile.common
