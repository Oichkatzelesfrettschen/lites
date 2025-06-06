/*
 * Copyright (c) 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 *	@(#)nfs_socket.c	8.3 (Berkeley) 1/12/94
 */

/*
 * Socket operations for use by nfs
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/vnode.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/syslog.h>
#include <sys/tprintf.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <nfs/rpcv2.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>
#include <nfs/nfsmount.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsrtt.h>
#include <nfs/nqnfs.h>

/*
 * Estimate rto for an nfs rpc sent via. an unreliable datagram.
 * Use the mean and mean deviation of rtt for the appropriate type of rpc
 * for the frequent rpcs and a default for the others.
 * The justification for doing "other" this way is that these rpcs
 * happen so infrequently that timer est. would probably be stale.
 * Also, since many of these rpcs are
 * non-idempotent, a conservative timeout is desired.
 * getattr, lookup - A+2D
 * read, write     - A+4D
 * other           - nm_timeo
 */
#define	NFS_RTO(n, t) \
	((t) == 0 ? (n)->nm_timeo : \
	 ((t) < 3 ? \
	  (((((n)->nm_srtt[t-1] + 3) >> 2) + (n)->nm_sdrtt[t-1] + 1) >> 1) : \
	  ((((n)->nm_srtt[t-1] + 7) >> 3) + (n)->nm_sdrtt[t-1] + 1)))
#define	NFS_SRTT(r)	(r)->r_nmp->nm_srtt[proct[(r)->r_procnum] - 1]
#define	NFS_SDRTT(r)	(r)->r_nmp->nm_sdrtt[proct[(r)->r_procnum] - 1]
/*
 * External data, mostly RPC constants in XDR form
 */
extern u_long rpc_reply, rpc_msgdenied, rpc_mismatch, rpc_vers, rpc_auth_unix,
	rpc_msgaccepted, rpc_call, rpc_autherr, rpc_rejectedcred,
	rpc_auth_kerb;
extern u_long nfs_prog, nfs_vers, nqnfs_prog, nqnfs_vers;
extern time_t nqnfsstarttime;
extern int nonidempotent[NFS_NPROCS];

/*
 * Maps errno values to nfs error numbers.
 * Use NFSERR_IO as the catch all for ones not specifically defined in
 * RFC 1094.
 */
#ifdef LITES_ERRNO_BASE
static int nfsrv_errmap[___ELAST] = {
#else
static int nfsrv_errmap[ELAST] = {
#endif
  NFSERR_PERM,	NFSERR_NOENT,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_NXIO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_ACCES,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_EXIST,	NFSERR_IO,	NFSERR_NODEV,	NFSERR_NOTDIR,
  NFSERR_ISDIR,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_FBIG,	NFSERR_NOSPC,	NFSERR_IO,	NFSERR_ROFS,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_NAMETOL,	NFSERR_IO,	NFSERR_IO,
  NFSERR_NOTEMPTY, NFSERR_IO,	NFSERR_IO,	NFSERR_DQUOT,	NFSERR_STALE,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,	NFSERR_IO,
  NFSERR_IO,
};

/*
 * Defines which timer to use for the procnum.
 * 0 - default
 * 1 - getattr
 * 2 - lookup
 * 3 - read
 * 4 - write
 */
static int proct[NFS_NPROCS] = {
	0, 1, 0, 0, 2, 3, 3, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0,
};

/*
 * There is a congestion window for outstanding rpcs maintained per mount
 * point. The cwnd size is adjusted in roughly the way that:
 * Van Jacobson, Congestion avoidance and Control, In "Proceedings of
 * SIGCOMM '88". ACM, August 1988.
 * describes for TCP. The cwnd size is chopped in half on a retransmit timeout
 * and incremented by 1/cwnd when each rpc reply is received and a full cwnd
 * of rpcs is in progress.
 * (The sent count and cwnd are scaled for integer arith.)
 * Variants of "slow start" were tried and were found to be too much of a
 * performance hit (ave. rtt 3 times larger),
 * I suspect due to the large rtt that nfs rpcs have.
 */
#define	NFS_CWNDSCALE	256
#define	NFS_MAXCWND	(NFS_CWNDSCALE * 32)
static int nfs_backoff[8] = { 2, 4, 8, 16, 32, 64, 128, 256, };
int	nfs_sbwait();
void	nfs_disconnect(), nfs_realign(), nfsrv_wakenfsd(), nfs_sndunlock();
void	nfs_rcvunlock(), nqnfs_serverd(), nqnfs_clientlease();
struct mbuf *nfsm_rpchead();
int nfsrtton = 0;
struct nfsrtt nfsrtt;
struct nfsd nfsd_head;

int	nfsrv_null(),
	nfsrv_getattr(),
	nfsrv_setattr(),
	nfsrv_lookup(),
	nfsrv_readlink(),
	nfsrv_read(),
	nfsrv_write(),
	nfsrv_create(),
	nfsrv_remove(),
	nfsrv_rename(),
	nfsrv_link(),
	nfsrv_symlink(),
	nfsrv_mkdir(),
	nfsrv_rmdir(),
	nfsrv_readdir(),
	nfsrv_statfs(),
	nfsrv_noop(),
	nqnfsrv_readdirlook(),
	nqnfsrv_getlease(),
	nqnfsrv_vacated(),
	nqnfsrv_access();

int (*nfsrv_procs[NFS_NPROCS])() = {
	nfsrv_null,
	nfsrv_getattr,
	nfsrv_setattr,
	nfsrv_noop,
	nfsrv_lookup,
	nfsrv_readlink,
	nfsrv_read,
	nfsrv_noop,
	nfsrv_write,
	nfsrv_create,
	nfsrv_remove,
	nfsrv_rename,
	nfsrv_link,
	nfsrv_symlink,
	nfsrv_mkdir,
	nfsrv_rmdir,
	nfsrv_readdir,
	nfsrv_statfs,
	nqnfsrv_readdirlook,
	nqnfsrv_getlease,
	nqnfsrv_vacated,
	nfsrv_noop,
	nqnfsrv_access,
};

struct nfsreq nfsreqh;

/*
 * Initialize sockets and congestion for a new NFS connection.
 * We do not free the sockaddr if error.
 */
nfs_connect(nmp, rep)
	register struct nfsmount *nmp;
	struct nfsreq *rep;
{
	register struct socket *so;
	int s, error, rcvreserve, sndreserve;
	struct sockaddr *saddr;
	struct sockaddr_in *sin;
	struct mbuf *m;
	u_short tport;

	nmp->nm_so = (struct socket *)0;
	saddr = mtod(nmp->nm_nam, struct sockaddr *);
	if (error = socreate(saddr->sa_family,
		&nmp->nm_so, nmp->nm_sotype, nmp->nm_soproto))
		goto bad;
	so = nmp->nm_so;
	nmp->nm_soflags = so->so_proto->pr_flags;

	/*
	 * Some servers require that the client port be a reserved port number.
	 */
	if (saddr->sa_family == AF_INET && (nmp->nm_flag & NFSMNT_RESVPORT)) {
		MGET(m, M_WAIT, MT_SONAME);
		sin = mtod(m, struct sockaddr_in *);
		sin->sin_len = m->m_len = sizeof (struct sockaddr_in);
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = INADDR_ANY;
		tport = IPPORT_RESERVED - 1;
		sin->sin_port = htons(tport);
		while ((error = sobind(so, m)) == EADDRINUSE &&
		       --tport > IPPORT_RESERVED / 2)
			sin->sin_port = htons(tport);
		m_freem(m);
		if (error)
			goto bad;
	}

	/*
	 * Protocols that do not require connections may be optionally left
	 * unconnected for servers that reply from a port other than NFS_PORT.
	 */
	if (nmp->nm_flag & NFSMNT_NOCONN) {
		if (nmp->nm_soflags & PR_CONNREQUIRED) {
			error = ENOTCONN;
			goto bad;
		}
	} else {
		if (error = soconnect(so, nmp->nm_nam))
			goto bad;

		/*
		 * Wait for the connection to complete. Cribbed from the
		 * connect system call but with the wait timing out so
		 * that interruptible mounts don't hang here for a long time.
		 */
		s = splnet();
		while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0) {
			(void) tsleep((caddr_t)&so->so_timeo, PSOCK,
				"nfscon", 2 * hz);
			if ((so->so_state & SS_ISCONNECTING) &&
			    so->so_error == 0 && rep &&
			    (error = nfs_sigintr(nmp, rep, rep->r_procp))) {
				so->so_state &= ~SS_ISCONNECTING;
				splx(s);
				goto bad;
			}
		}
		if (so->so_error) {
			error = so->so_error;
			so->so_error = 0;
			splx(s);
			goto bad;
		}
		splx(s);
	}
	if (nmp->nm_flag & (NFSMNT_SOFT | NFSMNT_INT)) {
		so->so_rcv.sb_timeo = (5 * hz);
		so->so_snd.sb_timeo = (5 * hz);
	} else {
		so->so_rcv.sb_timeo = 0;
		so->so_snd.sb_timeo = 0;
	}
	if (nmp->nm_sotype == SOCK_DGRAM) {
		sndreserve = nmp->nm_wsize + NFS_MAXPKTHDR;
		rcvreserve = nmp->nm_rsize + NFS_MAXPKTHDR;
	} else if (nmp->nm_sotype == SOCK_SEQPACKET) {
		sndreserve = (nmp->nm_wsize + NFS_MAXPKTHDR) * 2;
		rcvreserve = (nmp->nm_rsize + NFS_MAXPKTHDR) * 2;
	} else {
		if (nmp->nm_sotype != SOCK_STREAM)
			panic("nfscon sotype");
		if (so->so_proto->pr_flags & PR_CONNREQUIRED) {
			MGET(m, M_WAIT, MT_SOOPTS);
			*mtod(m, int *) = 1;
			m->m_len = sizeof(int);
			sosetopt(so, SOL_SOCKET, SO_KEEPALIVE, m);
		}
		if (so->so_proto->pr_protocol == IPPROTO_TCP) {
			MGET(m, M_WAIT, MT_SOOPTS);
			*mtod(m, int *) = 1;
			m->m_len = sizeof(int);
			sosetopt(so, IPPROTO_TCP, TCP_NODELAY, m);
		}
		sndreserve = (nmp->nm_wsize + NFS_MAXPKTHDR + sizeof (u_long))
				* 2;
		rcvreserve = (nmp->nm_rsize + NFS_MAXPKTHDR + sizeof (u_long))
				* 2;
	}
	if (error = soreserve(so, sndreserve, rcvreserve))
		goto bad;
	so->so_rcv.sb_flags |= SB_NOINTR;
	so->so_snd.sb_flags |= SB_NOINTR;

	/* Initialize other non-zero congestion variables */
	nmp->nm_srtt[0] = nmp->nm_srtt[1] = nmp->nm_srtt[2] = nmp->nm_srtt[3] =
		nmp->nm_srtt[4] = (NFS_TIMEO << 3);
	nmp->nm_sdrtt[0] = nmp->nm_sdrtt[1] = nmp->nm_sdrtt[2] =
		nmp->nm_sdrtt[3] = nmp->nm_sdrtt[4] = 0;
	nmp->nm_cwnd = NFS_MAXCWND / 2;	    /* Initial send window */
	nmp->nm_sent = 0;
	nmp->nm_timeouts = 0;
	return (0);

bad:
	nfs_disconnect(nmp);
	return (error);
}

