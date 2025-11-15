/*
 * Copyright (c) 1990, 1993
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
 *	@(#)kernel.h	8.3 (Berkeley) 1/21/94
 */

#ifndef _SYS_KERNEL_H_
#define _SYS_KERNEL_H_

#include <sys/time.h>

/* Global variables used by kernel */

#ifdef KERNEL

/*
 * Clock handling
 */

/* Ticks since boot */
extern int lbolt;           /* awoken once a second */
extern int ticks;           /* system ticks */

/* Clock rate */
extern int hz;              /* clock frequency */
extern int tick;            /* usec per tick (1000000 / hz) */
extern int tickadj;         /* "standard" clock skew, us/tick */
extern int stathz;          /* statistics clock frequency */
extern int profhz;          /* profiling clock frequency */

/*
 * Time of day
 */
extern struct timeval boottime;     /* time system was booted */
extern struct timeval runtime;      /* time since last boot */
extern volatile struct timeval time;    /* current time (legacy) */
extern struct timeval mono_time;    /* monotonic time */

/*
 * Kernel initialization phases
 */
enum sysinit_elem_order {
    SI_ORDER_FIRST = 0x0000000,     /* first */
    SI_ORDER_SECOND = 0x0000001,    /* second */
    SI_ORDER_THIRD = 0x0000002,     /* third */
    SI_ORDER_FOURTH = 0x0000003,    /* fourth */
    SI_ORDER_MIDDLE = 0x1000000,    /* somewhere in the middle */
    SI_ORDER_ANY = 0xfffffff        /* last */
};

/*
 * System initialization subsystems
 */
enum sysinit_sub_id {
    SI_SUB_DUMMY = 0x0000000,       /* not executed; for linker*/
    SI_SUB_CONSOLE = 0x0800000,     /* console */
    SI_SUB_COPYRIGHT = 0x0800001,   /* first use of console*/
    SI_SUB_VM = 0x1000000,          /* virtual memory */
    SI_SUB_KMEM = 0x1800000,        /* kernel memory */
    SI_SUB_CPU = 0x2000000,         /* CPU resource(s) */
    SI_SUB_DEVFS = 0x2200000,       /* get DEVFS ready */
    SI_SUB_DRIVERS = 0x3100000,     /* Let drivers initialize */
    SI_SUB_CONFIGURE = 0x3800000,   /* Configure devices */
    SI_SUB_VFS = 0x4000000,         /* virtual filesystem */
    SI_SUB_CLOCKS = 0x4800000,      /* real time and stat clocks */
    SI_SUB_MBUF = 0x5000000,        /* mbufs */
    SI_SUB_CLIST = 0x5800000,       /* clists */
    SI_SUB_SYSV_SHM = 0x6400000,    /* System V shared memory */
    SI_SUB_SYSV_SEM = 0x6800000,    /* System V semaphores */
    SI_SUB_SYSV_MSG = 0x6C00000,    /* System V message queues */
    SI_SUB_PSEUDO = 0x7000000,      /* pseudo devices */
    SI_SUB_EXEC = 0x7400000,        /* execve() handlers */
    SI_SUB_PROTO_BEGIN = 0x8000000, /* XXX: set SYSINIT_SUB_ORDER */
    SI_SUB_PROTO_IF = 0x8400000,    /* interfaces */
    SI_SUB_PROTO_DOMAIN = 0x8800000,    /* domains (address families?) */
    SI_SUB_PROTO_END = 0x8ffffff,   /* XXX: set SYSINIT_SUB_ORDER */
    SI_SUB_KPROF = 0x9000000,       /* kernel profiling */
    SI_SUB_KICK_SCHEDULER = 0xa000000,  /* start the timeout events */
    SI_SUB_ROOT = 0xb000000,        /* root mount */
    SI_SUB_ROOTCONF = 0xc000000,    /* find root devices */
    SI_SUB_DUMP = 0xd000000,        /* dump routines */
    SI_SUB_MOUNT = 0xe000000,       /* mount root fs */
    SI_SUB_RUN_SCHEDULER = 0xfffffff    /* scheduler: no return */
};

/*
 * Kernel timeout/untimeout interface
 */
struct callout {
    struct callout *c_next;     /* next callout in queue */
    void *c_arg;                /* function argument */
    void (*c_func)(void *);     /* function to call */
    int c_time;                 /* ticks to the event */
};

/*
 * Callout list head
 */
struct callout_list {
    struct callout *ch_head;
};

/* Timeout/untimeout function prototypes */
typedef void timeout_t(void *);

void timeout(timeout_t *func, void *arg, int ticks);
void untimeout(timeout_t *func, void *arg);

/*
 * Boot flags passed from the boot program
 */
#define RB_ASKNAME  0x001   /* ask for file name to reboot from */
#define RB_SINGLE   0x002   /* reboot to single user only */
#define RB_NOSYNC   0x004   /* dont sync before reboot */
#define RB_HALT     0x008   /* don't reboot, just halt */
#define RB_INITNAME 0x010   /* name given for /etc/init (unused) */
#define RB_DFLTROOT 0x020   /* use compiled-in rootdev */
#define RB_KDB      0x040   /* give control to kernel debugger */
#define RB_RDONLY   0x080   /* mount root fs read-only */
#define RB_DUMP     0x100   /* dump kernel memory before reboot */
#define RB_MINIROOT 0x200   /* mini-root present in memory at boot time */
#define RB_CONFIG   0x400   /* invoke user configuration */
#define RB_VERBOSE  0x800   /* print all potentially useful info */
#define RB_SERIAL   0x1000  /* use serial port as console */
#define RB_CDROM    0x2000  /* use cdrom as root */
#define RB_POWEROFF 0x4000  /* turn power off (or at least halt) */
#define RB_GDB      0x8000  /* use GDB remote debugger instead of DDB */
#define RB_MUTE     0x10000 /* start up with the console muted */
#define RB_PAUSE    0x100000    /* pause after each output line */

extern int boothowto;       /* reboot flags, from boot loader */
extern char *rootdevnames[];    /* names of root devices */

/*
 * Macro to declare a kernel initialization routine
 */
#define SYSINIT(uniquifier, subsystem, order, func, ident) \
    static void uniquifier(void *dummy) { func(ident); }

#endif /* KERNEL */

#endif /* !_SYS_KERNEL_H_ */
