/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * Copyright (c) 1994 Johannes Helander
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON AND JOHANNES HELANDER ALLOW FREE USE OF THIS
 * SOFTWARE IN ITS "AS IS" CONDITION.  CARNEGIE MELLON AND JOHANNES
 * HELANDER DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: subr_prf.c,v $
 * Revision 1.5  1995/08/15  06:49:01  sclawson
 * modifications from lites-1.1-950808
 *
 * Revision 1.4  1995/07/12  16:39:58  mike
 * actually return a value from dgetc
 *
 * Revision 1.3  1995/03/23  01:44:40  law
 * Update to 950323 snapshot + utah changes
 *
 *
 */
/* 
 *	File:	 subr_prf.c
 *	Authors:
 *	Randall Dean, Carnegie Mellon University, 1992.
 *	Johannes Helander, Helsinki University of Technology, 1994.
 *	Origin:	 Adapted to Lites from 4.4 BSD Lite.
 */
/*-
 * Copyright (c) 1986, 1988, 1991, 1993
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
 *	@(#)subr_prf.c	8.3 (Berkeley) 1/21/94
 */

#include "osfmach3.h"
#include "second_server.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/reboot.h>
#include <sys/msgbuf.h>
#include <sys/proc.h>
#include <sys/ioctl.h>
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/tty.h>
#include <sys/tprintf.h>
#include <sys/syslog.h>
#include <sys/malloc.h>
#include <sys/kernel.h>

/*
 * Note that stdarg.h and the ANSI style va_start macro is used for both
 * ANSI and traditional C compilers.
 */
#include <machine/stdarg.h>

#ifdef KADB
#include <machine/kdbparam.h>
#endif

#define TOCONS	0x01
#define TOTTY	0x02
#define TOLOG	0x04

struct	tty *constty;			/* pointer to console "window" tty */

#ifdef parisc
/* Should be a configuration option */
#define PUTCTRAP 1
#else
#define PUTCTRAP 0
#endif

#if PUTCTRAP
extern	cntrap();		/* standard console putc */
int	(*v_putc)() = cntrap;	/* routine to putc on virtual console */
#else
extern	cnputc();		/* standard console putc */
int	(*v_putc)() = cnputc;	/* routine to putc on virtual console */
#endif

void  logpri (int level);
static void  putchar (int ch, int flags, struct tty *tp);
static char *ksprintn (u_long num, int base, int *len);
void kprintf (const char *fmt, int flags, struct tty *tp, va_list ap);

int consintr = 1;			/* Ok to handle console interrupts? */

#if OSFMACH3
extern security_token_t security_id;
#endif

#if	SECOND_SERVER
extern	int 	second_server;
#endif	/* SECOND_SERVER */
/*
 * Variable panicstr contains argument to first call to panic; used as flag
 * to indicate that the kernel has already called panic.
 */
const char *panicstr;

/*
 * Panic is called on unresolvable fatal errors.  It prints "panic: mesg",
 * and then reboots.  If we are called twice, then we avoid trying to sync
 * the disks as this often leads to recursive panics.
 */

void panic(const char *fmt, ...)
{
	int bootopt;
	va_list ap;

	bootopt = RB_AUTOBOOT | RB_DUMP;
	if (panicstr)
		bootopt |= RB_NOSYNC;
	else
		panicstr = fmt;

	va_start(ap, fmt);
	printf("panic: %r\n", fmt, ap);
	va_end(ap);

#if	SECOND_SERVER
    if (second_server) {
	if (boothowto & RB_KDB)
	    Debugger("panic");
	else
	    task_suspend(mach_task_self());
	return;
    }
#endif	/* SECOND_SERVER */
    if (boothowto & RB_KDB) {
	    int s;
	    
	    for(s=1;s<20000;s++);
	    Debugger("Panic");
    } else {
	    boot(TRUE, bootopt);
    }
}