/*
 * Reconnect routine:
 * Called when a connection is broken on a reliable protocol.
 * - clean up the old socket
 * - nfs_connect() again
 * - set R_MUSTRESEND for all outstanding requests on mount point
 * If this fails the mount point is DEAD!
 * nb: Must be called with the nfs_sndlock() set on the mount point.
 */
nfs_reconnect(rep)
	register struct nfsreq *rep;
{
	register struct nfsreq *rp;
	register struct nfsmount *nmp = rep->r_nmp;
	int error;

	nfs_disconnect(nmp);
	while (error = nfs_connect(nmp, rep)) {
		if (error == EINTR || error == ERESTART)
			return (EINTR);
		(void) tsleep((caddr_t)&lbolt, PSOCK, "nfscon", 0);
	}

	/*
	 * Loop through outstanding request list and fix up all requests
	 * on old socket.
	 */
	rp = nfsreqh.r_next;
	while (rp != &nfsreqh) {
		if (rp->r_nmp == nmp)
			rp->r_flags |= R_MUSTRESEND;
		rp = rp->r_next;
	}
	return (0);
}

/*
 * NFS disconnect. Clean up and unlink.
 */
void
nfs_disconnect(nmp)
	register struct nfsmount *nmp;
{
	register struct socket *so;

	if (nmp->nm_so) {
		so = nmp->nm_so;
		nmp->nm_so = (struct socket *)0;
		soshutdown(so, 2);
		soclose(so);
	}
}

/*
 * This is the nfs send routine. For connection based socket types, it
 * must be called with an nfs_sndlock() on the socket.
 * "rep == NULL" indicates that it has been called from a server.
 * For the client side:
 * - return EINTR if the RPC is terminated, 0 otherwise
 * - set R_MUSTRESEND if the send fails for any reason
 * - do any cleanup required by recoverable socket errors (???)
 * For the server side:
 * - return EINTR or ERESTART if interrupted by a signal
 * - return EPIPE if a connection is lost for connection based sockets (TCP...)
 * - do any cleanup required by recoverable socket errors (???)
 */
nfs_send(so, nam, top, rep)
	register struct socket *so;
	struct mbuf *nam;
	register struct mbuf *top;
	struct nfsreq *rep;
{
	struct mbuf *sendnam;
	int error, soflags, flags;

	if (rep) {
		if (rep->r_flags & R_SOFTTERM) {
			m_freem(top);
			return (EINTR);
		}
		if ((so = rep->r_nmp->nm_so) == NULL) {
			rep->r_flags |= R_MUSTRESEND;
			m_freem(top);
			return (0);
		}
		rep->r_flags &= ~R_MUSTRESEND;
		soflags = rep->r_nmp->nm_soflags;
	} else
		soflags = so->so_proto->pr_flags;
	if ((soflags & PR_CONNREQUIRED) || (so->so_state & SS_ISCONNECTED))
		sendnam = (struct mbuf *)0;
	else
		sendnam = nam;
	if (so->so_type == SOCK_SEQPACKET)
		flags = MSG_EOR;
	else
		flags = 0;

	error = sosend(so, sendnam, (struct uio *)0, top,
		(struct mbuf *)0, flags);
	if (error) {
		if (rep) {
			log(LOG_INFO, "nfs send error %d for server %s\n",error,
			    rep->r_nmp->nm_mountp->mnt_stat.f_mntfromname);
			/*
			 * Deal with errors for the client side.
			 */
			if (rep->r_flags & R_SOFTTERM)
				error = EINTR;
			else
				rep->r_flags |= R_MUSTRESEND;
		} else
			log(LOG_INFO, "nfsd send error %d\n", error);

		/*
		 * Handle any recoverable (soft) socket errors here. (???)
		 */
		if (error != EINTR && error != ERESTART &&
			error != EWOULDBLOCK && error != EPIPE)
			error = 0;
	}
	return (error);
}

/*
 * Receive a Sun RPC Request/Reply. For SOCK_DGRAM, the work is all
 * done by soreceive(), but for SOCK_STREAM we must deal with the Record
 * Mark and consolidate the data into a new mbuf list.
 * nb: Sometimes TCP passes the data up to soreceive() in long lists of
 *     small mbufs.
 * For SOCK_STREAM we must be very careful to read an entire record once
 * we have read any of it, even if the system call has been interrupted.
 */
nfs_receive(rep, aname, mp)
	register struct nfsreq *rep;
	struct mbuf **aname;
	struct mbuf **mp;
{
	register struct socket *so;
	struct uio auio;
	struct iovec aio;
	register struct mbuf *m;
	struct mbuf *control;
	u_long len;
	struct mbuf **getnam;
	int error, sotype, rcvflg;
	struct proc *p = get_proc();	/* XXX */

	/*
	 * Set up arguments for soreceive()
	 */
	*mp = (struct mbuf *)0;
	*aname = (struct mbuf *)0;
	sotype = rep->r_nmp->nm_sotype;

