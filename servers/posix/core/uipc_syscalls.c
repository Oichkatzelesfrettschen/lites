/* 
 * Mach Operating System
 * Copyright (c) 1994 Johannes Helander
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * JOHANNES HELANDER ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  JOHANNES HELANDER DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: uipc_syscalls.c,v $
 * Revision 1.1.1.2  1995/03/23  01:16:35  law
 * lites-950323 from jvh.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1989, 1990, 1993
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
 *	@(#)uipc_syscalls.c	8.4 (Berkeley) 2/21/94
 */

#include "compat_43.h"
#include "compat_oldsock.h"

#include <serv/server_defs.h>
#include <sys/param.h>
#include <sys/filedesc.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#ifdef KTRACE
#include <sys/ktrace.h>
#endif
#include <sys/kernel.h>

/*
 * System call interface to the socket abstraction.
 */

extern	struct fileops socketops;

struct socket_args {
	int	domain;
	int	type;
	int	protocol;
};
socket(p, uap, retval)
	struct proc *p;
	register struct socket_args *uap;
	int *retval;
{
	struct filedesc *fdp = p->p_fd;
	struct socket *so;
	struct file *fp;
	int fd, error;

	if (error = falloc(p, &fp, &fd))
		return (error);
	fp->f_flag = FREAD|FWRITE;
	fp->f_type = DTYPE_SOCKET;
	fp->f_ops = &socketops;
	if (error = socreate(uap->domain, &so, uap->type, uap->protocol)) {
		fdp->fd_ofiles[fd] = 0;
		ffree(fp);
	} else {
		fp->f_data = (caddr_t)so;
		*retval = fd;
	}
	return (error);
}

mach_error_t s_bind(
	struct proc *p,
	int	s,
	caddr_t	name,
	int	namelen)
{
	struct file *fp;
	struct mbuf *nam;
	int error;

	if (error = getsock(p->p_fd, s, &fp))
		return (error);
	if (error = NEWsockargs(&nam, name, namelen, MT_SONAME))
		return (error);
	error = sobind((struct socket *)fp->f_data, nam);
	m_freem(nam);
	return (error);
}

struct listen_args {
	int	s;
	int	backlog;
};
/* ARGSUSED */
listen(p, uap, retval)
	struct proc *p;
	register struct listen_args *uap;
	int *retval;
{
	struct file *fp;
	int error;

	if (error = getsock(p->p_fd, uap->s, &fp))
		return (error);
	return (solisten((struct socket *)fp->f_data, uap->backlog));
}

struct accept_args {
	int	s;
	caddr_t	name;
	int	*anamelen;
};

accept(
	struct proc *p,
	struct accept_args *uap,
	int *retval)
{
	struct file *fp;
	struct mbuf *nam;
	int namelen, error, s;
	register struct socket *so;

	if (uap->name && (error = copyin((caddr_t)uap->anamelen,
	    (caddr_t)&namelen, sizeof (namelen))))
		return (error);
	if (error = getsock(p->p_fd, uap->s, &fp))
		return (error);
	s = splnet();
	so = (struct socket *)fp->f_data;
	if ((so->so_options & SO_ACCEPTCONN) == 0) {
		splx(s);
		return (EINVAL);
	}
	if ((so->so_state & SS_NBIO) && so->so_qlen == 0) {
		splx(s);
		return (EWOULDBLOCK);
	}
	while (so->so_qlen == 0 && so->so_error == 0) {
		if (so->so_state & SS_CANTRCVMORE) {
			so->so_error = ECONNABORTED;
			break;
		}
		if (error = tsleep((caddr_t)&so->so_timeo, PSOCK | PCATCH,
		    netcon, 0)) {
			splx(s);
			return (error);
		}
	}
	if (so->so_error) {
		error = so->so_error;
		so->so_error = 0;
		splx(s);
		return (error);
	}
	if (error = falloc(p, &fp, retval)) {
		splx(s);
		return (error);
	}
	{ struct socket *aso = so->so_q;
	  if (soqremque(aso, 1) == 0)
		panic("accept");
	  so = aso;
	}
	fp->f_type = DTYPE_SOCKET;
	fp->f_flag = FREAD|FWRITE;
	fp->f_ops = &socketops;
	fp->f_data = (caddr_t)so;
	nam = m_get(M_WAIT, MT_SONAME);
	(void) soaccept(so, nam);
	if (uap->name) {
		if (namelen > nam->m_len)
			namelen = nam->m_len;
		/* SHOULD COPY OUT A CHAIN HERE */
		if ((error = copyout(mtod(nam, caddr_t), (caddr_t)uap->name,
		    (u_int)namelen)) == 0)
			error = copyout((caddr_t)&namelen,
			    (caddr_t)uap->anamelen, sizeof (*uap->anamelen));
	}
	m_freem(nam);
	splx(s);
	return (error);
}

