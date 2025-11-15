/*-
 * Copyright (c) 1982, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 *	@(#)systm.h	8.4 (Berkeley) 2/23/94
 */

#ifndef _SYS_SYSTM_H_
#define _SYS_SYSTM_H_

#include <sys/cdefs.h>

/*
 * The `securelevel' variable controls the security level of the system.
 * It can only be decreased by process 1 (/sbin/init).
 *
 * Security levels are as follows:
 *   -1	permannently insecure mode - always run system in level 0 mode.
 *    0	insecure mode - immutable and append-only flags make be turned off.
 *	All devices may be read or written subject to permission modes.
 *    1	secure mode - immutable and append-only flags may not be changed;
 *	raw disks of mounted filesystems, /dev/mem, and /dev/kmem are
 *	read-only.
 *    2	highly secure mode - same as (1) plus raw disks are always
 *	read-only whether mounted or not. This level precludes tampering 
 *	with filesystems by unmounting them, but also inhibits running
 *	newfs while the system is secured.
 *
 * In normal operation, the system runs in level 0 mode while single user
 * and in level 1 mode while multiuser. If level 2 mode is desired while
 * running multiuser, it can be set in the multiuser startup script
 * (/etc/rc.local) using sysctl(1). If it is desired to run the system
 * in level 0 mode while multiuser, initialize the variable securelevel
 * in /sys/kern/kern_sysctl.c to -1. Note that it is NOT initialized to
 * zero as that would allow the vmunix binary to be patched to -1.
 * Without initialization, securelevel loads in the BSS area which only
 * comes into existence when the kernel is loaded and hence cannot be
 * patched by a stalking hacker.
 */
extern int securelevel;		/* system security level */
extern const char *panicstr;	/* panic message */
extern char version[];		/* system version */
extern char copyright[];	/* system copyright */

extern int nblkdev;		/* number of entries in bdevsw */
extern int nchrdev;		/* number of entries in cdevsw */
extern int nswdev;		/* number of swap devices */
extern int nswap;		/* size of swap space */

extern int selwait;		/* select timeout address */

extern u_char curpriority;	/* priority of current process */

extern int maxmem;		/* max memory per process */
extern int physmem;		/* physical memory */

extern dev_t dumpdev;		/* dump device */
extern long dumplo;		/* offset into dumpdev */

extern dev_t rootdev;		/* root device */
extern struct vnode *rootvp;	/* vnode equivalent to above */

extern dev_t swapdev;		/* swapping device */
extern struct vnode *swapdev_vp;/* vnode equivalent to above */

extern struct sysent {		/* system call table */
	int	sy_narg;	/* number of arguments */
	int	(*sy_call)();	/* implementing function */
} sysent[];

extern int boothowto;		/* reboot flags, from console subsystem */

/* casts to keep lint happy */
#define	insque(q,p)	_insque((caddr_t)q,(caddr_t)p)
#define	remque(q)	_remque((caddr_t)q)

/*
 * General function declarations.
 */
int	nullop __P((void));
int	enodev __P((void));
int	enoioctl __P((void));
int	enxio __P((void));
int	eopnotsupp __P((void));
int	seltrue __P((dev_t dev, int which, struct proc *p));
void	*hashinit __P((int count, int type, u_long *hashmask));

void	panic __P((const char *, ...));
void	tablefull __P((const char *));
void	addlog __P((const char *, ...));
void	log __P((int, const char *, ...));
void	printf __P((const char *, ...));
int	sprintf __P((char *buf, const char *, ...));
void	ttyprintf __P((struct tty *, const char *, ...));

void	bcopy __P((const void *from, void *to, size_t len));
void	ovbcopy __P((const void *from, void *to, u_int len));
void	bzero __P((void *buf, size_t len));

int	copystr __P((void *kfaddr, void *kdaddr, u_int len, u_int *done));
int	copyinstr __P((void *udaddr, void *kaddr, u_int len, u_int *done));
int	copyoutstr __P((void *kaddr, void *udaddr, u_int len, u_int *done));
int	copyin __P((void *udaddr, void *kaddr, u_int len));
int	copyout __P((void *kaddr, void *udaddr, u_int len));

int	fubyte __P((void *base));
#ifdef notdef
int	fuibyte __P((void *base));
#endif
int	subyte __P((void *base, int byte));
int	suibyte __P((void *base, int byte));
int	fuword __P((void *base));
int	fuiword __P((void *base));
int	suword __P((void *base, int word));
int	suiword __P((void *base, int word));