	/*
	 * For reliable protocols, lock against other senders/receivers
	 * in case a reconnect is necessary.
	 * For SOCK_STREAM, first get the Record Mark to find out how much
	 * more there is to get.
	 * We must lock the socket against other receivers
	 * until we have an entire rpc request/reply.
	 */
	if (sotype != SOCK_DGRAM) {
		if (error = nfs_sndlock(&rep->r_nmp->nm_flag, rep))
			return (error);
tryagain:
		/*
		 * Check for fatal errors and resending request.
		 */
		/*
		 * Ugh: If a reconnect attempt just happened, nm_so
		 * would have changed. NULL indicates a failed
		 * attempt that has essentially shut down this
		 * mount point.
		 */
		if (rep->r_mrep || (rep->r_flags & R_SOFTTERM)) {
			nfs_sndunlock(&rep->r_nmp->nm_flag);
			return (EINTR);
		}
		if ((so = rep->r_nmp->nm_so) == NULL) {
			if (error = nfs_reconnect(rep)) {
				nfs_sndunlock(&rep->r_nmp->nm_flag);
				return (error);
			}
			goto tryagain;
		}
		while (rep->r_flags & R_MUSTRESEND) {
			m = m_copym(rep->r_mreq, 0, M_COPYALL, M_WAIT);
			nfsstats.rpcretries++;
			if (error = nfs_send(so, rep->r_nmp->nm_nam, m, rep)) {
				if (error == EINTR || error == ERESTART ||
				    (error = nfs_reconnect(rep))) {
					nfs_sndunlock(&rep->r_nmp->nm_flag);
					return (error);
				}
				goto tryagain;
			}
		}
		nfs_sndunlock(&rep->r_nmp->nm_flag);
		if (sotype == SOCK_STREAM) {
			aio.iov_base = (caddr_t) &len;
			aio.iov_len = sizeof(u_long);
			auio.uio_iov = &aio;
			auio.uio_iovcnt = 1;
			auio.uio_segflg = UIO_SYSSPACE;
			auio.uio_rw = UIO_READ;
			auio.uio_offset = 0;
			auio.uio_resid = sizeof(u_long);
			auio.uio_procp = p;
			do {
			   rcvflg = MSG_WAITALL;
			   error = soreceive(so, (struct mbuf **)0, &auio,
				(struct mbuf **)0, (struct mbuf **)0, &rcvflg);
			   if (error == EWOULDBLOCK && rep) {
				if (rep->r_flags & R_SOFTTERM)
					return (EINTR);
			   }
			} while (error == EWOULDBLOCK);
			if (!error && auio.uio_resid > 0) {
			    log(LOG_INFO,
				 "short receive (%d/%d) from nfs server %s\n",
				 sizeof(u_long) - auio.uio_resid,
				 sizeof(u_long),
				 rep->r_nmp->nm_mountp->mnt_stat.f_mntfromname);
			    error = EPIPE;
			}
			if (error)
				goto errout;
			len = ntohl(len) & ~0x80000000;
			/*
			 * This is SERIOUS! We are out of sync with the sender
			 * and forcing a disconnect/reconnect is all I can do.
			 */
			if (len > NFS_MAXPACKET) {
			    log(LOG_ERR, "%s (%d) from nfs server %s\n",
				"impossible packet length",
				len,
				rep->r_nmp->nm_mountp->mnt_stat.f_mntfromname);
			    error = EFBIG;
			    goto errout;
			}
			auio.uio_resid = len;
			do {
			    rcvflg = MSG_WAITALL;
			    error =  soreceive(so, (struct mbuf **)0,
				&auio, mp, (struct mbuf **)0, &rcvflg);
			} while (error == EWOULDBLOCK || error == EINTR ||
				 error == ERESTART);
			if (!error && auio.uio_resid > 0) {
			    log(LOG_INFO,
				"short receive (%d/%d) from nfs server %s\n",
				len - auio.uio_resid, len,
				rep->r_nmp->nm_mountp->mnt_stat.f_mntfromname);
			    error = EPIPE;
			}
		} else {
			/*
			 * NB: Since uio_resid is big, MSG_WAITALL is ignored
			 * and soreceive() will return when it has either a
			 * control msg or a data msg.
			 * We have no use for control msg., but must grab them
			 * and then throw them away so we know what is going
			 * on.
			 */
			auio.uio_resid = len = 100000000; /* Anything Big */
			auio.uio_procp = p;
			do {
			    rcvflg = 0;
			    error =  soreceive(so, (struct mbuf **)0,
				&auio, mp, &control, &rcvflg);
			    if (control)
				m_freem(control);
			    if (error == EWOULDBLOCK && rep) {
				if (rep->r_flags & R_SOFTTERM)
					return (EINTR);
			    }
			} while (error == EWOULDBLOCK ||
				 (!error && *mp == NULL && control));
			if ((rcvflg & MSG_EOR) == 0)
				printf("Egad!!\n");
			if (!error && *mp == NULL)
				error = EPIPE;
			len -= auio.uio_resid;
		}
errout:
		if (error && error != EINTR && error != ERESTART) {
			m_freem(*mp);
			*mp = (struct mbuf *)0;
			if (error != EPIPE)
				log(LOG_INFO,
				    "receive error %d from nfs server %s\n",
				    error,
				 rep->r_nmp->nm_mountp->mnt_stat.f_mntfromname);
			error = nfs_sndlock(&rep->r_nmp->nm_flag, rep);
			if (!error)
				error = nfs_reconnect(rep);
			if (!error)
				goto tryagain;
		}
	} else {
		if ((so = rep->r_nmp->nm_so) == NULL)
			return (EACCES);
		if (so->so_state & SS_ISCONNECTED)
			getnam = (struct mbuf **)0;
		else
			getnam = aname;
		auio.uio_resid = len = 1000000;
		auio.uio_procp = p;
		do {
			rcvflg = 0;
			error =  soreceive(so, getnam, &auio, mp,
				(struct mbuf **)0, &rcvflg);
			if (error == EWOULDBLOCK &&
			    (rep->r_flags & R_SOFTTERM))
				return (EINTR);
		} while (error == EWOULDBLOCK);
		len -= auio.uio_resid;
	}
	if (error) {
		m_freem(*mp);
		*mp = (struct mbuf *)0;
	}
	/*
	 * Search for any mbufs that are not a multiple of 4 bytes long
	 * or with m_data not longword aligned.
	 * These could cause pointer alignment problems, so copy them to
	 * well aligned mbufs.
	 */
	nfs_realign(*mp, 5 * NFSX_UNSIGNED);
	return (error);
}

/*
 * Implement receipt of reply on a socket.
 * We must search through the list of received datagrams matching them
 * with outstanding requests using the xid, until ours is found.
 */
/* ARGSUSED */
nfs_reply(myrep)
	struct nfsreq *myrep;
{
	register struct nfsreq *rep;
	register struct nfsmount *nmp = myrep->r_nmp;
	register long t1;
	struct mbuf *mrep, *nam, *md;
	u_long rxid, *tl;
	caddr_t dpos, cp2;
	int error;