/* Called on nonfatal errors that might be interesting to debug */
void warning_panic(const char *fmt, ...)
{
	int bootopt;
	va_list ap;

	bootopt = RB_AUTOBOOT | RB_DUMP;

	va_start(ap, fmt);
	printf("warning_panic: %r\n", fmt, ap);
	va_end(ap);

#if	SECOND_SERVER
    if (second_server) {
	if (boothowto & RB_KDB)
	    Debugger("panic");
	return;
    }
#endif	/* SECOND_SERVER */
    if (boothowto & RB_KDB) {
	    int s;
	    
	    for(s=1;s<20000;s++);
	    Debugger("Panic");
    }
}

/*
 * Warn that a system table is full.
 */
void
tablefull(tab)
	const char *tab;
{

	log(LOG_ERR, "%s: table is full\n", tab);
}

/*
 * Uprintf prints to the controlling terminal for the current process.
 * It may block if the tty queue is overfull.  No message is printed if
 * the queue does not clear in a reasonable time.
 */
void uprintf(const char *fmt, ...)
{
	register struct proc *p = get_proc();
	va_list ap;

	if (p->p_flag & P_CONTROLT && p->p_session->s_ttyvp) {
		va_start(ap, fmt);
		kprintf(fmt, TOTTY, p->p_session->s_ttyp, ap);
		va_end(ap);
	}
}

tpr_t tprintf_open(struct proc *p)
{

	if (p->p_flag & P_CONTROLT && p->p_session->s_ttyvp) {
		SESSHOLD(p->p_session);
		return ((tpr_t) p->p_session);
	}
	return ((tpr_t) NULL);
}

void tprintf_close(tpr_t sess)
{

	if (sess)
		SESSRELE((struct session *) sess);
}

/*
 * tprintf prints on the controlling terminal associated
 * with the given session.
 */
void tprintf(tpr_t tpr, const char *fmt, ...)
{
	register struct session *sess = (struct session *)tpr;
	struct tty *tp = NULL;
	int flags = TOLOG;
	va_list ap;

	logpri(LOG_INFO);
	if (sess && sess->s_ttyvp && ttycheckoutq(sess->s_ttyp, 0)) {
		flags |= TOTTY;
		tp = sess->s_ttyp;
	}
	va_start(ap, fmt);
	kprintf(fmt, flags, tp, ap);
	va_end(ap);
	logwakeup();
}

/*
 * Ttyprintf displays a message on a tty; it should be used only by
 * the tty driver, or anything that knows the underlying tty will not
 * be revoke(2)'d away.  Other callers should use tprintf.
 */
void ttyprintf(struct tty *tp, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	kprintf(fmt, TOTTY, tp, ap);
	va_end(ap);
}

extern	int log_open;

/*
 * Log writes to the log buffer, and guarantees not to sleep (so can be
 * called by interrupt routines).  If there is no process reading the
 * log yet, it writes to the console also.
 */
void log(int level, const char *fmt, ...)
{
	register int s;
	va_list ap;

	s = splhigh();
	logpri(level);
	va_start(ap, fmt);
	kprintf(fmt, TOLOG, NULL, ap);
	splx(s);
	va_end(ap);
	if (!log_open) {
		va_start(ap, fmt);
		kprintf(fmt, TOCONS, NULL, ap);
		va_end(ap);
	}
	logwakeup();
}

void logpri(int level)
{
	register int ch;
	register char *p;

	putchar('<', TOLOG, NULL);
	for (p = ksprintn((u_long)level, 10, NULL); ch = *p--;)
		putchar(ch, TOLOG, NULL);
	putchar('>', TOLOG, NULL);
}

void addlog(const char *fmt, ...)
{
	register int s;
	va_list ap;

	s = splhigh();
	va_start(ap, fmt);
	kprintf(fmt, TOLOG, NULL, ap);
	splx(s);
	va_end(ap);
	if (!log_open) {
		va_start(ap, fmt);
		kprintf(fmt, TOCONS, NULL, ap);
		va_end(ap);
	}
	logwakeup();
}

/* 
 * XXX printf is called from all kinds of places that may hold any
 * XXX locks. It is very easy to deadlock when calling wakeup,
 * XXX selwakeup, tty->putc etc. from here.
 * XXX It would be better to just queue the stuff up here and have a
 * XXX separate thread do the printing.  That have drawbacks though
 * XXX (complex, hard to guarantee flush in panic, etc.)
 */
