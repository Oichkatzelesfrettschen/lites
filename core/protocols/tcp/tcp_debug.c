/*
 * tcp_debug.c
 *
 * Derived from:
 *
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 *
 *	@(#)tcp_debug.c	7.2 (Berkeley) 12/7/87
 *
 * Modified for x-kernel v3.2
 * Modifications Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 * $Revision: 1.4 $
 * $Date: 1993/02/01 22:23:43 $
 */

#include "tcp_internal.h"
#define TCPSTATES
#include "tcp_fsm.h"
#include "tcp_seq.h"
#define	TCPTIMERS
#include "tcp_timer.h"
#include "tcp_var.h"
#include "tcpip.h"
#define	TANAMES
#include "tcp_debug.h"

int	tcpconsdebug = 1;
int	tracetcpp = 0;
/*
 * Tcp debug routines
 */
void
tcp_trace(act, ostate, tp, ti, req)
	short act, ostate;
	struct tcpcb *tp;
	struct tcpiphdr *ti;
	int req;
{
	tcp_seq seq, ack;
	int len, flags;
	struct tcp_debug *td = &tcp_debug[tcp_debx++];

	if (tcp_debx == TCP_NDEBUG)
		tcp_debx = 0;
/*	td->td_time = iptime(); */
	td->td_act = act;
	td->td_ostate = ostate;
	td->td_tcb = (caddr_t)tp;
	if (tp)
		td->td_cb = *tp;
	else
		memset((caddr_t)&td->td_cb, 0, sizeof (*tp));
	if (ti)
		td->td_ti = *ti;
	else
		memset((caddr_t)&td->td_ti, 0, sizeof (*ti));
	td->td_req = req;
	if (tcpconsdebug == 0)
		return;
	if (tp)
		printf("%x %s:", tp, tcpstates[ostate]);
	else
		printf("???????? ");
	printf("%s ", tanames[act]);
	switch (act) {

	case TA_INPUT:
	case TA_OUTPUT:
	case TA_DROP:
		if (ti == 0)
			break;
		seq = ti->ti_seq;
		ack = ti->ti_ack;
		len = ti->ti_len;
		if (act == TA_OUTPUT) {
			seq = ntohl(seq);
			ack = ntohl(ack);
			len = ntohs((u_short)len);
		}
		if (act == TA_OUTPUT)
			len -= sizeof (struct tcphdr);
		if (len)
			printf("[%x..%x)", seq, seq+len);
		else
			printf("%x", seq);
		printf("@%x, urp=%x", ack, ti->ti_urp);
		flags = ti->ti_flags;
		if (flags) {
#ifndef lint
			char *cp = "<";
#ifdef __STDC__
#define pf(f) { if (ti->ti_flags&TH_##f) { printf("%s%s", cp, "f"); cp = ","; } }
#else
#define pf(f) { if (ti->ti_flags&TH_/**/f) { printf("%s%s", cp, "f"); cp = ","; } }
#endif
			pf(SYN); pf(ACK); pf(FIN); pf(RST); pf(PUSH); pf(URG);
#endif
			printf(">");
		}
		break;

	case TA_USER:
		printf("%s", prurequests[req&0xff]);
		if ((req & 0xff) == PRU_SLOWTIMO)
			printf("<%s>", tcptimers[req>>8]);
		break;
	}
	if (tp)
		printf(" -> %s", tcpstates[tp->t_state]);
	/* print out internal state of tp !?! */
	printf("\n");
	if (tp == 0)
		return;
	if (tracetcpp > 3) {
	  printf("\trcv_(nxt,wnd,up) (%x,%x,%x) snd_(una,nxt,max) (%x,%x,%x)\n",
		 tp->rcv_nxt, tp->rcv_wnd, tp->rcv_up, tp->snd_una, tp->snd_nxt,
		 tp->snd_max);
	  printf("\tsnd_(wl1,wl2,wnd) (%x,%x,%x)\n",
		 tp->snd_wl1, tp->snd_wl2, tp->snd_wnd);
	}
}


char *
tcpFlagStr(f)
    int f;
{
    static char buf[80];

    buf[0] = 0;
    if ( f & TH_FIN ) {
	strcat( buf, "FIN " );
    }
    if ( f & TH_SYN ) {
	strcat( buf, "SYN " );
    }
    if ( f & TH_RST ) {
	strcat( buf, "RST " );
    }
    if ( f & TH_PUSH ) {
	strcat( buf, "PUSH " );
    }
    if ( f & TH_ACK ) {
	strcat( buf, "ACK " );
    }
    if ( f & TH_URG ) {
	strcat( buf, "URG " );
    }
    return buf;
}