	/*
	 * Loop around until we get our own reply
	 */
	for (;;) {
		/*
		 * Lock against other receivers so that I don't get stuck in
		 * sbwait() after someone else has received my reply for me.
		 * Also necessary for connection based protocols to avoid
		 * race conditions during a reconnect.
		 */
		if (error = nfs_rcvlock(myrep))
			return (error);
		/* Already received, bye bye */
		if (myrep->r_mrep != NULL) {
			nfs_rcvunlock(&nmp->nm_flag);
			return (0);
		}
		/*
		 * Get the next Rpc reply off the socket
		 */
		error = nfs_receive(myrep, &nam, &mrep);
		nfs_rcvunlock(&nmp->nm_flag);
		if (error) {

			/*
			 * Ignore routing errors on connectionless protocols??
			 */
			if (NFSIGNORE_SOERROR(nmp->nm_soflags, error)) {
				nmp->nm_so->so_error = 0;
				if (myrep->r_flags & R_GETONEREP)
					return (0);
				continue;
			}
			return (error);
		}
		if (nam)
			m_freem(nam);
	
		/*
		 * Get the xid and check that it is an rpc reply
		 */
		md = mrep;
		dpos = mtod(md, caddr_t);
		nfsm_dissect(tl, u_long *, 2*NFSX_UNSIGNED);
		rxid = *tl++;
		if (*tl != rpc_reply) {
			if (nmp->nm_flag & NFSMNT_NQNFS) {
				if (nqnfs_callback(nmp, mrep, md, dpos))
					nfsstats.rpcinvalid++;
			} else {
				nfsstats.rpcinvalid++;
				m_freem(mrep);
			}
nfsmout:
			if (myrep->r_flags & R_GETONEREP)
				return (0);
			continue;
		}

		/*
		 * Loop through the request list to match up the reply
		 * Iff no match, just drop the datagram
		 */
		rep = nfsreqh.r_next;
		while (rep != &nfsreqh) {
			if (rep->r_mrep == NULL && rxid == rep->r_xid) {
				/* Found it.. */
				rep->r_mrep = mrep;
				rep->r_md = md;
				rep->r_dpos = dpos;
				if (nfsrtton) {
					struct rttl *rt;

					rt = &nfsrtt.rttl[nfsrtt.pos];
					rt->proc = rep->r_procnum;
					rt->rto = NFS_RTO(nmp, proct[rep->r_procnum]);
					rt->sent = nmp->nm_sent;
					rt->cwnd = nmp->nm_cwnd;
					rt->srtt = nmp->nm_srtt[proct[rep->r_procnum] - 1];
					rt->sdrtt = nmp->nm_sdrtt[proct[rep->r_procnum] - 1];
					rt->fsid = nmp->nm_mountp->mnt_stat.f_fsid;
					get_time(&rt->tstamp);
					if (rep->r_flags & R_TIMING)
						rt->rtt = rep->r_rtt;
					else
						rt->rtt = 1000000;
					nfsrtt.pos = (nfsrtt.pos + 1) % NFSRTTLOGSIZ;
				}
				/*
				 * Update congestion window.
				 * Do the additive increase of
				 * one rpc/rtt.
				 */
				if (nmp->nm_cwnd <= nmp->nm_sent) {
					nmp->nm_cwnd +=
					   (NFS_CWNDSCALE * NFS_CWNDSCALE +
					   (nmp->nm_cwnd >> 1)) / nmp->nm_cwnd;
					if (nmp->nm_cwnd > NFS_MAXCWND)
						nmp->nm_cwnd = NFS_MAXCWND;
				}
				rep->r_flags &= ~R_SENT;
				nmp->nm_sent -= NFS_CWNDSCALE;
				/*
				 * Update rtt using a gain of 0.125 on the mean
				 * and a gain of 0.25 on the deviation.
				 */
				if (rep->r_flags & R_TIMING) {
					/*
					 * Since the timer resolution of
					 * NFS_HZ is so course, it can often
					 * result in r_rtt == 0. Since
					 * r_rtt == N means that the actual
					 * rtt is between N+dt and N+2-dt ticks,
					 * add 1.
					 */
					t1 = rep->r_rtt + 1;
					t1 -= (NFS_SRTT(rep) >> 3);
					NFS_SRTT(rep) += t1;
					if (t1 < 0)
						t1 = -t1;
					t1 -= (NFS_SDRTT(rep) >> 2);
					NFS_SDRTT(rep) += t1;
				}
				nmp->nm_timeouts = 0;
				break;
			}
			rep = rep->r_next;
		}
		/*
		 * If not matched to a request, drop it.
		 * If it's mine, get out.
		 */
		if (rep == &nfsreqh) {
			nfsstats.rpcunexpected++;
			m_freem(mrep);
		} else if (rep == myrep) {
			if (rep->r_mrep == NULL)
				panic("nfsreply nil");
			return (0);
		}
		if (myrep->r_flags & R_GETONEREP)
			return (0);
	}
}

/*
 * nfs_request - goes something like this
 *	- fill in request struct
 *	- links it into list
 *	- calls nfs_send() for first transmit
 *	- calls nfs_receive() to get reply
 *	- break down rpc header and return with nfs reply pointed to
 *	  by mrep or error
 * nb: always frees up mreq mbuf list
 */
nfs_request(vp, mrest, procnum, procp, cred, mrp, mdp, dposp)
	struct vnode *vp;
	struct mbuf *mrest;
	int procnum;
	struct proc *procp;
	struct ucred *cred;
	struct mbuf **mrp;
	struct mbuf **mdp;
	caddr_t *dposp;
{
	register struct mbuf *m, *mrep;
	register struct nfsreq *rep;
	register u_long *tl;
	register int i;
	struct nfsmount *nmp;
	struct mbuf *md, *mheadend;
	struct nfsreq *reph;
	struct nfsnode *np;
	time_t reqtime, waituntil;
	caddr_t dpos, cp2;
	int t1, nqlflag, cachable, s, error = 0, mrest_len, auth_len, auth_type;
	int trylater_delay = NQ_TRYLATERDEL, trylater_cnt = 0, failed_auth = 0;
	u_long xid;
	u_quad_t frev;
	char *auth_str;

	nmp = VFSTONFS(vp->v_mount);
	MALLOC(rep, struct nfsreq *, sizeof(struct nfsreq), M_NFSREQ, M_WAITOK);
	rep->r_nmp = nmp;
	rep->r_vp = vp;
	rep->r_procp = procp;
	rep->r_procnum = procnum;
	i = 0;
	m = mrest;
	while (m) {
		i += m->m_len;
		m = m->m_next;
	}
	mrest_len = i;

	/*
	 * Get the RPC header with authorization.
	 */
kerbauth:
	auth_str = (char *)0;
	if (nmp->nm_flag & NFSMNT_KERB) {
		if (failed_auth) {
			error = nfs_getauth(nmp, rep, cred, &auth_type,
				&auth_str, &auth_len);
			if (error) {
				free((caddr_t)rep, M_NFSREQ);
				m_freem(mrest);
				return (error);
			}
		} else {
			auth_type = RPCAUTH_UNIX;
			auth_len = 5 * NFSX_UNSIGNED;
		}
	} else {
		auth_type = RPCAUTH_UNIX;
		if (cred->cr_ngroups < 1)
			panic("nfsreq nogrps");
		auth_len = ((((cred->cr_ngroups - 1) > nmp->nm_numgrps) ?
			nmp->nm_numgrps : (cred->cr_ngroups - 1)) << 2) +
			5 * NFSX_UNSIGNED;
	}
	m = nfsm_rpchead(cred, (nmp->nm_flag & NFSMNT_NQNFS), procnum,
	     auth_type, auth_len, auth_str, mrest, mrest_len, &mheadend, &xid);
	if (auth_str)
		free(auth_str, M_TEMP);

	/*
	 * For stream protocols, insert a Sun RPC Record Mark.
	 */
	if (nmp->nm_sotype == SOCK_STREAM) {
		M_PREPEND(m, NFSX_UNSIGNED, M_WAIT);
		*mtod(m, u_long *) = htonl(0x80000000 |
			 (m->m_pkthdr.len - NFSX_UNSIGNED));
	}
	rep->r_mreq = m;
	rep->r_xid = xid;
tryagain:
	if (nmp->nm_flag & NFSMNT_SOFT)
		rep->r_retry = nmp->nm_retry;
	else
		rep->r_retry = NFS_MAXREXMIT + 1;	/* past clip limit */
	rep->r_rtt = rep->r_rexmit = 0;
	if (proct[procnum] > 0)
		rep->r_flags = R_TIMING;
	else
		rep->r_flags = 0;
	rep->r_mrep = NULL;

	/*
	 * Do the client side RPC.
	 */
	nfsstats.rpcrequests++;
	/*
	 * Chain request into list of outstanding requests. Be sure
	 * to put it LAST so timer finds oldest requests first.
	 */
	s = splsoftclock();
	reph = &nfsreqh;
	reph->r_prev->r_next = rep;
	rep->r_prev = reph->r_prev;
	reph->r_prev = rep;
	rep->r_next = reph;

	/* Get send time for nqnfs */
	reqtime = get_seconds();

	/*
	 * If backing off another request or avoiding congestion, don't
	 * send this one now but let timer do it. If not timing a request,
	 * do it now.
	 */
	if (nmp->nm_so && (nmp->nm_sotype != SOCK_DGRAM ||
		(nmp->nm_flag & NFSMNT_DUMBTIMR) ||
		nmp->nm_sent < nmp->nm_cwnd)) {
		splx(s);
		if (nmp->nm_soflags & PR_CONNREQUIRED)
			error = nfs_sndlock(&nmp->nm_flag, rep);
		if (!error) {
			m = m_copym(m, 0, M_COPYALL, M_WAIT);
			error = nfs_send(nmp->nm_so, nmp->nm_nam, m, rep);
			if (nmp->nm_soflags & PR_CONNREQUIRED)
				nfs_sndunlock(&nmp->nm_flag);
		}
		if (!error && (rep->r_flags & R_MUSTRESEND) == 0) {
			nmp->nm_sent += NFS_CWNDSCALE;
			rep->r_flags |= R_SENT;
		}
	} else {
		splx(s);
		rep->r_rtt = -1;
	}

	/*
	 * Wait for the reply from our send or the timer's.
	 */
	if (!error || error == EPIPE)
		error = nfs_reply(rep);

	/*
	 * RPC done, unlink the request.
	 */
	s = splsoftclock();
	rep->r_prev->r_next = rep->r_next;
	rep->r_next->r_prev = rep->r_prev;
	splx(s);

	/*
	 * Decrement the outstanding request count.
	 */
	if (rep->r_flags & R_SENT) {
		rep->r_flags &= ~R_SENT;	/* paranoia */
		nmp->nm_sent -= NFS_CWNDSCALE;
	}

	/*
	 * If there was a successful reply and a tprintf msg.
	 * tprintf a response.
	 */
	if (!error && (rep->r_flags & R_TPRINTFMSG))
		nfs_msg(rep->r_procp, nmp->nm_mountp->mnt_stat.f_mntfromname,
		    "is alive again");
	mrep = rep->r_mrep;
	md = rep->r_md;
	dpos = rep->r_dpos;
	if (error) {
		m_freem(rep->r_mreq);
		free((caddr_t)rep, M_NFSREQ);
		return (error);
	}

	/*
	 * break down the rpc header and check if ok
	 */
	nfsm_dissect(tl, u_long *, 3*NFSX_UNSIGNED);
	if (*tl++ == rpc_msgdenied) {
		if (*tl == rpc_mismatch)
			error = EOPNOTSUPP;
		else if ((nmp->nm_flag & NFSMNT_KERB) && *tl++ == rpc_autherr) {
			if (*tl == rpc_rejectedcred && failed_auth == 0) {
				failed_auth++;
				mheadend->m_next = (struct mbuf *)0;
				m_freem(mrep);
				m_freem(rep->r_mreq);
				goto kerbauth;
			} else
				error = EAUTH;
		} else
			error = EACCES;
		m_freem(mrep);
		m_freem(rep->r_mreq);
		free((caddr_t)rep, M_NFSREQ);
		return (error);
	}

	/*
	 * skip over the auth_verf, someday we may want to cache auth_short's
	 * for nfs_reqhead(), but for now just dump it
	 */
	if (*++tl != 0) {
		i = nfsm_rndup(fxdr_unsigned(long, *tl));
		nfsm_adv(i);
	}
	nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
	/* 0 == ok */
	if (*tl == 0) {
		nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
		if (*tl != 0) {
			error = fxdr_unsigned(int, *tl);
#ifdef LITES_ERRNO_BASE
			if (error > 0 && error <= ___ELAST)
				error += LITES_ERRNO_BASE;
#endif
			m_freem(mrep);
			if ((nmp->nm_flag & NFSMNT_NQNFS) &&
			    error == NQNFS_TRYLATER) {
				error = 0;
				waituntil = get_seconds() + trylater_delay;
				while (get_seconds() < waituntil)
					(void) tsleep((caddr_t)&lbolt,
						PSOCK, "nqnfstry", 0);
				trylater_delay *= nfs_backoff[trylater_cnt];
				if (trylater_cnt < 7)
					trylater_cnt++;
				goto tryagain;
			}

			/*
			 * If the File Handle was stale, invalidate the
			 * lookup cache, just in case.
			 */
			if (error == ESTALE)
				cache_purge(vp);
			m_freem(rep->r_mreq);
			free((caddr_t)rep, M_NFSREQ);
			return (error);
		}

		/*
		 * For nqnfs, get any lease in reply
		 */
		if (nmp->nm_flag & NFSMNT_NQNFS) {
			nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			if (*tl) {
				np = VTONFS(vp);
				nqlflag = fxdr_unsigned(int, *tl);
				nfsm_dissect(tl, u_long *, 4*NFSX_UNSIGNED);
				cachable = fxdr_unsigned(int, *tl++);
				reqtime += fxdr_unsigned(int, *tl++);
				if (reqtime > get_seconds()) {
				    fxdr_hyper(tl, &frev);
				    nqnfs_clientlease(nmp, np, nqlflag,
					cachable, reqtime, frev);
				}
			}
		}
		*mrp = mrep;
		*mdp = md;
		*dposp = dpos;
		m_freem(rep->r_mreq);
		FREE((caddr_t)rep, M_NFSREQ);
		return (0);
	}
	m_freem(mrep);
	m_freem(rep->r_mreq);
	free((caddr_t)rep, M_NFSREQ);
	error = EPROTONOSUPPORT;
nfsmout:
	return (error);
}

