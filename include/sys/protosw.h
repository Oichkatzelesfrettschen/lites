/*
 * Copyright (c) 1982, 1986, 1993
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
 *	@(#)protosw.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _SYS_PROTOSW_H_
#define _SYS_PROTOSW_H_

#include <sys/types.h>

/*
 * Forward declarations
 */
struct mbuf;
struct sockaddr;
struct socket;
struct domain;
struct proc;
struct sockopt;

/*
 * Protocol switch table.
 *
 * Each protocol has a handle initializing one of these structures,
 * which is used for protocol-protocol and system-protocol communication.
 *
 * A protocol is called through the pr_input routine from the lower
 * level protocol that is sending data up the stack, and through the
 * pr_output routine to send data down the stack.
 */
struct protosw {
    short   pr_type;            /* socket type used for */
    struct  domain *pr_domain;  /* domain protocol belongs to */
    short   pr_protocol;        /* protocol number */
    short   pr_flags;           /* see below */

/* protocol-protocol hooks */
    void    (*pr_input)(struct mbuf *, int);
                                /* input to protocol (from below) */
    int     (*pr_output)(struct mbuf *, struct mbuf *, struct mbuf *, int);
                                /* output to protocol (from above) */
    void    (*pr_ctlinput)(int, struct sockaddr *, void *);
                                /* control input (from below) */
    int     (*pr_ctloutput)(int, struct socket *, int, int, struct mbuf **);
                                /* control output (from above) */

/* user-protocol hook */
    int     (*pr_usrreq)(struct socket *, int, struct mbuf *,
                         struct mbuf *, struct mbuf *);
                                /* user request: see list below */

/* utility hooks */
    void    (*pr_init)(void);   /* initialization hook */
    void    (*pr_fasttimo)(void);   /* fast timeout (200ms) */
    void    (*pr_slowtimo)(void);   /* slow timeout (500ms) */
    void    (*pr_drain)(void);  /* flush any excess space possible */
    int     (*pr_sysctl)(int *, u_int, void *, size_t *, void *, size_t);
                                /* sysctl for protocol */
};

/*
 * Values for pr_flags
 */
#define PR_ATOMIC       0x01    /* exchange atomic messages only */
#define PR_ADDR         0x02    /* addresses given with messages */
#define PR_CONNREQUIRED 0x04    /* connection required by protocol */
#define PR_WANTRCVD     0x08    /* want PRU_RCVD calls */
#define PR_RIGHTS       0x10    /* passes capabilities */
#define PR_IMPLOPCL     0x20    /* implied open/close */
#define PR_LASTHDR      0x40    /* enforce ipsec policy; last header */

/*
 * The arguments to usrreq are:
 *	(*protosw[].pr_usrreq)(up, req, m, nam, opt);
 * where up is a (struct socket *), req is one of these requests,
 * m is a optional mbuf chain containing a message,
 * nam is an optional mbuf chain containing an address,
 * and opt is a pointer to a socketopt structure or nil.
 */
#define PRU_ATTACH              0   /* attach protocol to up */
#define PRU_DETACH              1   /* detach protocol from up */
#define PRU_BIND                2   /* bind socket to address */
#define PRU_LISTEN              3   /* listen for connection */
#define PRU_CONNECT             4   /* establish connection to peer */
#define PRU_ACCEPT              5   /* accept connection from peer */
#define PRU_DISCONNECT          6   /* disconnect from peer */
#define PRU_SHUTDOWN            7   /* won't send any more data */
#define PRU_RCVD                8   /* have taken data; more room now */
#define PRU_SEND                9   /* send this data */
#define PRU_ABORT               10  /* abort (fast DISCONNECT, DETACH) */
#define PRU_CONTROL             11  /* control operations on protocol */
#define PRU_SENSE               12  /* return status into m */
#define PRU_RCVOOB              13  /* retrieve out of band data */
#define PRU_SENDOOB             14  /* send out of band data */
#define PRU_SOCKADDR            15  /* fetch socket's address */
#define PRU_PEERADDR            16  /* fetch peer's address */
#define PRU_CONNECT2            17  /* connect two sockets */
/* begin for protocols internal use */
#define PRU_FASTTIMO            18  /* 200ms timeout */
#define PRU_SLOWTIMO            19  /* 500ms timeout */
#define PRU_PROTORCV            20  /* receive from below */
#define PRU_PROTOSEND           21  /* send to below */
#define PRU_SEND_EOF            22  /* send and close */
#define PRU_NREQ                23

#ifdef KERNEL

/*
 * Global protocol switch table.
 */
extern struct protosw *protoswLAST;
extern struct protosw *pffindproto(int family, int protocol, int type);
extern struct protosw *pffindtype(int family, int type);

#endif /* KERNEL */

#endif /* !_SYS_PROTOSW_H_ */
