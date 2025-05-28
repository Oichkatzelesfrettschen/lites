#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.8 $
# $Date: 1993/11/11 21:43:45 $
#


DIRS =
FILES = xk_lproxyS xk_lproxyU xk_uproxyS xk_uproxyU
LIBS = 

WARNINGS :=
TMP_INCLUDES := -I..

ifeq ($(MIG),)
MIG = mig
endif

include $(MAKE_INCLUDE)/Makefile.common

MIG_GEN_SRC = xk_lproxyS.c xk_lproxyU.c xk_lproxy.h xk_uproxyS.c xk_uproxyU.c xk_uproxy.h


#
# Both of the mig-generated User files require some postprocessing by
# sed scripts
#
xk_lproxyS.c xk_lproxyU.c xk_lproxy.h: ../xk_lproxy.defs ../lproxy.awk ../lproxy.demux.sed
	$(MIG) -server xk_lproxyS.c.tmp -user xk_lproxyU.c.tmp $<
	awk -f ../lproxy.awk xk_lproxyU.c.tmp
	mv lproxy.rest.tmp xk_lproxyU.c
	sed -f ../lproxy.demux.sed lproxy.demux.tmp >> xk_lproxyU.c
	sed -f ../lproxyServer.sed xk_lproxyS.c.tmp > xk_lproxyS.c

xk_uproxyS.c xk_uproxyU.c xk_uproxy.h: ../xk_uproxy.defs ../uproxy.awk ../uproxy.rest.sed ../uproxy.call.sed
	$(MIG) -server xk_uproxyS.c -user xk_uproxyU.c.tmp $<
	awk -f ../uproxy.awk xk_uproxyU.c.tmp
	sed -f ../uproxy.rest.sed uproxy.rest.tmp > xk_uproxyU.c
	sed -f ../uproxy.call.sed uproxy.call.tmp >> xk_uproxyU.c
	sed -f ../uproxy.push.sed uproxy.push.tmp >> xk_uproxyU.c
	rm uproxy.call.tmp uproxy.push.tmp uproxy.rest.tmp xk_uproxyU.c.tmp 

