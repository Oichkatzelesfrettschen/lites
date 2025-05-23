#
# protocols Makefile
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.52.1.3 $
# $Date: 1993/10/29 16:46:58 $
#


FILES = 
DIRS = $($(XPLATFORM)_PROTDIRS) arp bid bidctl blast chan \
 	eth icmp ip join rat select simsimeth \
 	tcp test udp vcache vchan vdisorder \
 	vdrop vmux vnet vsize vtap

mach3_PROTDIRS = machnetipc

XRT=..
# if necessary for your compiler, add .a extension for libProt here
LIB=libProt_$(HOW)

OBJECTFILE = newObjectList

include $(MAKE_INCLUDE)/Makefile.lib