/*
 * Generate the rpc reply header
 * siz arg. is used to decide if adding a cluster is worthwhile
 */
nfs_rephead(siz, nd, err, cache, frev, mrq, mbp, bposp)
	int siz;
	struct nfsd *nd;
	int err;
	int cache;
	u_quad_t *frev;
	struct mbuf **mrq;
	struct mbuf **mbp;
	caddr_t *bposp;
{
	register u_long *tl;
	register struct mbuf *mreq;
	caddr_t bpos;
	struct mbuf *mb, *mb2;

	MGETHDR(mreq, M_WAIT, MT_DATA);
	mb = mreq;
	/*
	 * If this is a big reply, use a cluster else
	 * try and leave leading space for the lower level headers.
	 */
	siz += RPC_REPLYSIZ;
	if (siz >= MINCLSIZE) {
		MCLGET(mreq, M_WAIT);
	} else
		mreq->m_data += max_hdr;
	tl = mtod(mreq, u_long *);
	mreq->m_len = 6*NFSX_UNSIGNED;
	bpos = ((caddr_t)tl)+mreq->m_len;
	*tl++ = nd->nd_retxid;
	*tl++ = rpc_reply;
#ifdef LITES_ERRNO_BASE
	if (err > 0 && err <= ___ELAST)
		printf("nfs_rephead: unmapped error code %d\n", err);
#endif
	if (err == ERPCMISMATCH || err == NQNFS_AUTHERR) {
		*tl++ = rpc_msgdenied;
		if (err == NQNFS_AUTHERR) {
			*tl++ = rpc_autherr;
			*tl = rpc_rejectedcred;
			mreq->m_len -= NFSX_UNSIGNED;
			bpos -= NFSX_UNSIGNED;
		} else {
			*tl++ = rpc_mismatch;
			*tl++ = txdr_unsigned(2);
			*tl = txdr_unsigned(2);
		}
	} else {
		*tl++ = rpc_msgaccepted;
		*tl++ = 0;
		*tl++ = 0;
		switch (err) {
		case EPROGUNAVAIL:
			*tl = txdr_unsigned(RPC_PROGUNAVAIL);
			break;
		case EPROGMISMATCH:
			*tl = txdr_unsigned(RPC_PROGMISMATCH);
			nfsm_build(tl, u_long *, 2*NFSX_UNSIGNED);
			*tl++ = txdr_unsigned(2);
			*tl = txdr_unsigned(2);	/* someday 3 */
			break;
		case EPROCUNAVAIL:
			*tl = txdr_unsigned(RPC_PROCUNAVAIL);
			break;
		default:
			*tl = 0;
			if (err != VNOVAL) {
				nfsm_build(tl, u_long *, NFSX_UNSIGNED);
#ifdef LITES_ERRNO_BASE
			if (err >= LITES_ERRNO_BASE)
				err -= LITES_ERRNO_BASE;
#endif
				if (err)
					*tl = txdr_unsigned(nfsrv_errmap[err - 1]);
				else
					*tl = 0;
			}
			break;
		};
	}

	/*
	 * For nqnfs, piggyback lease as requested.
	 */
	if (nd->nd_nqlflag != NQL_NOVAL && err == 0) {
		if (nd->nd_nqlflag) {
			nfsm_build(tl, u_long *, 5*NFSX_UNSIGNED);
			*tl++ = txdr_unsigned(nd->nd_nqlflag);
			*tl++ = txdr_unsigned(cache);
			*tl++ = txdr_unsigned(nd->nd_duration);
			txdr_hyper(frev, tl);
		} else {
			if (nd->nd_nqlflag != 0)
				panic("nqreph");
			nfsm_build(tl, u_long *, NFSX_UNSIGNED);
			*tl = 0;
		}
	}
	*mrq = mreq;
	*mbp = mb;
	*bposp = bpos;
	if (err != 0 && err != VNOVAL)
		nfsstats.srvrpc_errs++;
	return (0);
}

/*
 * Nfs timer routine
 * Scan the nfsreq list and retranmit any requests that have timed out
 * To avoid retransmission attempts on STREAM sockets (in the future) make
 * sure to set the r_retry field to 0 (implies nm_retry == 0).
 */
