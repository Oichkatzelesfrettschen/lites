# 
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
# Copyright (c) 1994 Johannes Helander
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON AND JOHANNES HELANDER ALLOW FREE USE OF THIS
# SOFTWARE IN ITS "AS IS" CONDITION.  CARNEGIE MELLON AND JOHANNES
# HELANDER DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
# WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
#
# HISTORY
# $Log$
# $EndLog$
#
#	File:	 include/ns532/Makefile
#	Authors: Mary Thompson, Johannes Helander
#	Date:	 1992, 1994
#
# This Makefile copies the header files from the bsdss source
# ns532 directory to the export directory where they can be found
# in the directory "machine". This eliminates the need for 
# machine links in the sources.

DATAFILES		= endian.h limits.h signal.h param.h stdarg.h \
			  ansi.h types.h cpu.h ptrace.h vmparam.h exec.h

INCLUDES		= ${DATAFILES}

EXPDIR			= /include/ns532/

.include <${RULES_MK}>