mach_error_t s_connect(
	struct proc *p,
	int	sock,
	caddr_t	name,
	int	namelen)
{
	struct file *fp;
	register struct socket *so;
	struct mbuf *nam;
	int error, s;

	if (error = getsock(p->p_fd, sock, &fp))
		return (error);
	so = (struct socket *)fp->f_data;
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING))
		return (EALREADY);
	if (error = NEWsockargs(&nam, name, namelen, MT_SONAME))
		return (error);
	error = soconnect(so, nam);
	if (error)
		goto bad;
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		m_freem(nam);
		return (EINPROGRESS);
	}
	s = splnet();
	while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0)
		if (error = tsleep((caddr_t)&so->so_timeo, PSOCK | PCATCH,
		    netcon, 0))
			break;
	if (error == 0) {
		error = so->so_error;
		so->so_error = 0;
	}
	splx(s);
bad:
	so->so_state &= ~SS_ISCONNECTING;
	m_freem(nam);
	if (error == ERESTART)
		error = EINTR;
	return (error);
}

struct socketpair_args {
	int	domain;
	int	type;
	int	protocol;
	int	*rsv;
};
socketpair(p, uap, retval)
	struct proc *p;
	register struct socketpair_args *uap;
	int retval[];
{
	register struct filedesc *fdp = p->p_fd;
	struct file *fp1, *fp2;
	struct socket *so1, *so2;
	int fd, error, sv[2];

	if (error = socreate(uap->domain, &so1, uap->type, uap->protocol))
		return (error);
	if (error = socreate(uap->domain, &so2, uap->type, uap->protocol))
		goto free1;
	if (error = falloc(p, &fp1, &fd))
		goto free2;
	sv[0] = fd;
	fp1->f_flag = FREAD|FWRITE;
	fp1->f_type = DTYPE_SOCKET;
	fp1->f_ops = &socketops;
	fp1->f_data = (caddr_t)so1;
	if (error = falloc(p, &fp2, &fd))
		goto free3;
	fp2->f_flag = FREAD|FWRITE;
	fp2->f_type = DTYPE_SOCKET;
	fp2->f_ops = &socketops;
	fp2->f_data = (caddr_t)so2;
	sv[1] = fd;
	if (error = soconnect2(so1, so2))
		goto free4;
	if (uap->type == SOCK_DGRAM) {
		/*
		 * Datagram socket connection is asymmetric.
		 */
		 if (error = soconnect2(so2, so1))
			goto free4;
	}
	error = copyout((caddr_t)sv, (caddr_t)uap->rsv, 2 * sizeof (int));
	retval[0] = sv[0];		/* XXX ??? */
	retval[1] = sv[1];		/* XXX ??? */
	return (error);
free4:
	ffree(fp2);
	fdp->fd_ofiles[sv[1]] = 0;
free3:
	ffree(fp1);
	fdp->fd_ofiles[sv[0]] = 0;
free2:
	(void)soclose(so2);
free1:
	(void)soclose(so1);
	return (error);
}

