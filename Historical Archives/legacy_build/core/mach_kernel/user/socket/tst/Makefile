#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.10 $
# $Date: 1993/11/15 04:55:58 $
#

CFLAGS := -g -O
XSILIBS	= -L.. -L/usr/mach/lib -lxksocket_$(HOW) -lnetname -lthreads
SYSLIBS	= -lcmucs -lmach

PGMS = xk-rtrip xk-truput mach-rtrip mach-truput


default: $(addprefix $(HOW)/, $(PGMS))


$(HOW)/mach-%: %.c
	@-if [ ! -d $(HOW) ] ; then mkdir $(HOW); else true; fi ;
	$(CC) $(CFLAGS) -o $@ $^ $(SYSLIBS)

$(HOW)/xk-%: %.c ../libxksocket_$(HOW).a
	@-if [ ! -d $(HOW) ] ; then mkdir $(HOW); else true; fi ; 
	$(CC) $(CFLAGS) -o $@ $< $(XSILIBS) $(SYSLIBS)