int	hzto __P((struct timeval *tv));
void	timeout __P((void (*func)(void *), void *arg, int ticks));
void	untimeout __P((void (*func)(void *), void *arg));
void	realitexpire __P((void *));

struct clockframe;
void	hardclock __P((struct clockframe *frame));
void	softclock __P((void));
void	statclock __P((struct clockframe *frame));

void	initclocks __P((void));

void	startprofclock __P((struct proc *));
void	stopprofclock __P((struct proc *));
void	setstatclockrate __P((int hzrate));

/*
 * Process management functions
 */
int	issig __P((void));
void	psig __P((void));
void	setpri __P((struct proc *));
int	grow __P((unsigned));
void	sleep __P((void *chan, int pri));
void	wakeup __P((void *chan));
void	wakeup_one __P((void *chan));
int	tsleep __P((void *chan, int pri, const char *wmesg, int timo));
void	unsleep __P((struct proc *));
void	setrun __P((struct proc *));
void	swtch __P((void));
void	cpu_switch __P((struct proc *));
int	procxmt __P((void));

/*
 * Signal handling functions
 */
void	savfp __P((void));
void	restfp __P((void));
void	psignal __P((struct proc *, int));
void	pgsignal __P((struct pgrp *, int, int));
int	issignal __P((struct proc *));
void	postsig __P((int));
void	siginit __P((struct proc *));
void	trapsignal __P((struct proc *, int, u_long));
void	sigexit __P((struct proc *, int));
int	sigprop __P((int));

/*
 * Memory management and address translation
 */
void	savu __P((label_t));
int	backup __P((int *));
void	setjmp __P((label_t));
void	longjmp __P((label_t));
int	savectx __P((void *));
void	loadctx __P((void *));

/*
 * Low-level trap and interrupt handling
 */
void	trap __P((void));
void	trap1 __P((int (*f)()));
void	syscall __P((void));
void	setrq __P((struct proc *));
void	remrq __P((struct proc *));

/*
 * File system and I/O functions
 */
void	ihinit __P((void));
void	bhinit __P((void));
void	binit __P((void));
void	iinit __P((void));
void	bflush __P((int));
void	bdwrite __P((struct buf *));
void	bawrite __P((struct buf *));
void	brelse __P((struct buf *));
void	bwrite __P((struct buf *));
void	biodone __P((struct buf *));
void	biowait __P((struct buf *));
int	bread __P((struct vnode *, daddr_t, int, struct ucred *, struct buf **));
int	breada __P((struct vnode *, daddr_t, int, daddr_t, int, struct ucred *, struct buf **));
struct buf *getblk __P((struct vnode *, daddr_t, int, int, int));
struct buf *geteblk __P((int));
struct buf *incore __P((struct vnode *, daddr_t));

/*
 * Scheduling and resource management
 */
void	schedcpu __P((void));
void	schedpaging __P((void));
void	roundrobin __P((void *));
void	schedclock __P((struct proc *));
void	setpriority __P((struct proc *, int));

/*
 * Device management
 */
int	nodev __P((void));
int	nulldev __P((void));
int	enodev __P((void));
int	nxio __P((void));
void	nxmove __P((void));

/*
 * Kernel initialization and system setup
 */
void	configure __P((void));
void	cpu_startup __P((void));
void	consinit __P((void));
void	swapinit __P((void));
void	cninit __P((void));

/*
 * Utility functions
 */
void	_insque __P((void *, void *));
void	_remque __P((void *));
int	scanc __P((u_int, u_char *, u_char *, int));
int	skpc __P((int, int, char *));
int	locc __P((int, char *, u_int));
int	ffs __P((int));
void	microtime __P((struct timeval *));
void	get_time __P((struct timeval *));
void	set_time __P((struct timeval *));
int	imax __P((int, int));
int	imin __P((int, int));
long	lmax __P((long, long));
long	lmin __P((long, long));
u_int	max __P((u_int, u_int));
u_int	min __P((u_int, u_int));
int	abs __P((int));
long	labs __P((long));

/*
 * Random number generation
 */
void	srandom __P((u_long));
long	random __P((void));

/*
 * Quota management
 */
void	dqinit __P((void));

/*
 * Network protocol functions
 */
void	pfctlinput __P((int, struct sockaddr *));
void	pfslowtimo __P((void));
void	pffasttimo __P((void));

/*
 * VM and paging functions
 */
void	vmmeter __P((void));
void	vmtotal __P((struct vmtotal *));
void	vmsched __P((void));

#include <libkern/libkern.h>

#endif /* !_SYS_SYSTM_H_ */