void
nfs_timer(arg)
	void *arg;
{
	register struct nfsreq *rep;
	register struct mbuf *m;
	register struct socket *so;
	register struct nfsmount *nmp;
	register int timeo;
	static long lasttime = 0;
	int s, error;

	s = splnet();
	for (rep = nfsreqh.r_next; rep != &nfsreqh; rep = rep->r_next) {
		nmp = rep->r_nmp;
		if (rep->r_mrep || (rep->r_flags & R_SOFTTERM))
			continue;
		if (nfs_sigintr(nmp, rep, rep->r_procp)) {
			rep->r_flags |= R_SOFTTERM;
			continue;
		}
		if (rep->r_rtt >= 0) {
			rep->r_rtt++;
			if (nmp->nm_flag & NFSMNT_DUMBTIMR)
				timeo = nmp->nm_timeo;
			else
				timeo = NFS_RTO(nmp, proct[rep->r_procnum]);
			if (nmp->nm_timeouts > 0)
				timeo *= nfs_backoff[nmp->nm_timeouts - 1];
			if (rep->r_rtt <= timeo)
				continue;
			if (nmp->nm_timeouts < 8)
				nmp->nm_timeouts++;
		}
		/*
		 * Check for server not responding
		 */
		if ((rep->r_flags & R_TPRINTFMSG) == 0 &&
		     rep->r_rexmit > nmp->nm_deadthresh) {
			nfs_msg(rep->r_procp,
			    nmp->nm_mountp->mnt_stat.f_mntfromname,
			    "not responding");
			rep->r_flags |= R_TPRINTFMSG;
		}
		if (rep->r_rexmit >= rep->r_retry) {	/* too many */
			nfsstats.rpctimeouts++;
			rep->r_flags |= R_SOFTTERM;
			continue;
		}
		if (nmp->nm_sotype != SOCK_DGRAM) {
			if (++rep->r_rexmit > NFS_MAXREXMIT)
				rep->r_rexmit = NFS_MAXREXMIT;
			continue;
		}
		if ((so = nmp->nm_so) == NULL)
			continue;

		/*
		 * If there is enough space and the window allows..
		 *	Resend it
		 * Set r_rtt to -1 in case we fail to send it now.
		 */
		rep->r_rtt = -1;
		if (sbspace(&so->so_snd) >= rep->r_mreq->m_pkthdr.len &&
		   ((nmp->nm_flag & NFSMNT_DUMBTIMR) ||
		    (rep->r_flags & R_SENT) ||
		    nmp->nm_sent < nmp->nm_cwnd) &&
		   (m = m_copym(rep->r_mreq, 0, M_COPYALL, M_DONTWAIT))){
			if ((nmp->nm_flag & NFSMNT_NOCONN) == 0)
			    error = (*so->so_proto->pr_usrreq)(so, PRU_SEND, m,
			    (struct mbuf *)0, (struct mbuf *)0);
			else
			    error = (*so->so_proto->pr_usrreq)(so, PRU_SEND, m,
			    nmp->nm_nam, (struct mbuf *)0);
			if (error) {
				if (NFSIGNORE_SOERROR(nmp->nm_soflags, error))
					so->so_error = 0;
			} else {
				/*
				 * Iff first send, start timing
				 * else turn timing off, backoff timer
				 * and divide congestion window by 2.
				 */
				if (rep->r_flags & R_SENT) {
					rep->r_flags &= ~R_TIMING;
					if (++rep->r_rexmit > NFS_MAXREXMIT)
						rep->r_rexmit = NFS_MAXREXMIT;
					nmp->nm_cwnd >>= 1;
					if (nmp->nm_cwnd < NFS_CWNDSCALE)
						nmp->nm_cwnd = NFS_CWNDSCALE;
					nfsstats.rpcretries++;
				} else {
					rep->r_flags |= R_SENT;
					nmp->nm_sent += NFS_CWNDSCALE;
				}
				rep->r_rtt = 0;
			}
		}
	}

	/*
	 * Call the nqnfs server timer once a second to handle leases.
	 */
	{
		struct timeval time;
		get_time(&time);
		if (lasttime != time.tv_sec) {
			lasttime = time.tv_sec;
			nqnfs_serverd();
		}
	}
	splx(s);
	timeout(nfs_timer, (void *)0, hz / NFS_HZ);
}

/*
 * Test for a termination condition pending on the process.
 * This is used for NFSMNT_INT mounts.
 */
nfs_sigintr(nmp, rep, p)
	struct nfsmount *nmp;
	struct nfsreq *rep;
	register struct proc *p;
{

	if (rep && (rep->r_flags & R_SOFTTERM))
		return (EINTR);
	if (!(nmp->nm_flag & NFSMNT_INT))
		return (0);
	if (p && p->p_siglist &&
	    (((p->p_siglist & ~p->p_sigmask) & ~p->p_sigignore) &
	    NFSINT_SIGMASK))
		return (EINTR);
	return (0);
}

/*
 * Lock a socket against others.
 * Necessary for STREAM sockets to ensure you get an entire rpc request/reply
 * and also to avoid race conditions between the processes with nfs requests
 * in progress when a reconnect is necessary.
 */
nfs_sndlock(flagp, rep)
	register int *flagp;
	struct nfsreq *rep;
{
	struct proc *p;
	int slpflag = 0, slptimeo = 0;

	if (rep) {
		p = rep->r_procp;
		if (rep->r_nmp->nm_flag & NFSMNT_INT)
			slpflag = PCATCH;
	} else
		p = (struct proc *)0;
	while (*flagp & NFSMNT_SNDLOCK) {
		if (nfs_sigintr(rep->r_nmp, rep, p))
			return (EINTR);
		*flagp |= NFSMNT_WANTSND;
		(void) tsleep((caddr_t)flagp, slpflag | (PZERO - 1), "nfsndlck",
			slptimeo);
		if (slpflag == PCATCH) {
			slpflag = 0;
			slptimeo = 2 * hz;
		}
	}
	*flagp |= NFSMNT_SNDLOCK;
	return (0);
}

/*
 * Unlock the stream socket for others.
 */
void
nfs_sndunlock(flagp)
	register int *flagp;
{

	if ((*flagp & NFSMNT_SNDLOCK) == 0)
		panic("nfs sndunlock");
	*flagp &= ~NFSMNT_SNDLOCK;
	if (*flagp & NFSMNT_WANTSND) {
		*flagp &= ~NFSMNT_WANTSND;
		wakeup((caddr_t)flagp);
	}
}

nfs_rcvlock(rep)
	register struct nfsreq *rep;
{
	register int *flagp = &rep->r_nmp->nm_flag;
	int slpflag, slptimeo = 0;

	if (*flagp & NFSMNT_INT)
		slpflag = PCATCH;
	else
		slpflag = 0;
	while (*flagp & NFSMNT_RCVLOCK) {
		if (nfs_sigintr(rep->r_nmp, rep, rep->r_procp))
			return (EINTR);
		*flagp |= NFSMNT_WANTRCV;
		(void) tsleep((caddr_t)flagp, slpflag | (PZERO - 1), "nfsrcvlk",
			slptimeo);
		if (slpflag == PCATCH) {
			slpflag = 0;
			slptimeo = 2 * hz;
		}
	}
	*flagp |= NFSMNT_RCVLOCK;
	return (0);
}

/*
 * Unlock the stream socket for others.
 */
void
nfs_rcvunlock(flagp)
	register int *flagp;
{

	if ((*flagp & NFSMNT_RCVLOCK) == 0)
		panic("nfs rcvunlock");
	*flagp &= ~NFSMNT_RCVLOCK;
	if (*flagp & NFSMNT_WANTRCV) {
		*flagp &= ~NFSMNT_WANTRCV;
		wakeup((caddr_t)flagp);
	}
}

/*
 * Check for badly aligned mbuf data areas and
 * realign data in an mbuf list by copying the data areas up, as required.
 */
