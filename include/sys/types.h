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

/* Basic unsigned types - always defined for BSD kernel */
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* Sys V compatibility */
typedef	unsigned int	uint;		/* Sys V compatibility */

/* Quad types for large integers */
typedef	unsigned long long u_quad_t;	/* quads */
typedef	long long	quad_t;
typedef	quad_t *	qaddr_t;

/* setjmp/longjmp label type - BSD compatibility */
typedef int		label_t[10];	/* setjmp buffer */

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

/* Guard against system header conflicts */
#ifndef _PID_T_DEFINED_
#define _PID_T_DEFINED_
typedef	long		pid_t;		/* process id */
#endif

typedef	long		segsz_t;	/* segment size */
typedef	long		swblk_t;	/* swap offset */

#ifndef _UID_T_DEFINED_
#define _UID_T_DEFINED_
typedef	unsigned long	uid_t;		/* user id */
#endif

/* Time types - guard against system header conflicts */
#ifndef _CLOCK_T_DEFINED_
#define _CLOCK_T_DEFINED_
typedef	long		clock_t;	/* clock ticks */
#endif

#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef	unsigned long	size_t;		/* size of objects */
#endif

#ifndef _SSIZE_T_DEFINED_
#define _SSIZE_T_DEFINED_
typedef	long		ssize_t;	/* signed size */
#endif

#ifndef _TIME_T_DEFINED_
#define _TIME_T_DEFINED_
typedef	long		time_t;		/* time in seconds */
#endif

/*
 * Modern integer types for KERNEL compatibility
 * These types are needed by modern code that expects stdint.h types
 */
#ifdef KERNEL

/* Fixed-width unsigned integer types */
#ifndef _UINT8_T_DEFINED_
#define _UINT8_T_DEFINED_
typedef unsigned char       uint8_t;
typedef unsigned char       u_int8_t;
#endif

#ifndef _UINT16_T_DEFINED_
#define _UINT16_T_DEFINED_
typedef unsigned short      uint16_t;
typedef unsigned short      u_int16_t;
#endif

#ifndef _UINT32_T_DEFINED_
#define _UINT32_T_DEFINED_
typedef unsigned int        uint32_t;
typedef unsigned int        u_int32_t;
#endif

#ifndef _UINT64_T_DEFINED_
#define _UINT64_T_DEFINED_
typedef unsigned long long  uint64_t;
typedef unsigned long long  u_int64_t;
#endif

/* Fixed-width signed integer types */
#ifndef _INT8_T_DEFINED_
#define _INT8_T_DEFINED_
typedef signed char         int8_t;
#endif

#ifndef _INT16_T_DEFINED_
#define _INT16_T_DEFINED_
typedef signed short        int16_t;
#endif

#ifndef _INT32_T_DEFINED_
#define _INT32_T_DEFINED_
typedef signed int          int32_t;
#endif

#ifndef _INT64_T_DEFINED_
#define _INT64_T_DEFINED_
typedef signed long long    int64_t;
#endif

/* Linux kernel compatibility types (little-endian) */
#ifndef _LE_TYPES_DEFINED_
#define _LE_TYPES_DEFINED_
typedef unsigned short      __le16;
typedef unsigned int        __le32;
typedef unsigned long long  __le64;
typedef signed short        __s16;
typedef signed int          __s32;
typedef signed long long    __s64;
typedef unsigned short      __u16;
typedef unsigned int        __u32;
typedef unsigned long long  __u64;
#endif

/* Big-endian types for completeness */
#ifndef _BE_TYPES_DEFINED_
#define _BE_TYPES_DEFINED_
typedef unsigned short      __be16;
typedef unsigned int        __be32;
typedef unsigned long long  __be64;
#endif

#endif /* KERNEL */

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
