#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.5 $
# $Date: 1993/11/18 03:51:41 $
#


DIRS =
FILES = xsi_server
TMP_INCLUDES = -I..

include $(MAKE_INCLUDE)/Makefile.common

xsi_user.h xsi_user.c xsi_server.c: ../xsi.defs
	$(MIG) $(CPPFLAGS) -server xsi_server.c -user xsi_user.c -header xsi_user.h $^