mach_error_t s_sendmsg(
	struct proc	*p,
	int		s,
	int		flags,
	struct sockaddr	*to,
	int		tolen,
	struct cmsghdr	*cmsg,
	int		cmsglen,
	char		*data,
	int		datalen,
	size_t		*retsize)
{
	struct file *fp;
	struct uio auio;
	struct iovec aiov, *iov;
	struct mbuf *to_mbuf, *control;
	int len, error, i;

	if (error = getsock(p->p_fd, s, &fp))
		return (error);

	iov = &aiov;
	aiov.iov_base = data;
	aiov.iov_len = datalen;
	if (datalen < 0)
	    return EINVAL;

	auio.uio_iov = iov;
	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_procp = p;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = iov->iov_len;
	if (to && tolen != 0) {
		/* wrap mbuf around address */
		if (error = NEWsockargs(&to_mbuf, to, tolen, MT_SONAME))
			return error;
	} else
		to_mbuf = 0;

	if (cmsg && cmsglen != 0) {
		if (cmsglen < sizeof(struct cmsghdr)) {
			error = EINVAL;
			goto bad;
		}
		if (error = NEWsockargs(&control, cmsg, cmsglen, MT_CONTROL))
			goto bad;
	} else
		control = 0;
	len = auio.uio_resid;
	if (error = sosend((struct socket *)fp->f_data, to_mbuf, &auio,
	    (struct mbuf *)0, control, flags)) {
		if (auio.uio_resid != len && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
		if (error == EPIPE)
			psignal(p, SIGPIPE);
	}
	if (error == 0)
		*retsize = len - auio.uio_resid;
bad:
	if (to_mbuf)
		m_freem(to_mbuf);
	return (error);
}

mach_error_t s_recvmsg(
	struct proc	*p,
	int		s,
	int		flags,
	int		*outflags,	/* OUT */
	struct sockaddr	*from,		/* OUT */
	int		*fromlen,	/* IN/OUT */
	struct cmsghdr	*cmsg,		/* OUT */
	int		*cmsglen,	/* IN/OUT */
	char		*data,		/* OUT */
	int		datalen,
	size_t		*retsize)	/* OUT */
{
	struct file *fp;
	struct uio auio;
	struct iovec aiov, *iov;
	int error, i;
	struct mbuf *from_mbuf = 0, *control = 0;

	if (error = getsock(p->p_fd, s, &fp))
		return (error);

	iov = &aiov;
	aiov.iov_base = data;
	aiov.iov_len = datalen;
	if (datalen < 0)
	    return EINVAL;

	auio.uio_iov = iov;
	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_procp = p;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = datalen;

	*outflags = flags;	/* in/out for soreceive */
	if (error = soreceive((struct socket *)fp->f_data, &from_mbuf, &auio,
	    (struct mbuf **)0,
 	    (cmsg && cmsglen != 0) ? &control : (struct mbuf **)0,
	    outflags)) {
		if (auio.uio_resid != datalen && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error)
		goto out;
	*retsize = datalen - auio.uio_resid;
	if (from && *fromlen != 0) {
		int len = *fromlen;
		if (len <= 0 || from_mbuf == 0)
			len = 0;
		else {
			if (len > from_mbuf->m_len)
				len = from_mbuf->m_len;
			/* else if len < from_mbuf->m_len ??? */
			memcpy(caddr_t), mtod(from_mbuf, from, (unsigned) len);
		}
		*fromlen = len;
	}
	if (cmsg && cmsglen != 0) {
		int len;
#if COMPAT_OLDSOCK && 0 /* XXX notyet */
		/*
		 * We assume that old recvmsg calls won't receive access
		 * rights and other control info, esp. as control info
		 * is always optional and those options didn't exist in 4.3.
		 * If we receive rights, trim the cmsghdr; anything else
		 * is tossed.
		 */
		if (control && mp->msg_flags & MSG_COMPAT) {
			if (mtod(control, struct cmsghdr *)->cmsg_level !=
			    SOL_SOCKET ||
			    mtod(control, struct cmsghdr *)->cmsg_type !=
			    SCM_RIGHTS) {
				mp->msg_controllen = 0;
				goto out;
			}
			control->m_len -= sizeof (struct cmsghdr);
			control->m_data += sizeof (struct cmsghdr);
		}
#endif
		len = *cmsglen;
		if (len <= 0 || control == 0)
			len = 0;
		else {
			if (len >= control->m_len)
				len = control->m_len;
			else
				*outflags |= MSG_CTRUNC;
			memcpy(caddr_t), (caddr_t) mtod(control, cmsg, len);
		}
		*cmsglen = len;
	}
out:
	if (from_mbuf)
		m_freem(from_mbuf);
	if (control)
		m_freem(control);
	return (error);
}

struct shutdown_args {
	int	s;
	int	how;
};
/* ARGSUSED */
shutdown(p, uap, retval)
	struct proc *p;
	register struct shutdown_args *uap;
	int *retval;
{
	struct file *fp;
	int error;

	if (error = getsock(p->p_fd, uap->s, &fp))
		return (error);
	return (soshutdown((struct socket *)fp->f_data, uap->how));
}

mach_error_t s_setsockopt(
	struct proc *p,
	int	s,
	int	level,
	int	name,
	caddr_t	val,
	int	valsize)
{
	struct file *fp;
	struct mbuf *m = NULL;
	int error;

	if (error = getsock(p->p_fd, s, &fp))
		return (error);
	if (valsize > MLEN)
		return (EINVAL);
	if (val) {
		m = m_get(M_WAIT, MT_SOOPTS);
		if (m == NULL)
			return (ENOBUFS);
		memcpy(mtod(m, val, caddr_t), (u_int)valsize);
		m->m_len = valsize;
	}
	return sosetopt((struct socket *)fp->f_data, level, name, m);
}

mach_error_t s_getsockopt(
	struct proc *p,
	int	s,
	int	level,
	int	name,
	caddr_t	val,
	int	*avalsize)
{
	struct file *fp;
	struct mbuf *m = NULL;
	int valsize, error;

	if (error = getsock(p->p_fd, s, &fp))
		return (error);
	if (val)
		memcpy((caddr_t)&valsize, (caddr_t)avalsize, sizeof (valsize));
	else
		valsize = 0;
	if ((error = sogetopt((struct socket *)fp->f_data, level,
	    name, &m)) == 0 && val && valsize && m != NULL) {
		if (valsize > m->m_len)
			valsize = m->m_len;
		error = copyout(mtod(m, caddr_t), val, (u_int)valsize);
		if (error == 0)
			error = copyout((caddr_t)&valsize,
			    (caddr_t)avalsize, sizeof (valsize));
	}
	if (m != NULL)
		(void) m_free(m);
	return (error);
}

struct pipe_args {
	int	dummy;
};
/* ARGSUSED */
pipe(p, uap, retval)
	struct proc *p;
	struct pipe_args *uap;
	int retval[];
{
	register struct filedesc *fdp = p->p_fd;
	struct file *rf, *wf;
	struct socket *rso, *wso;
	int fd, error;

	if (error = socreate(AF_UNIX, &rso, SOCK_STREAM, 0))
		return (error);
	if (error = socreate(AF_UNIX, &wso, SOCK_STREAM, 0))
		goto free1;
	if (error = falloc(p, &rf, &fd))
		goto free2;
	retval[0] = fd;
	rf->f_flag = FREAD;
	rf->f_type = DTYPE_SOCKET;
	rf->f_ops = &socketops;
	rf->f_data = (caddr_t)rso;
	if (error = falloc(p, &wf, &fd))
		goto free3;
	wf->f_flag = FWRITE;
	wf->f_type = DTYPE_SOCKET;
	wf->f_ops = &socketops;
	wf->f_data = (caddr_t)wso;
	retval[1] = fd;
	if (error = unp_connect2(wso, rso))
		goto free4;
	return (0);
free4:
	ffree(wf);
	fdp->fd_ofiles[retval[1]] = 0;
free3:
	ffree(rf);
	fdp->fd_ofiles[retval[0]] = 0;
free2:
	(void)soclose(wso);
free1:
	(void)soclose(rso);
	return (error);
}

/*
 * Get socket name.
 */
struct getsockname_args {
	int	fdes;
	caddr_t	asa;
	int	*alen;
};
/* ARGSUSED */
getsockname(
	struct proc *p,
	register struct getsockname_args *uap,
	int *retval)
{
	struct file *fp;
	register struct socket *so;
	struct mbuf *m;
	int len, error;

	if (error = getsock(p->p_fd, uap->fdes, &fp))
		return (error);
	if (error = copyin((caddr_t)uap->alen, (caddr_t)&len, sizeof (len)))
		return (error);
	so = (struct socket *)fp->f_data;
	m = m_getclr(M_WAIT, MT_SONAME);
	if (m == NULL)
		return (ENOBUFS);
	if (error = (*so->so_proto->pr_usrreq)(so, PRU_SOCKADDR, 0, m, 0))
		goto bad;
	if (len > m->m_len)
		len = m->m_len;
	error = copyout(mtod(m, caddr_t), (caddr_t)uap->asa, (u_int)len);
	if (error == 0)
		error = copyout((caddr_t)&len, (caddr_t)uap->alen,
		    sizeof (len));
bad:
	m_freem(m);
	return (error);
}

/*
 * Get name of peer for connected socket.
 */
struct getpeername_args {
	int	fdes;
	caddr_t	asa;
	int	*alen;
};

/* ARGSUSED */
getpeername(
	struct proc *p,
	struct getpeername_args *uap,
	int *retval)
{
	struct file *fp;
	register struct socket *so;
	struct mbuf *m;
	int len, error;

	if (error = getsock(p->p_fd, uap->fdes, &fp))
		return (error);
	so = (struct socket *)fp->f_data;
	if ((so->so_state & (SS_ISCONNECTED|SS_ISCONFIRMING)) == 0)
		return (ENOTCONN);
	if (error = copyin((caddr_t)uap->alen, (caddr_t)&len, sizeof (len)))
		return (error);
	m = m_getclr(M_WAIT, MT_SONAME);
	if (m == NULL)
		return (ENOBUFS);
	if (error = (*so->so_proto->pr_usrreq)(so, PRU_PEERADDR, 0, m, 0))
		goto bad;
	if (len > m->m_len)
		len = m->m_len;
	if (error = copyout(mtod(m, caddr_t), (caddr_t)uap->asa, (u_int)len))
		goto bad;
	error = copyout((caddr_t)&len, (caddr_t)uap->alen, sizeof (len));
bad:
	m_freem(m);
	return (error);
}

/* wrap in mbuf */
NEWsockargs(mp, buf, buflen, type)
	struct mbuf **mp;
	caddr_t buf;
	int buflen, type;
{
	register struct sockaddr *sa;
	register struct mbuf *m;
	int error;

	if ((u_int)buflen > MLEN) {
#if COMPAT_OLDSOCK
		if (type == MT_SONAME && (u_int)buflen <= 112)
			buflen = MLEN;		/* unix domain compat. hack */
		else
#endif
		return (EINVAL);
	}
	m = m_get(M_WAIT, type);
	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	memcpy(mtod(m, buf, caddr_t), (u_int)buflen);
	*mp = m;
	if (type == MT_SONAME) {
		sa = mtod(m, struct sockaddr *);

#if COMPAT_OLDSOCK && (BYTE_ORDER != BIG_ENDIAN)
		if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
		    sa->sa_family = sa->sa_len;
#endif
		sa->sa_len = buflen;
	}
	return KERN_SUCCESS;
}

getsock(fdp, fdes, fpp)
	struct filedesc *fdp;
	int fdes;
	struct file **fpp;
{
	register struct file *fp;

	if ((unsigned)fdes >= fdp->fd_nfiles ||
	    (fp = fdp->fd_ofiles[fdes]) == NULL)
		return (EBADF);
	if (fp->f_type != DTYPE_SOCKET)
		return (ENOTSOCK);
	*fpp = fp;
	return (0);
}


/* sockargs: Otherwise obsolete but still used by nfssvc and nfs_mount */
/* copy in and wrap in mbuf */
sockargs(mp, buf, buflen, type)
	struct mbuf **mp;
	caddr_t buf;
	int buflen, type;
{
	register struct sockaddr *sa;
	register struct mbuf *m;
	int error;

	if ((u_int)buflen > MLEN) {
#if COMPAT_OLDSOCK
		if (type == MT_SONAME && (u_int)buflen <= 112)
			buflen = MLEN;		/* unix domain compat. hack */
		else
#endif
		return (EINVAL);
	}
	m = m_get(M_WAIT, type);
	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	error = copyin(buf, mtod(m, caddr_t), (u_int)buflen);
	if (error)
		(void) m_free(m);
	else {
		*mp = m;
		if (type == MT_SONAME) {
			sa = mtod(m, struct sockaddr *);

#if COMPAT_OLDSOCK && (BYTE_ORDER != BIG_ENDIAN)
			if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
				sa->sa_family = sa->sa_len;
#endif
			sa->sa_len = buflen;
		}
	}
	return (error);
}


/* OBSOLETE CODE FOLLOWS */

#define MSG_COMPAT	0x8000

struct sendmsg_args {
	int	s;
	caddr_t	msg;
	int	flags;
};
sendmsg(p, uap, retval)
	struct proc *p;
	register struct sendmsg_args *uap;
	int *retval;
{
	struct msghdr msg;
	struct iovec aiov[UIO_SMALLIOV], *iov;
	int error;

	if (error = copyin(uap->msg, (caddr_t)&msg, sizeof (msg)))
		return (error);
	if ((u_int)msg.msg_iovlen >= UIO_SMALLIOV) {
		if ((u_int)msg.msg_iovlen >= UIO_MAXIOV)
			return (EMSGSIZE);
		MALLOC(iov, struct iovec *,
		       sizeof(struct iovec) * (u_int)msg.msg_iovlen, M_IOV,
		       M_WAITOK);
	} else
		iov = aiov;
	if (msg.msg_iovlen &&
	    (error = copyin((caddr_t)msg.msg_iov, (caddr_t)iov,
	    (unsigned)(msg.msg_iovlen * sizeof (struct iovec)))))
		goto done;
	msg.msg_iov = iov;
#if COMPAT_OLDSOCK
	msg.msg_flags = 0;
#endif
	error = sendit(p, uap->s, &msg, uap->flags, retval);
done:
	if (iov != aiov)
		FREE(iov, M_IOV);
	return (error);
}

sendit(p, s, mp, flags, retsize)
	register struct proc *p;
	int s;
	register struct msghdr *mp;
	int flags, *retsize;
{
	struct file *fp;
	struct uio auio;
	register struct iovec *iov;
	register int i;
	struct mbuf *to, *control;
	int len, error;
#ifdef KTRACE
	struct iovec *ktriov = NULL;
#endif
	
	if (error = getsock(p->p_fd, s, &fp))
		return (error);
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_procp = p;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if (iov->iov_len < 0)
			return (EINVAL);
		if ((auio.uio_resid += iov->iov_len) < 0)
			return (EINVAL);
	}
	if (mp->msg_name) {
		if (error = sockargs(&to, mp->msg_name, mp->msg_namelen,
		    MT_SONAME))
			return (error);
	} else
		to = 0;
	if (mp->msg_control) {
		if (mp->msg_controllen < sizeof(struct cmsghdr)
#if COMPAT_OLDSOCK
		    && mp->msg_flags != MSG_COMPAT
#endif
		) {
			error = EINVAL;
			goto bad;
		}
		if (error = sockargs(&control, mp->msg_control,
		    mp->msg_controllen, MT_CONTROL))
			goto bad;
#if COMPAT_OLDSOCK
		if (mp->msg_flags == MSG_COMPAT) {
			register struct cmsghdr *cm;

			M_PREPEND(control, sizeof(*cm), M_WAIT);
			if (control == 0) {
				error = ENOBUFS;
				goto bad;
			} else {
				cm = mtod(control, struct cmsghdr *);
				cm->cmsg_len = control->m_len;
				cm->cmsg_level = SOL_SOCKET;
				cm->cmsg_type = SCM_RIGHTS;
			}
		}
#endif
	} else
		control = 0;
#ifdef KTRACE
	if (KTRPOINT(p, KTR_GENIO)) {
		int iovlen = auio.uio_iovcnt * sizeof (struct iovec);

		MALLOC(ktriov, struct iovec *, iovlen, M_TEMP, M_WAITOK);
		memcpy((caddr_t)ktriov, (caddr_t)auio.uio_iov, iovlen);
	}
#endif
	len = auio.uio_resid;
	if (error = sosend((struct socket *)fp->f_data, to, &auio,
	    (struct mbuf *)0, control, flags)) {
		if (auio.uio_resid != len && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
		if (error == EPIPE)
			psignal(p, SIGPIPE);
	}
	if (error == 0)
		*retsize = len - auio.uio_resid;
#ifdef KTRACE
	if (ktriov != NULL) {
		if (error == 0)
			ktrgenio(p->p_tracep, s, UIO_WRITE,
				ktriov, *retsize, error);
		FREE(ktriov, M_TEMP);
	}
#endif
bad:
	if (to)
		m_freem(to);
	return (error);
}

struct recvmsg_args {
	int	s;
	struct	msghdr *msg;
	int	flags;
};
recvmsg(p, uap, retval)
	struct proc *p;
	register struct recvmsg_args *uap;
	int *retval;
{
	struct msghdr msg;
	struct iovec aiov[UIO_SMALLIOV], *uiov, *iov;
	register int error;

	if (error = copyin((caddr_t)uap->msg, (caddr_t)&msg, sizeof (msg)))
		return (error);
	if ((u_int)msg.msg_iovlen >= UIO_SMALLIOV) {
		if ((u_int)msg.msg_iovlen >= UIO_MAXIOV)
			return (EMSGSIZE);
		MALLOC(iov, struct iovec *,
		       sizeof(struct iovec) * (u_int)msg.msg_iovlen, M_IOV,
		       M_WAITOK);
	} else
		iov = aiov;
#if COMPAT_OLDSOCK
	msg.msg_flags = uap->flags &~ MSG_COMPAT;
#else
	msg.msg_flags = uap->flags;
#endif
	uiov = msg.msg_iov;
	msg.msg_iov = iov;
	if (error = copyin((caddr_t)uiov, (caddr_t)iov,
	    (unsigned)(msg.msg_iovlen * sizeof (struct iovec))))
		goto done;
	if ((error = recvit(p, uap->s, &msg, (caddr_t)0, retval)) == 0) {
		msg.msg_iov = uiov;
		error = copyout((caddr_t)&msg, (caddr_t)uap->msg, sizeof(msg));
	}
done:
	if (iov != aiov)
		FREE(iov, M_IOV);
	return (error);
}

recvit(p, s, mp, namelenp, retsize)
	register struct proc *p;
	int s;
	register struct msghdr *mp;
	caddr_t namelenp;
	int *retsize;
{
	struct file *fp;
	struct uio auio;
	register struct iovec *iov;
	register int i;
	int len, error;
	struct mbuf *from = 0, *control = 0;
#ifdef KTRACE
	struct iovec *ktriov = NULL;
#endif
	
	if (error = getsock(p->p_fd, s, &fp))
		return (error);
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_procp = p;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if (iov->iov_len < 0)
			return (EINVAL);
		if ((auio.uio_resid += iov->iov_len) < 0)
			return (EINVAL);
	}
#ifdef KTRACE
	if (KTRPOINT(p, KTR_GENIO)) {
		int iovlen = auio.uio_iovcnt * sizeof (struct iovec);

		MALLOC(ktriov, struct iovec *, iovlen, M_TEMP, M_WAITOK);
		memcpy((caddr_t)ktriov, (caddr_t)auio.uio_iov, iovlen);
	}
#endif
	len = auio.uio_resid;
	if (error = soreceive((struct socket *)fp->f_data, &from, &auio,
	    (struct mbuf **)0, mp->msg_control ? &control : (struct mbuf **)0,
	    &mp->msg_flags)) {
		if (auio.uio_resid != len && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
#ifdef KTRACE
	if (ktriov != NULL) {
		if (error == 0)
			ktrgenio(p->p_tracep, s, UIO_READ,
				ktriov, len - auio.uio_resid, error);
		FREE(ktriov, M_TEMP);
	}
#endif
	if (error)
		goto out;
	*retsize = len - auio.uio_resid;
	if (mp->msg_name) {
		len = mp->msg_namelen;
		if (len <= 0 || from == 0)
			len = 0;
		else {
#if COMPAT_OLDSOCK
			if (mp->msg_flags & MSG_COMPAT)
				mtod(from, struct osockaddr *)->sa_family =
				    mtod(from, struct sockaddr *)->sa_family;
#endif
			if (len > from->m_len)
				len = from->m_len;
			/* else if len < from->m_len ??? */
			if (error = copyout(mtod(from, caddr_t),
			    (caddr_t)mp->msg_name, (unsigned)len))
				goto out;
		}
		mp->msg_namelen = len;
		if (namelenp &&
		    (error = copyout((caddr_t)&len, namelenp, sizeof (int)))) {
#if COMPAT_OLDSOCK
			if (mp->msg_flags & MSG_COMPAT)
				error = 0;	/* old recvfrom didn't check */
			else
#endif
			goto out;
		}
	}
	if (mp->msg_control) {
#if COMPAT_OLDSOCK
		/*
		 * We assume that old recvmsg calls won't receive access
		 * rights and other control info, esp. as control info
		 * is always optional and those options didn't exist in 4.3.
		 * If we receive rights, trim the cmsghdr; anything else
		 * is tossed.
		 */
		if (control && mp->msg_flags & MSG_COMPAT) {
			if (mtod(control, struct cmsghdr *)->cmsg_level !=
			    SOL_SOCKET ||
			    mtod(control, struct cmsghdr *)->cmsg_type !=
			    SCM_RIGHTS) {
				mp->msg_controllen = 0;
				goto out;
			}
			control->m_len -= sizeof (struct cmsghdr);
			control->m_data += sizeof (struct cmsghdr);
		}
#endif
		len = mp->msg_controllen;
		if (len <= 0 || control == 0)
			len = 0;
		else {
			if (len >= control->m_len)
				len = control->m_len;
			else
				mp->msg_flags |= MSG_CTRUNC;
			error = copyout((caddr_t)mtod(control, caddr_t),
			    (caddr_t)mp->msg_control, (unsigned)len);
		}
		mp->msg_controllen = len;
	}
out:
	if (from)
		m_freem(from);
	if (control)
		m_freem(control);
	return (error);
}