void printf(const char *fmt, ...)
{
	va_list ap;
	register int savintr;

	savintr = consintr;		/* disable interrupts */
	consintr = 0;
	va_start(ap, fmt);
	kprintf(fmt, TOCONS | TOLOG, NULL, ap);
	va_end(ap);
	if (!panicstr)
		logwakeup();
	consintr = savintr;		/* reenable interrupts */
}

/*
 * Scaled down version of printf(3).
 *
 * Two additional formats:
 *
 * The format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	kprintf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 *
 * The format %r passes an additional format string and argument list
 * recursively.  Its usage is:
 *
 * fn(char *fmt, ...)
 * {
 *	va_list ap;
 *	va_start(ap, fmt);
 *	printf("prefix: %r: suffix\n", fmt, ap);
 *	va_end(ap);
 * }
 *
 * Space or zero padding and a field width are supported for the numeric
 * formats only.
 */
void kprintf(
	const char *fmt,
	int flags,
	struct tty *tp,
	va_list ap)
{
	register char *p, *q;
	register int ch, n;
	u_long ul;
	int base, lflag, tmp, width;
	char padc;

	for (;;) {
		padc = ' ';
		width = 0;
		while ((ch = *(u_char *)fmt++) != '%') {
			if (ch == '\0')
				return;
			putchar(ch, flags, tp);
		}
		lflag = 0;
reswitch:	switch (ch = *(u_char *)fmt++) {
		case '0':
			padc = '0';
			goto reswitch;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			for (width = 0;; ++fmt) {
				width = width * 10 + ch - '0';
				ch = *fmt;
				if (ch < '0' || ch > '9')
					break;
			}
			goto reswitch;
		case 'l':
			lflag = 1;
			goto reswitch;
		case 'b':
			ul = va_arg(ap, int);
			p = va_arg(ap, char *);
			for (q = ksprintn(ul, *p++, NULL); ch = *q--;)
				putchar(ch, flags, tp);

			if (!ul)
				break;

			for (tmp = 0; n = *p++;) {
				if (ul & (1 << (n - 1))) {
					putchar(tmp ? ',' : '<', flags, tp);
					for (; (n = *p) > ' '; ++p)
						putchar(n, flags, tp);
					tmp = 1;
				} else
					for (; *p > ' '; ++p)
						continue;
			}
			if (tmp)
				putchar('>', flags, tp);
			break;
		case 'c':
			putchar(va_arg(ap, int), flags, tp);
			break;
		case 'r':
			p = va_arg(ap, char *);
			kprintf(p, flags, tp, va_arg(ap, va_list));
			break;
		case 's':
			p = va_arg(ap, char *);
			while (ch = *p++)
				putchar(ch, flags, tp);
			break;
		case 'd':
			ul = lflag ? va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				putchar('-', flags, tp);
				ul = -(long)ul;
			}
			base = 10;
			goto number;
		case 'o':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 8;
			goto number;
		case 'u':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 10;
			goto number;
		case 'x':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 16;
number:			p = ksprintn(ul, base, &tmp);
			if (width && (width -= tmp) > 0)
				while (width--)
					putchar(padc, flags, tp);
			while (ch = *p--)
				putchar(ch, flags, tp);
			break;
		default:
			putchar('%', flags, tp);
			if (lflag)
				putchar('l', flags, tp);
			/* FALLTHROUGH */
		case '%':
			putchar(ch, flags, tp);
		}
	}
}

/*
 * Print a character on console or users terminal.  If destination is
 * the console then the last MSGBUFS characters are saved in msgbuf for
 * inspection later.
 */
static void putchar(
	int c,
	int flags,
	struct tty *tp)
{
	extern int msgbufmapped;
	register struct msgbuf *mbp;
#if defined(SECOND_SERVER) && PUTCTRAP
	static int first = 1;
	extern cnputc();

	if (first) {
		first = 0;
		if (second_server)
		    v_putc = cnputc;
	}
#endif

