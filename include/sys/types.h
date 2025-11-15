/*-
 * Copyright (c) 1982, 1986, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.1 (Berkeley) 6/2/93
 *	Adapted for Lites 1.1.u3 on CMU Mach 3.0
 */

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#include <sys/cdefs.h>

/* Basic unsigned types */
#ifndef _POSIX_SOURCE
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* Sys V compatibility */
typedef	unsigned int	uint;		/* Sys V compatibility */
#endif

/* Quad types for large integers */
typedef	unsigned long long u_quad_t;	/* quads */
typedef	long long	quad_t;
typedef	quad_t *	qaddr_t;

/* Address and system types */
typedef	char *		caddr_t;	/* core address */
typedef	long		daddr_t;	/* disk address */
typedef	unsigned long	dev_t;		/* device number */
typedef unsigned long	fixpt_t;	/* fixed point number */
typedef	unsigned long	gid_t;		/* group id */
typedef	unsigned long	ino_t;		/* inode number */
typedef	unsigned short	mode_t;		/* permissions */
typedef	unsigned short	nlink_t;	/* link count */
typedef	quad_t		off_t;		/* file offset */
typedef	long		pid_t;		/* process id */
typedef	long		segsz_t;	/* segment size */
typedef	long		swblk_t;	/* swap offset */
typedef	unsigned long	uid_t;		/* user id */

/* Time types */
typedef	long		clock_t;	/* clock ticks */
typedef	unsigned long	size_t;		/* size of objects */
typedef	long		ssize_t;	/* signed size */
typedef	long		time_t;		/* time in seconds */

#ifndef _POSIX_SOURCE
/* Bit manipulation for device numbers */
#define	major(x)	((int)(((u_int)(x) >> 8)&0xff))	/* major number */
#define	minor(x)	((int)((x)&0xff))		/* minor number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))	/* create dev_t */

/* Bit operations */
#define	NBBY	8		/* number of bits in a byte */

/*
 * Forward structure declarations for function prototypes.
 * These are common structures that cross subsystem boundaries.
 */
#if defined(__STDC__) && defined(KERNEL)
struct	proc;
struct	pgrp;
struct	ucred;
struct	rusage;
struct	file;
struct	buf;
struct	tty;
struct	uio;
#endif

#endif /* !_POSIX_SOURCE */

#endif /* !_SYS_TYPES_H_ */
