# 
# Mach Operating System
# Copyright (c) 1994 Johannes Helander
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# JOHANNES HELANDER ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  JOHANNES HELANDER DISCLAIMS ANY LIABILITY OF ANY KIND
# FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
#
#
# HISTORY
# $Log: $
#
#	File:	include/ufs/Makefile
#	Author:	Johannes Helander, Helsinki University of Technology, 1994.
#	Date:	1994
#

IDIR		= /include/ufs/ufs/

VPATH		= ${MAKETOP}server/ufs/ufs ${MAKETOP}server/ufs/ffs ${MAKETOP}server/ufs/lfs ${MAKETOP}server/ufs/mfs

INCLUDES = ffs_extern.h dinode.h fs.h dir.h lfs.h inode.h lfs_extern.h \
	lockf.h mfs_extern.h quota.h mfsiom.h ufs_extern.h mfsnode.h \
	ufsmount.h

ILIST		= ${INCLUDES}

.include <${RULES_MK}>
