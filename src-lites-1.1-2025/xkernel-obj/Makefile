#
# $RCSfile: Makefile.mach3-intelx86,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.7.1.5 $
# $Date: 1993/11/30 19:32:55 $
#
#
# Build-directory Makefile for the mach3/intelx86 platform.
#
# There are many variables defined here that may vary from site to
# site.  If you are the first person at your site to build  an
# x-kernel, look over the definitions carefully and determine what the
# appropriate values should be for your site. 


XPLATFORM = mach4
MACHINE = intelx86

#
# HOWTOCOMPILE controls optimization flags and tracing routines.  
# It should be set to either DEBUG or OPTIMIZE
#
HOWTOCOMPILE = DEBUG

#
# If XK_BYPASS_RCS is defined (to anything), RCS consistency checks
# will be disabled.  This can make a build run significantly faster.
#
XK_BYPASS_RCS = blurp


#
# Compiler-related flags.  If you decide to change compilers, some of
# these flags may also have to change.
#
CC = i386-mach-gcc
DEBUGWARNINGS = 
OPTIMIZEWARNINGS = 
WARNINGS = $($(HOWTOCOMPILE)WARNINGS)
CC_SPECIFIC_FLAGS = -nostdinc -fwritable-strings -fsigned-char
OPTIMIZECFLAGS = -O
DEBUGCFLAGS = -g 
CC_LIBPATH = -L/usr/local/lib/gcc-lib/i386-mach/2.6.2
CC_INCLUDES = -I/usr/local/lib/gcc-lib/i386-mach/2.6.2/include
CCLIBS = -lgcc
# /usr/local/lib/gcc-lib/i386-mach/2.6.2/libgcc.a
#
# GCC Warnings we aren't using yet:
#
# -Wconversion -- nails us on conversion of enumerated types in a most
# 		  annoying fashion
#
# -Wformat -- Gripes about using an uncast pointer as a '%x' arg to
# 	      printf which we do all the time
# -Wparentheses -- this is lame


#
# Libraries.  Some of these libraries are labeled 'optional',
# indicating that they are only used by some protocols.  Names and
# locations of libraries may be site-specific.
#
LIBPATH = -L/usr/mach4/lib $(CC_LIBPATH) 
mach4_LIBS = -lthreads -lmach -lmach_sa	# required	# not mach_sa?
LIB_NETNAME = #-lnetname		# optional
LIB_CMUCS = #-lcmucs		# optional
LIB_SERVICE = #-lservice		# optional

MIG_INCLUDES = -I/usr/mach4/include 

SED = sed
MAKE = gmake
RANLIB = i386-mach-ranlib
AR = i386-mach-ar
MIG = mig 
LD = i386-mach-ld

#
# XRT names a path to the top of the x-kernel source tree
#
XRT = ../xkernel

HOW = $(HOWTOCOMPILE)$(XPLATFORM)-$(MACHINE)

#
# TMP_CPPFLAGS and TMP_CFLAGS apply to the build directory only, and
# won't be propagated to compilation in other x-kernel directories. 
#
# TMP_CPPFLAGS = 
# TMP_CFLAGS = 

#
# When compiling private versions of x-kernel utilities in your build
# area, you may need to add include paths.  Add these to the
# TMP_INCLUDES variable.
#
TMP_INCLUDES = $(COMPOSE_INCLUDES)

#
# We rebuild some source code from the user library so we need a path
# to some kernel include directories.
#
MACH_KERNEL_INCLUDES = -I/usr/mach4
SYS_INCLUDES =  -I/usr/mach4/include/mach/sa -I/usr/mach4/include -I/usr/mach4/include/mach \
 -I/usr/mach4/include/mach/mach_debug \
	-I/usr/local/lib/gcc-lib/i386-mach/2.6.2/include \
	$(MACH_KERNEL_INCLUDES) \
		$(CC_INCLUDES) -I/usr/src/mach/xkernel/oddlibs


MAKEFILE=Makefile
MAKEFLAGS += --no-print-directory

OPTIMIZECPPFLAGS = 
DEBUGCPPFLAGS = -DXK_DEBUG
BUILD_CPPFLAGS = $($(HOWTOCOMPILE)CPPFLAGS) -DMACH -DXKMACH4 -Dintelx86 -DMACH_MKREV=82
BUILD_CFLAGS = $($(HOWTOCOMPILE)CFLAGS) 
LDFLAGS = $(LIBPATH)

#
# Invoking gcc in the final load grabs the wrong crt0.o, so we
# explicitly invoke ld and name the correct crt0.o
#
CRT0 = /usr/mach4/lib/mach-crt0.o


all:	xkernel

#
# The COMPOSE_MAKEFILE is generated from graph.comp when 'make compose'
# is run. 
#
COMPOSE_MAKEFILE = Makefile.local
COMPOSEFLAGS = -f -m $(COMPOSE_MAKEFILE)
include $(COMPOSE_MAKEFILE)

$(COMPOSE_MAKEFILE):
	@touch $(COMPOSE_MAKEFILE)

#-----------------------------------------------------------------------
#
# If you compile source code in your build area which isn't associated
# with a particular protocol, add the source names to PRIVSRC and the
# object names (e.g., ./$(HOW)/foo.o) to PRIVOBJ. 
#
PRIVSRC = cerror.S bcmp.c insque.c remque.c qsort.c ptbl_static.c
#
PRIVOBJ = cerror.o bcmp.o insque.o remque.o qsort.o ptbl_static.o


include $(XRT)/build/Template/Makefile.common

#
# Include Makefile.nolibs to compile without libraries
#
include $(XRT)/build/Template/Makefile.nolibs


libSocket:
	cd $(XRT)/$(XPLATFORM)/socket/lib; $(MAKE)


ifeq (,$(XLIBS))
LDLIBS := $(XLIBS) $($(XPLATFORM)_LIBS)
else
LDLIBS := $(XLIBS) $($(XPLATFORM)_LIBS) $(XLIBS) $($(XPLATFORM)_LIBS)
endif


xkernel:	$(OBJ) $(XLIBS)
	@echo linking xkernel ...
	@$(LD) -o xkernel $(LDFLAGS) \
		$(CRT0) \
		$(OBJ) \
		$(LDLIBS) $(COMPOSE_LIBS) \
		$(CCLIBS) \
		#-lc

include Makedep.$(HOW)

#To be removed
bcmp.o:	bcmp.c
	$(CC) -c $(CFLAGS) bcmp.c

insque.o:	insque.c
	$(CC) -c $(CFLAGS) insque.c

remque.o:	remque.c
	$(CC) -c $(CFLAGS) remque.c

qsort.o:	qsort.c
	$(CC) -c $(CFLAGS) qsort.c

ptbl_static.o:	ptbl_static.c
	$(CC) -c $(CFLAGS) ptbl_static.c

cerror.o: cerror.S
	$(CC) -c $(ASFLAGS) -MD $<


