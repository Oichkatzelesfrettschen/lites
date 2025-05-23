/*-
 * Copyright (c) 1982, 1986, 1991, 1993
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
 *	@(#)tty_conf.c	8.4 (Berkeley) 1/21/94
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/ioctl.h>
#include <sys/proc.h>
#include <sys/tty.h>
#include <sys/conf.h>

#define	ttynodisc ((int (*) (dev_t, struct tty *))enodev)
#define	ttyerrclose ((int (*) (struct tty *, int flags))enodev)
#define	ttyerrio ((int (*) (struct tty *, struct uio *, int))enodev)
#define	ttyerrinput ((int (*) (int c, struct tty *))enodev)
#define	ttyerrstart ((int (*) (struct tty *))enodev)

int	nullioctl (struct tty *tp, int cmd, caddr_t data,
			int flag, struct proc *p);

#include "tb.h"
#if NTB > 0
int	tbopen (dev_t dev, struct tty *tp);
int	tbclose (struct tty *tp, int flags);
int	tbread (struct tty *, struct uio *, int flags);
int	tbioctl (struct tty *tp, int cmd, caddr_t data,
			int flag, struct proc *p);
int	tbinput (int c, struct tty *tp);
#endif

#include "sl.h"
#if NSL > 0
int	slopen (dev_t dev, struct tty *tp);
int	slclose (struct tty *tp, int flags);
int	sltioctl (struct tty *tp, int cmd, caddr_t data,
			int flag, struct proc *p);
int	slinput (int c, struct tty *tp);
int	slstart (struct tty *tp);
#endif

#include "ppp.h"
#if NPPP > 0
int	pppopen (dev_t dev, struct tty *tp);
int	pppclose (struct tty *tp, int flags);
int	pppread (struct tty *tp, struct uio *uio, int flags);
int	pppwrite (struct tty *tp, struct uio *uio, int flags);
int	ppptioctl (struct tty *tp, int cmd, caddr_t data,
		       	int flag, struct proc *p);
int	pppinput (int c, struct tty *tp);
int	pppstart (struct tty *tp);
#endif

struct	linesw linesw[] =
{
	{ ttyopen, ttylclose, ttread, ttwrite, nullioctl,
	  ttyinput, ttstart, ttymodem },		/* 0- termios */

	{ ttynodisc, ttyerrclose, ttyerrio, ttyerrio, nullioctl,
	  ttyerrinput, ttyerrstart, nullmodem },	/* 1- defunct */

	{ ttynodisc, ttyerrclose, ttyerrio, ttyerrio, nullioctl,
	  ttyerrinput, ttyerrstart, nullmodem },	/* 2- defunct */

#if NTB > 0
	{ tbopen, tbclose, tbread, enodev, tbioctl,
	  tbinput, ttstart, nullmodem },		/* 3- TABLDISC */
#else
	{ ttynodisc, ttyerrclose, ttyerrio, ttyerrio, nullioctl,
	  ttyerrinput, ttyerrstart, nullmodem },
#endif

#if NSL > 0
	{ slopen, slclose, ttyerrio, ttyerrio, sltioctl,
	  slinput, slstart, nullmodem },		/* 4- SLIPDISC */
#else
	{ ttynodisc, ttyerrclose, ttyerrio, ttyerrio, nullioctl,
	  ttyerrinput, ttyerrstart, nullmodem },
#endif
#if NPPP > 0
	{ pppopen, pppclose, pppread, pppwrite, ppptioctl,
	  pppinput, pppstart, nullmodem },		/* 5- PPPDISC */
#else
	{ ttynodisc, ttyerrclose, ttyerrio, ttyerrio, nullioctl,
	  ttyerrinput, ttyerrstart, nullmodem },
#endif
};

int	nlinesw = sizeof (linesw) / sizeof (linesw[0]);

/*
 * Do nothing specific version of line
 * discipline specific ioctl command.
 */
/*ARGSUSED*/
nullioctl(tp, cmd, data, flags, p)
	struct tty *tp;
	int cmd;
	char *data;
	int flags;
	struct proc *p;
{

#ifdef lint
	tp = tp; data = data; flags = flags; p = p;
#endif
	return (-1);
}
