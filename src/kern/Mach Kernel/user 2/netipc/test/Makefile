#
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1991  Arizona Board of Regents
#
# $Revision: 1.6 $
# $Date: 1993/12/01 00:19:04 $
#

INCLUDE = -I. -I$(XRT)/mach3/api/netipc -I/usr/mach/include -I/usr/include
OBJS = nnstest.o
TARGETS = nnstest

all: $(TARGETS)

nnstest:  nnstest.o
	$(CC) -o nnstest -L/usr/mach/lib nnstest.o -lnetname -lthreads -lmach 

nnstest.o: nnstest.c
	$(CC) -c -g $(INCLUDE) -o nnstest.o nnstest.c

clean:	
	rm -f $(TARGETS) $(OBJS)

depend:	
	@$(CC) -MM $(INCLUDE) *.c  > Makedep

Makedep:
	touch $@

include Makedep
