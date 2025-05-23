#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.4 $
# $Date: 1993/11/15 04:54:09 $
#

DIRS = netipc 

ifeq ($(MACHINE),mips)

DIRS += socket

endif


XRT=../..
LIB=libPxk_$(HOW)

OBJECTFILE = newObjectList

include $(MAKE_INCLUDE)/Makefile.lib