void
nfs_realign(m, hsiz)
	register struct mbuf *m;
	int hsiz;
{
	register struct mbuf *m2;
	register int siz, mlen, olen;
	register caddr_t tcp, fcp;
	struct mbuf *mnew;

	while (m) {
	    /*
	     * This never happens for UDP, rarely happens for TCP
	     * but frequently happens for iso transport.
	     */
	    if ((m->m_len & 0x3) || (mtod(m, integer_t) & 0x3)) {
		olen = m->m_len;
		fcp = mtod(m, caddr_t);
		if ((integer_t)fcp & 0x3) {
			m->m_flags &= ~M_PKTHDR;
			if (m->m_flags & M_EXT)
				m->m_data = m->m_ext.ext_buf +
					((m->m_ext.ext_size - olen) & ~0x3);
			else
				m->m_data = m->m_dat;
		}
		m->m_len = 0;
		tcp = mtod(m, caddr_t);
		mnew = m;
		m2 = m->m_next;
	
		/*
		 * If possible, only put the first invariant part
		 * of the RPC header in the first mbuf.
		 */
		mlen = M_TRAILINGSPACE(m);
		if (olen <= hsiz && mlen > hsiz)
			mlen = hsiz;
	
		/*
		 * Loop through the mbuf list consolidating data.
		 */
		while (m) {
			while (olen > 0) {
				if (mlen == 0) {
					m2->m_flags &= ~M_PKTHDR;
					if (m2->m_flags & M_EXT)
						m2->m_data = m2->m_ext.ext_buf;
					else
						m2->m_data = m2->m_dat;
					m2->m_len = 0;
					mlen = M_TRAILINGSPACE(m2);
					tcp = mtod(m2, caddr_t);
					mnew = m2;
					m2 = m2->m_next;
				}
				siz = min(mlen, olen);
				if (tcp != fcp)
					memcpy(tcp, fcp, siz);
				mnew->m_len += siz;
				mlen -= siz;
				olen -= siz;
				tcp += siz;
				fcp += siz;
			}
			m = m->m_next;
			if (m) {
				olen = m->m_len;
				fcp = mtod(m, caddr_t);
			}
		}
	
		/*
		 * Finally, set m_len == 0 for any trailing mbufs that have
		 * been copied out of.
		 */
		while (m2) {
			m2->m_len = 0;
			m2 = m2->m_next;
		}
		return;
	    }
	    m = m->m_next;
	}
}

/*
 * Socket upcall routine for the nfsd sockets.
 * The caddr_t arg is a pointer to the "struct nfssvc_sock".
 * Essentially do as much as possible non-blocking, else punt and it will
 * be called with M_WAIT from an nfsd.
 */
void
nfsrv_rcv(so, arg, waitflag)
	struct socket *so;
	caddr_t arg;
	int waitflag;
{
	register struct nfssvc_sock *slp = (struct nfssvc_sock *)arg;
	register struct mbuf *m;
	struct mbuf *mp, *nam;
	struct uio auio;
	int flags, error;

	if ((slp->ns_flag & SLP_VALID) == 0)
		return;
#ifdef notdef
	/*
	 * Define this to test for nfsds handling this under heavy load.
	 */
	if (waitflag == M_DONTWAIT) {
		slp->ns_flag |= SLP_NEEDQ; goto dorecs;
	}
#endif
	auio.uio_procp = NULL;
	if (so->so_type == SOCK_STREAM) {
		/*
		 * If there are already records on the queue, defer soreceive()
		 * to an nfsd so that there is feedback to the TCP layer that
		 * the nfs servers are heavily loaded.
		 */
		if (slp->ns_rec && waitflag == M_DONTWAIT) {
			slp->ns_flag |= SLP_NEEDQ;
			goto dorecs;
		}

		/*
		 * Do soreceive().
		 */
		auio.uio_resid = 1000000000;
		flags = MSG_DONTWAIT;
		error = soreceive(so, &nam, &auio, &mp, (struct mbuf **)0, &flags);
		if (error || mp == (struct mbuf *)0) {
			if (error == EWOULDBLOCK)
				slp->ns_flag |= SLP_NEEDQ;
			else
				slp->ns_flag |= SLP_DISCONN;
			goto dorecs;
		}
		m = mp;
		if (slp->ns_rawend) {
			slp->ns_rawend->m_next = m;
			slp->ns_cc += 1000000000 - auio.uio_resid;
		} else {
			slp->ns_raw = m;
			slp->ns_cc = 1000000000 - auio.uio_resid;
		}
		while (m->m_next)
			m = m->m_next;
		slp->ns_rawend = m;

		/*
		 * Now try and parse record(s) out of the raw stream data.
		 */
		if (error = nfsrv_getstream(slp, waitflag)) {
			if (error == EPERM)
				slp->ns_flag |= SLP_DISCONN;
			else
				slp->ns_flag |= SLP_NEEDQ;
		}
	} else {
		do {
			auio.uio_resid = 1000000000;
			flags = MSG_DONTWAIT;
			error = soreceive(so, &nam, &auio, &mp,
						(struct mbuf **)0, &flags);
			if (mp) {
				nfs_realign(mp, 10 * NFSX_UNSIGNED);
				if (nam) {
					m = nam;
					m->m_next = mp;
				} else
					m = mp;
				if (slp->ns_recend)
					slp->ns_recend->m_nextpkt = m;
				else
					slp->ns_rec = m;
				slp->ns_recend = m;
				m->m_nextpkt = (struct mbuf *)0;
			}
			if (error) {
				if ((so->so_proto->pr_flags & PR_CONNREQUIRED)
					&& error != EWOULDBLOCK) {
					slp->ns_flag |= SLP_DISCONN;
					goto dorecs;
				}
			}
		} while (mp);
	}

	/*
	 * Now try and process the request records, non-blocking.
	 */
dorecs:
	if (waitflag == M_DONTWAIT &&
		(slp->ns_rec || (slp->ns_flag & (SLP_NEEDQ | SLP_DISCONN))))
		nfsrv_wakenfsd(slp);
}

/*
 * Try and extract an RPC request from the mbuf data list received on a
 * stream socket. The "waitflag" argument indicates whether or not it
 * can sleep.
 */
nfsrv_getstream(slp, waitflag)
	register struct nfssvc_sock *slp;
	int waitflag;
{
	register struct mbuf *m;
	register char *cp1, *cp2;
	register int len;
	struct mbuf *om, *m2, *recm;
	u_long recmark;

	if (slp->ns_flag & SLP_GETSTREAM)
		panic("nfs getstream");
	slp->ns_flag |= SLP_GETSTREAM;
	for (;;) {
	    if (slp->ns_reclen == 0) {
		if (slp->ns_cc < NFSX_UNSIGNED) {
			slp->ns_flag &= ~SLP_GETSTREAM;
			return (0);
		}
		m = slp->ns_raw;
		if (m->m_len >= NFSX_UNSIGNED) {
			memcpy(caddr_t), mtod(m, (caddr_t)&recmark, NFSX_UNSIGNED);
			m->m_data += NFSX_UNSIGNED;
			m->m_len -= NFSX_UNSIGNED;
		} else {
			cp1 = (caddr_t)&recmark;
			cp2 = mtod(m, caddr_t);
			while (cp1 < ((caddr_t)&recmark) + NFSX_UNSIGNED) {
				while (m->m_len == 0) {
					m = m->m_next;
					cp2 = mtod(m, caddr_t);
				}
				*cp1++ = *cp2++;
				m->m_data++;
				m->m_len--;
			}
		}
		slp->ns_cc -= NFSX_UNSIGNED;
		slp->ns_reclen = ntohl(recmark) & ~0x80000000;
		if (slp->ns_reclen < NFS_MINPACKET || slp->ns_reclen > NFS_MAXPACKET) {
			slp->ns_flag &= ~SLP_GETSTREAM;
			return (EPERM);
		}
	    }

	    /*
	     * Now get the record part.
	     */
	    if (slp->ns_cc == slp->ns_reclen) {
		recm = slp->ns_raw;
		slp->ns_raw = slp->ns_rawend = (struct mbuf *)0;
		slp->ns_cc = slp->ns_reclen = 0;
	    } else if (slp->ns_cc > slp->ns_reclen) {
		len = 0;
		m = slp->ns_raw;
		om = (struct mbuf *)0;
		while (len < slp->ns_reclen) {
			if ((len + m->m_len) > slp->ns_reclen) {
				m2 = m_copym(m, 0, slp->ns_reclen - len,
					waitflag);
				if (m2) {
					if (om) {
						om->m_next = m2;
						recm = slp->ns_raw;
					} else
						recm = m2;
					m->m_data += slp->ns_reclen - len;
					m->m_len -= slp->ns_reclen - len;
					len = slp->ns_reclen;
				} else {
					slp->ns_flag &= ~SLP_GETSTREAM;
					return (EWOULDBLOCK);
				}
			} else if ((len + m->m_len) == slp->ns_reclen) {
				om = m;
				len += m->m_len;
				m = m->m_next;
				recm = slp->ns_raw;
				om->m_next = (struct mbuf *)0;
			} else {
				om = m;
				len += m->m_len;
				m = m->m_next;
			}
		}
		slp->ns_raw = m;
		slp->ns_cc -= len;
		slp->ns_reclen = 0;
	    } else {
		slp->ns_flag &= ~SLP_GETSTREAM;
		return (0);
	    }
	    nfs_realign(recm, 10 * NFSX_UNSIGNED);
	    if (slp->ns_recend)
		slp->ns_recend->m_nextpkt = recm;
	    else
		slp->ns_rec = recm;
	    slp->ns_recend = recm;
	}
}

