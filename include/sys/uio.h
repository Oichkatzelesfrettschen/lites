/*
 * Copyright (c) 1982, 1986, 1993, 1994
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
 *	@(#)uio.h	8.5 (Berkeley) 2/22/94
 */

#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

#include <sys/types.h>

/*
 * I/O vector structure for scatter/gather I/O operations.
 */
struct iovec {
    void    *iov_base;  /* Base address of I/O memory region */
    size_t  iov_len;    /* Length of region */
};

/*
 * Segment flag values for uio_segflg.
 */
enum uio_seg {
    UIO_USERSPACE,      /* from user data space */
    UIO_SYSSPACE,       /* from system space */
    UIO_USERISPACE      /* from user I space */
};

/*
 * Direction flag for uio_rw.
 */
enum uio_rw {
    UIO_READ,           /* read operation */
    UIO_WRITE           /* write operation */
};

/*
 * UIO structure - describes I/O operation with potentially multiple buffers.
 * Used for scatter/gather I/O throughout the kernel.
 */
struct uio {
    struct iovec *uio_iov;      /* pointer to array of iovecs */
    int     uio_iovcnt;         /* number of iovecs in array */
    off_t   uio_offset;         /* offset into file this uio corresponds to */
    int     uio_resid;          /* residual i/o count */
    enum uio_seg uio_segflg;    /* see above */
    enum uio_rw uio_rw;         /* see above */
    struct proc *uio_procp;     /* associated thread or NULL */
};

/*
 * Limits on the number of iovecs and total size for I/O operations.
 */
#define UIO_MAXIOV      1024    /* max 1K of iov's */
#define UIO_SMALLIOV    8       /* 8 on stack, else malloc */

#ifdef KERNEL

/*
 * Function prototypes for UIO operations
 */
struct buf;
struct vnode;

int     uiomove(caddr_t cp, int n, struct uio *uio);
int     uiomoveco(caddr_t cp, int n, struct uio *uio);
int     ureadc(int c, struct uio *uio);
int     uwritec(struct uio *uio);

#endif /* KERNEL */

#endif /* !_SYS_UIO_H_ */
