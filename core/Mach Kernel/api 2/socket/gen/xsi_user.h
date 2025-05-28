#ifndef	_xsi_user_
#define	_xsi_user_

/* Module xsi */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <mach/std_types.h>
#include "xsi_types.h"

/* Routine hello */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_hello
#if	defined(LINTLIBRARY)
    (server_port, client_id, signal_port, is_xsi_fd, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	mach_port_t signal_port;
	xsi_fdset_t is_xsi_fd;
	int *errnum;
{ return xsi_hello(server_port, client_id, signal_port, is_xsi_fd, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	mach_port_t signal_port,
	xsi_fdset_t is_xsi_fd,
	int *errnum
);
#endif

/* Routine clone */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_clone
#if	defined(LINTLIBRARY)
    (server_port, client_id, parent_id, port, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int parent_id;
	mach_port_t port;
	int *errnum;
{ return xsi_clone(server_port, client_id, parent_id, port, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int parent_id,
	mach_port_t port,
	int *errnum
);
#endif

/* Routine await_clone_done */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_await_clone_done
#if	defined(LINTLIBRARY)
    (server_port, client_id, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int *errnum;
{ return xsi_await_clone_done(server_port, client_id, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int *errnum
);
#endif

/* Routine accept */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_accept
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, addr, addrCnt, ns, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_sockaddr_t addr;
	mach_msg_type_number_t *addrCnt;
	int ns;
	int *errnum;
{ return xsi_accept(server_port, client_id, s, addr, addrCnt, ns, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t *addrCnt,
	int ns,
	int *errnum
);
#endif

/* Routine bind */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_bind
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, addr, addrCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_sockaddr_t addr;
	mach_msg_type_number_t addrCnt;
	int *errnum;
{ return xsi_bind(server_port, client_id, s, addr, addrCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t addrCnt,
	int *errnum
);
#endif

/* Routine close */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_close
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	int *errnum;
{ return xsi_close(server_port, client_id, s, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int *errnum
);
#endif

/* Routine connect */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_connect
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, addr, addrCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_sockaddr_t addr;
	mach_msg_type_number_t addrCnt;
	int *errnum;
{ return xsi_connect(server_port, client_id, s, addr, addrCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t addrCnt,
	int *errnum
);
#endif

/* Routine dup2 */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_dup2
#if	defined(LINTLIBRARY)
    (server_port, client_id, from, to, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int from;
	int to;
	int *errnum;
{ return xsi_dup2(server_port, client_id, from, to, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int from,
	int to,
	int *errnum
);
#endif

/* Routine listen */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_listen
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, backlog, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	int backlog;
	int *errnum;
{ return xsi_listen(server_port, client_id, s, backlog, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int backlog,
	int *errnum
);
#endif

/* Routine socket */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_socket
#if	defined(LINTLIBRARY)
    (server_port, client_id, domain, conn_type, protocol, s, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int domain;
	int conn_type;
	int protocol;
	int s;
	int *errnum;
{ return xsi_socket(server_port, client_id, domain, conn_type, protocol, s, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int domain,
	int conn_type,
	int protocol,
	int s,
	int *errnum
);
#endif

/* Routine getpeername */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_getpeername
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, addr, addrCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_sockaddr_t addr;
	mach_msg_type_number_t *addrCnt;
	int *errnum;
{ return xsi_getpeername(server_port, client_id, s, addr, addrCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t *addrCnt,
	int *errnum
);
#endif

/* Routine getsockname */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_getsockname
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, addr, addrCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_sockaddr_t addr;
	mach_msg_type_number_t *addrCnt;
	int *errnum;
{ return xsi_getsockname(server_port, client_id, s, addr, addrCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_sockaddr_t addr,
	mach_msg_type_number_t *addrCnt,
	int *errnum
);
#endif

/* Routine getsockopt */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_getsockopt
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, level, optname, optval, optvalCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	int level;
	int optname;
	xsi_varbuf_t *optval;
	mach_msg_type_number_t *optvalCnt;
	int *errnum;
{ return xsi_getsockopt(server_port, client_id, s, level, optname, optval, optvalCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int level,
	int optname,
	xsi_varbuf_t *optval,
	mach_msg_type_number_t *optvalCnt,
	int *errnum
);
#endif

/* Routine setsockopt */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_setsockopt
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, level, optname, optval, optvalCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	int level;
	int optname;
	xsi_varbuf_t optval;
	mach_msg_type_number_t optvalCnt;
	int *errnum;
{ return xsi_setsockopt(server_port, client_id, s, level, optname, optval, optvalCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int level,
	int optname,
	xsi_varbuf_t optval,
	mach_msg_type_number_t optvalCnt,
	int *errnum
);
#endif

/* Routine select */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_select
#if	defined(LINTLIBRARY)
    (server_port, client_id, seqno, nfds, specified, readfds, writefds, exceptfds, timeout, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int seqno;
	int *nfds;
	xsi_select_mask_t specified;
	xsi_fdset_t readfds;
	xsi_fdset_t writefds;
	xsi_fdset_t exceptfds;
	xsi_timeval_t timeout;
	int *errnum;
{ return xsi_select(server_port, client_id, seqno, nfds, specified, readfds, writefds, exceptfds, timeout, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int seqno,
	int *nfds,
	xsi_select_mask_t specified,
	xsi_fdset_t readfds,
	xsi_fdset_t writefds,
	xsi_fdset_t exceptfds,
	xsi_timeval_t timeout,
	int *errnum
);
#endif

/* Routine select_cancel */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_select_cancel
#if	defined(LINTLIBRARY)
    (server_port, client_id, seqno, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int seqno;
	int *errnum;
{ return xsi_select_cancel(server_port, client_id, seqno, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int seqno,
	int *errnum
);
#endif

/* Routine ioctl */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_ioctl
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, request, inp, inpCnt, outp, outpCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	int request;
	xsi_varbuf_t inp;
	mach_msg_type_number_t inpCnt;
	xsi_varbuf_t *outp;
	mach_msg_type_number_t *outpCnt;
	int *errnum;
{ return xsi_ioctl(server_port, client_id, s, request, inp, inpCnt, outp, outpCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int request,
	xsi_varbuf_t inp,
	mach_msg_type_number_t inpCnt,
	xsi_varbuf_t *outp,
	mach_msg_type_number_t *outpCnt,
	int *errnum
);
#endif

/* Routine fcntl */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_fcntl
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, cmd, arg, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	int cmd;
	int *arg;
	int *errnum;
{ return xsi_fcntl(server_port, client_id, s, cmd, arg, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	int cmd,
	int *arg,
	int *errnum
);
#endif

/* Routine read */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_read
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, buf, bufCnt, nbytes, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_varbuf_t *buf;
	mach_msg_type_number_t *bufCnt;
	int nbytes;
	int *errnum;
{ return xsi_read(server_port, client_id, s, buf, bufCnt, nbytes, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t *buf,
	mach_msg_type_number_t *bufCnt,
	int nbytes,
	int *errnum
);
#endif

/* Routine write */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_write
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, buf, bufCnt, nwritten, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_varbuf_t buf;
	mach_msg_type_number_t bufCnt;
	int *nwritten;
	int *errnum;
{ return xsi_write(server_port, client_id, s, buf, bufCnt, nwritten, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t buf,
	mach_msg_type_number_t bufCnt,
	int *nwritten,
	int *errnum
);
#endif

/* Routine recv */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_recv
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, buf, bufCnt, maxlen, flags, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_varbuf_t *buf;
	mach_msg_type_number_t *bufCnt;
	int maxlen;
	int flags;
	int *errnum;
{ return xsi_recv(server_port, client_id, s, buf, bufCnt, maxlen, flags, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t *buf,
	mach_msg_type_number_t *bufCnt,
	int maxlen,
	int flags,
	int *errnum
);
#endif

/* Routine recvfrom */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_recvfrom
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, buf, bufCnt, maxlen, flags, from_addr, from_addrCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_varbuf_t *buf;
	mach_msg_type_number_t *bufCnt;
	int maxlen;
	int flags;
	xsi_sockaddr_t from_addr;
	mach_msg_type_number_t *from_addrCnt;
	int *errnum;
{ return xsi_recvfrom(server_port, client_id, s, buf, bufCnt, maxlen, flags, from_addr, from_addrCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t *buf,
	mach_msg_type_number_t *bufCnt,
	int maxlen,
	int flags,
	xsi_sockaddr_t from_addr,
	mach_msg_type_number_t *from_addrCnt,
	int *errnum
);
#endif

/* Routine send */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_send
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, msg, msgCnt, nsent, flags, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_varbuf_t msg;
	mach_msg_type_number_t msgCnt;
	int *nsent;
	int flags;
	int *errnum;
{ return xsi_send(server_port, client_id, s, msg, msgCnt, nsent, flags, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t msg,
	mach_msg_type_number_t msgCnt,
	int *nsent,
	int flags,
	int *errnum
);
#endif

/* Routine sendto */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xsi_sendto
#if	defined(LINTLIBRARY)
    (server_port, client_id, s, msg, msgCnt, nsent, flags, toaddr, toaddrCnt, errnum)
	mach_port_t server_port;
	xsi_clientid_t client_id;
	int s;
	xsi_varbuf_t msg;
	mach_msg_type_number_t msgCnt;
	int *nsent;
	int flags;
	xsi_sockaddr_t toaddr;
	mach_msg_type_number_t toaddrCnt;
	int *errnum;
{ return xsi_sendto(server_port, client_id, s, msg, msgCnt, nsent, flags, toaddr, toaddrCnt, errnum); }
#else
(
	mach_port_t server_port,
	xsi_clientid_t client_id,
	int s,
	xsi_varbuf_t msg,
	mach_msg_type_number_t msgCnt,
	int *nsent,
	int flags,
	xsi_sockaddr_t toaddr,
	mach_msg_type_number_t toaddrCnt,
	int *errnum
);
#endif

#endif	/* not defined(_xsi_user_) */