	if (panicstr)
		constty = NULL;
	if ((flags & TOCONS) && tp == NULL && constty) {
		tp = constty;
		flags |= TOTTY;
	}
	if ((flags & TOTTY) && tp && tputchar(c, tp) < 0 &&
	    (flags & TOCONS) && tp == constty)
		constty = NULL;
	if ((flags & TOLOG) &&
	    c != '\0' && c != '\r' && c != 0177 && msgbufmapped) {
		mbp = msgbufp;	/* XXX locking */
		if (mbp->msg_magic != MSG_MAGIC) {
			memset((caddr_t)mbp, 0, sizeof(*mbp));
			mbp->msg_magic = MSG_MAGIC;
		}
		mbp->msg_bufc[mbp->msg_bufx++] = c;
		if (mbp->msg_bufx < 0 || mbp->msg_bufx >= MSG_BSIZE)
			mbp->msg_bufx = 0;
	}
	if ((flags & TOCONS) && constty == NULL && c != '\0')
		(*v_putc)(c);
}

#if PUTCTRAP
/* Should be in MD file */
cntrap(c)
       int c;
{
	asm volatile("
		copy    %0,%%r26
		ldi     -1,%%r25
		break   0,6"
		: : "r" (c));
}

cnputs(str, len)
	char *str;
	int len;
{
	asm volatile("
		copy    %0,%%r26
		copy    %1,%%r25
		break   0,6"
		: : "r" (str), "r" (len));
}
#endif

/*
 * Scaled down version of sprintf(3).
 */
sprintf(char *buf, const char *cfmt, ...)
{
	register const char *fmt = cfmt;
	register char *p, *bp;
	register int ch, base;
	u_long ul;
	int lflag;
	va_list ap;

	va_start(ap, cfmt);
	for (bp = buf; ; ) {
		while ((ch = *(u_char *)fmt++) != '%')
			if ((*bp++ = ch) == '\0')
				return ((bp - buf) - 1);

		lflag = 0;
reswitch:	switch (ch = *(u_char *)fmt++) {
		case 'l':
			lflag = 1;
			goto reswitch;
		case 'c':
			*bp++ = va_arg(ap, int);
			break;
		case 's':
			p = va_arg(ap, char *);
			while (*bp++ = *p++)
				continue;
			--bp;
			break;
		case 'd':
			ul = lflag ? va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				*bp++ = '-';
				ul = -(long)ul;
			}
			base = 10;
			goto number;
			break;
		case 'o':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 8;
			goto number;
			break;
		case 'u':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 10;
			goto number;
			break;
		case 'x':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 16;
number:			for (p = ksprintn(ul, base, NULL); ch = *p--;)
				*bp++ = ch;
			break;
		default:
			*bp++ = '%';
			if (lflag)
				*bp++ = 'l';
			/* FALLTHROUGH */
		case '%':
			*bp++ = ch;
		}
	}
	va_end(ap);
}

/*
 * Scaled down version of sprintf(3).
 */
vsprintf(char *buf, const char *fmt, va_list ap)
{
	register char *p, *bp;
	register int ch, base;
	u_long ul;
	int lflag;

	for (bp = buf; ; ) {
		while ((ch = *(u_char *)fmt++) != '%')
			if ((*bp++ = ch) == '\0')
				return ((bp - buf) - 1);

		lflag = 0;
reswitch:	switch (ch = *(u_char *)fmt++) {
		case 'l':
			lflag = 1;
			goto reswitch;
		case 'c':
			*bp++ = va_arg(ap, int);
			break;
		case 's':
			p = va_arg(ap, char *);
			while (*bp++ = *p++)
				continue;
			--bp;
			break;
		case 'd':
			ul = lflag ? va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				*bp++ = '-';
				ul = -(long)ul;
			}
			base = 10;
			goto number;
			break;
		case 'o':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 8;
			goto number;
			break;
		case 'u':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 10;
			goto number;
			break;
		case 'x':
			ul = lflag ? va_arg(ap, u_long) : va_arg(ap, u_int);
			base = 16;
number:			for (p = ksprintn(ul, base, NULL); ch = *p--;)
				*bp++ = ch;
			break;
		default:
			*bp++ = '%';
			if (lflag)
				*bp++ = 'l';
			/* FALLTHROUGH */
		case '%':
			*bp++ = ch;
		}
	}
}