/*
 * Parse an RPC header.
 */
nfsrv_dorec(slp, nd)
	register struct nfssvc_sock *slp;
	register struct nfsd *nd;
{
	register struct mbuf *m;
	int error;

	if ((slp->ns_flag & SLP_VALID) == 0 ||
	    (m = slp->ns_rec) == (struct mbuf *)0)
		return (ENOBUFS);
	if (slp->ns_rec = m->m_nextpkt)
		m->m_nextpkt = (struct mbuf *)0;
	else
		slp->ns_recend = (struct mbuf *)0;
	if (m->m_type == MT_SONAME) {
		nd->nd_nam = m;
		nd->nd_md = nd->nd_mrep = m->m_next;
		m->m_next = (struct mbuf *)0;
	} else {
		nd->nd_nam = (struct mbuf *)0;
		nd->nd_md = nd->nd_mrep = m;
	}
	nd->nd_dpos = mtod(nd->nd_md, caddr_t);
	if (error = nfs_getreq(nd, TRUE)) {
		m_freem(nd->nd_nam);
		return (error);
	}
	return (0);
}

/*
 * Parse an RPC request
 * - verify it
 * - fill in the cred struct.
 */
nfs_getreq(nd, has_header)
	register struct nfsd *nd;
	int has_header;
{
	register int len, i;
	register u_long *tl;
	register long t1;
	struct uio uio;
	struct iovec iov;
	caddr_t dpos, cp2;
	u_long nfsvers, auth_type;
	int error = 0, nqnfs = 0;
	struct mbuf *mrep, *md;

	mrep = nd->nd_mrep;
	md = nd->nd_md;
	dpos = nd->nd_dpos;
	if (has_header) {
		nfsm_dissect(tl, u_long *, 10*NFSX_UNSIGNED);
		nd->nd_retxid = *tl++;
		if (*tl++ != rpc_call) {
			m_freem(mrep);
			return (EBADRPC);
		}
	} else {
		nfsm_dissect(tl, u_long *, 8*NFSX_UNSIGNED);
	}
	nd->nd_repstat = 0;
	if (*tl++ != rpc_vers) {
		nd->nd_repstat = ERPCMISMATCH;
		nd->nd_procnum = NFSPROC_NOOP;
		return (0);
	}
	nfsvers = nfs_vers;
	if (*tl != nfs_prog) {
		if (*tl == nqnfs_prog) {
			nqnfs++;
			nfsvers = nqnfs_vers;
		} else {
			nd->nd_repstat = EPROGUNAVAIL;
			nd->nd_procnum = NFSPROC_NOOP;
			return (0);
		}
	}
	tl++;
	if (*tl++ != nfsvers) {
		nd->nd_repstat = EPROGMISMATCH;
		nd->nd_procnum = NFSPROC_NOOP;
		return (0);
	}
	nd->nd_procnum = fxdr_unsigned(u_long, *tl++);
	if (nd->nd_procnum == NFSPROC_NULL)
		return (0);
	if (nd->nd_procnum >= NFS_NPROCS ||
		(!nqnfs && nd->nd_procnum > NFSPROC_STATFS) ||
		(*tl != rpc_auth_unix && *tl != rpc_auth_kerb)) {
		nd->nd_repstat = EPROCUNAVAIL;
		nd->nd_procnum = NFSPROC_NOOP;
		return (0);
	}
	auth_type = *tl++;
	len = fxdr_unsigned(int, *tl++);
	if (len < 0 || len > RPCAUTH_MAXSIZ) {
		m_freem(mrep);
		return (EBADRPC);
	}

	/*
	 * Handle auth_unix or auth_kerb.
	 */
	if (auth_type == rpc_auth_unix) {
		len = fxdr_unsigned(int, *++tl);
		if (len < 0 || len > NFS_MAXNAMLEN) {
			m_freem(mrep);
			return (EBADRPC);
		}
		nfsm_adv(nfsm_rndup(len));
		nfsm_dissect(tl, u_long *, 3*NFSX_UNSIGNED);
		nd->nd_cr.cr_uid = fxdr_unsigned(uid_t, *tl++);
		nd->nd_cr.cr_gid = fxdr_unsigned(gid_t, *tl++);
		len = fxdr_unsigned(int, *tl);
		if (len < 0 || len > RPCAUTH_UNIXGIDS) {
			m_freem(mrep);
			return (EBADRPC);
		}
		nfsm_dissect(tl, u_long *, (len + 2)*NFSX_UNSIGNED);
		for (i = 1; i <= len; i++)
			if (i < NGROUPS)
				nd->nd_cr.cr_groups[i] = fxdr_unsigned(gid_t, *tl++);
			else
				tl++;
		nd->nd_cr.cr_ngroups = (len >= NGROUPS) ? NGROUPS : (len + 1);
	} else if (auth_type == rpc_auth_kerb) {
		nd->nd_cr.cr_uid = fxdr_unsigned(uid_t, *tl++);
		nd->nd_authlen = fxdr_unsigned(int, *tl);
		uio.uio_resid = nfsm_rndup(nd->nd_authlen);
		if (uio.uio_resid > (len - 2 * NFSX_UNSIGNED)) {
			m_freem(mrep);
			return (EBADRPC);
		}
		uio.uio_offset = 0;
		uio.uio_iov = &iov;
		uio.uio_iovcnt = 1;
		uio.uio_segflg = UIO_SYSSPACE;
		iov.iov_base = (caddr_t)nd->nd_authstr;
		iov.iov_len = RPCAUTH_MAXSIZ;
		nfsm_mtouio(&uio, uio.uio_resid);
		nfsm_dissect(tl, u_long *, 2 * NFSX_UNSIGNED);
		nd->nd_flag |= NFSD_NEEDAUTH;
	}

	/*
	 * Do we have any use for the verifier.
	 * According to the "Remote Procedure Call Protocol Spec." it
	 * should be AUTH_NULL, but some clients make it AUTH_UNIX?
	 * For now, just skip over it
	 */
	len = fxdr_unsigned(int, *++tl);
	if (len < 0 || len > RPCAUTH_MAXSIZ) {
		m_freem(mrep);
		return (EBADRPC);
	}
	if (len > 0) {
		nfsm_adv(nfsm_rndup(len));
	}

	/*
	 * For nqnfs, get piggybacked lease request.
	 */
	if (nqnfs && nd->nd_procnum != NQNFSPROC_EVICTED) {
		nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
		nd->nd_nqlflag = fxdr_unsigned(int, *tl);
		if (nd->nd_nqlflag) {
			nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			nd->nd_duration = fxdr_unsigned(int, *tl);
		} else
			nd->nd_duration = NQ_MINLEASE;
	} else {
		nd->nd_nqlflag = NQL_NOVAL;
		nd->nd_duration = NQ_MINLEASE;
	}
	nd->nd_md = md;
	nd->nd_dpos = dpos;
	return (0);
nfsmout:
	return (error);
}

/*
 * Search for a sleeping nfsd and wake it up.
 * SIDE EFFECT: If none found, set NFSD_CHECKSLP flag, so that one of the
 * running nfsds will go look for the work in the nfssvc_sock list.
 */
void
nfsrv_wakenfsd(slp)
	struct nfssvc_sock *slp;
{
	register struct nfsd *nd = nfsd_head.nd_next;

	if ((slp->ns_flag & SLP_VALID) == 0)
		return;
	while (nd != (struct nfsd *)&nfsd_head) {
		if (nd->nd_flag & NFSD_WAITING) {
			nd->nd_flag &= ~NFSD_WAITING;
			if (nd->nd_slp)
				panic("nfsd wakeup");
			slp->ns_sref++;
			nd->nd_slp = slp;
			wakeup((caddr_t)nd);
			return;
		}
		nd = nd->nd_next;
	}
	slp->ns_flag |= SLP_DOREC;
	nfsd_head.nd_flag |= NFSD_CHECKSLP;
}

nfs_msg(p, server, msg)
	struct proc *p;
	char *server, *msg;
{
	tpr_t tpr;

	if (p)
		tpr = tprintf_open(p);
	else
		tpr = NULL;
	tprintf(tpr, "nfs server %s: %s\n", server, msg);
	tprintf_close(tpr);
}
