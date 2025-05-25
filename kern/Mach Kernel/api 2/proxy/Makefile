#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.6 $
# $Date: 1993/11/11 21:37:25 $
#

DIRS = gen
FILES = lproxy uproxy proxy_util proxy_util_ook xk_mms
LIBS = LIB_NETNAME

ifeq ($(HOWTOCOMPILE),OPTIMIZE)
#
# We use BUILD_CPPFLAGS instead of TMP_CPPFLAGS to propagate
# these flags into subdirectories
#
BUILD_CPPFLAGS += -DTypeCheck=0 -DXK_PROXY_MSG_HACK

endif


TMP_INCLUDES := -Igen -I$(XRT)/pi

include $(MAKE_INCLUDE)/Makefile.common

#
# localdepend (and the other loop rules) are run with preorder
# traversal.  This rule forces generation of a header file in a
# subdirectory first.  Postorder would be nice ...
#

localdepend: gen/xk_lproxy.h gen/xk_uproxy.h

gen/xk_lproxy.h: xk_lproxy.defs
	cd gen; make XRT:=../$(XRT) xk_lproxy.h

gen/xk_uproxy.h: xk_uproxy.defs
	cd gen; make XRT:=../$(XRT) xk_uproxy.h