/*
 * Put a number (base <= 16) in a buffer in reverse order; return an
 * optional length and a pointer to the NULL terminated (preceded?)
 * buffer.
 */
static char *ksprintn(
	u_long ul,
	int base,
	int *lenp)
{					/* A long in base 8, plus NULL. */
	static char buf[sizeof(long) * NBBY / 3 + 2];
	register char *p;

	p = buf;
	do {
		*++p = "0123456789abcdef"[ul % base];
	} while (ul /= base);
	if (lenp)
		*lenp = p - buf;
	return (p);
}

/* Physical console printf for boot time */
#include <serv/import_mach.h>
#include <serv/device.h>

/*
 * NOTE: you need to put a \r in front of every \n
 * in the fmt string.
 */
static char		dprintf_buf[256];
mach_port_t		dprintf_console_port = MACH_PORT_NULL;
static int 		dprintf_initted = 0;

extern mach_port_t	device_server_port;

dprintf(const char *fmt, ...)
{
	char *bp = dprintf_buf;
	va_list	ap;
	int len;
	int done;

	if (!dprintf_initted) {
		kern_return_t	rc;

		if (! device_server_port) return;
		dprintf_initted = 1;
		/*
		 * Open the console
		 */
		rc = device_open(device_server_port,
#if OSF_LEDGERS
				 MACH_PORT_NULL,
#endif
				 D_READ|D_WRITE,
#if OSFMACH3
				 security_id,
#endif
				 "console",
				 &dprintf_console_port);
		if (rc != KERN_SUCCESS)
		    panic("dprintf: console_init");
	}

	va_start(ap, fmt);
	len = vsprintf(bp, fmt, ap);
	va_end(ap);

#if PUTCTRAP
	cnputs(dprintf_buf, len);
#else
	bp = dprintf_buf;
	if (dprintf_buf[len] == '\n')
	  dprintf_buf[ ++len ] = '\r';

	while (len > 0) {
	    (void) device_write_inband(dprintf_console_port,
#define _min(x,y) (((x)<(y))?(x):(y))
			0, 0, bp, _min(len, 128), &done);
	    bp  += done;
	    len -= done;
	}
#endif
}

dgetc()
{
	mach_msg_type_number_t len;
	io_buf_ptr_inband_t chr;
	kern_return_t	rc;

	if (!dprintf_initted) {
		if (! device_server_port) return -1;
		dprintf_initted = 1;
		/*
		 * Open the console
		 */
		rc = device_open(device_server_port,
#if OSF_LEDGERS
				 MACH_PORT_NULL,
#endif
				 D_READ|D_WRITE,
#if OSFMACH3
				 security_id,
#endif
				 "console",
				 &dprintf_console_port);
		if (rc != KERN_SUCCESS)
			panic("dgetc: console_init");
	}
	len = 1;
	rc = device_read_inband(dprintf_console_port,
				(dev_mode_t)0, (recnum_t)0, 1,
				chr, &len);
	if (rc != KERN_SUCCESS) {
		panic("dgetc: device_read_inband %x %s",
		      rc, mach_error_string(rc));
		return -1;
	}
	return (len == 1) ? chr[0] : -1;
}

/* Prompt on console and read string.  size includes nul */
dget_string(const char *prompt, char *buf, int size)
{
	int count = 0;
	int c;

	if (prompt)
		dprintf(prompt);

	while (count < size) {
		c = dgetc();
		switch (c) {
		case -1:
		case '.':
		case '\n':
		case '\r':
			dprintf("\n\r");
			buf[count] = '\0';
			return;
		case '\b':
		case '\177':
			if (count <= 0)
				continue;
			dprintf("\b \b");
			count--;
			break;
		default:
			buf[count] = c;
			dprintf("%c", c);
			count++;
			break;
		}
	}
	buf[count] = '\0';
}
