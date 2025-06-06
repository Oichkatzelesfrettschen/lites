/*
 * tcp_timer.c
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
 *	@(#)tcp_timer.c	7.11 (Berkeley) 12/7/87
 *
 * Modified for x-kernel v3.2
 * Modifications Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 * $Revision: 1.9 $
 * $Date: 1993/02/01 22:23:31 $
 */

#include "xkernel.h"
#include "tcp_internal.h"
#include "tcp_fsm.h"
#include "tcp_seq.h"
#include "tcp_timer.h"
#include "tcp_var.h"
#include "tcpip.h"

int	tcpnodelack = 0;
/*
 * Fast timeout routine for processing delayed acks
 */
void
tcp_fasttimo( ev, unusedArg )
    Event	ev;
    VOID 	*unusedArg;
{
	register struct inpcb *inp;
	register struct tcpcb *tp;
	/* 
	 * The original code protected this routine with splnet().  If
	 * the timeout thread blocks, there could be a problem.
	 */
	/* int s = splnet(); */

	inp = tcb.inp_next;
	if (inp)
	for (; inp != &tcb; inp = inp->inp_next)
		if ((tp = (struct tcpcb *)inp->inp_ppcb) &&
		    (tp->t_flags & TF_DELACK)) {
			tp->t_flags &= ~TF_DELACK;
			tp->t_flags |= TF_ACKNOW;
			tcpstat.tcps_delack++;
			/* 
			 * What if this blocks???
			 */
			(void) tcp_output(tp);
		}
	/* splx(s); */
	evDetach(evSchedule(tcp_fasttimo, 0, TCP_FAST_INTERVAL));
}

/*
 * Tcp protocol timeout routine called every 500 ms.
 * Updates the timers in all active tcb's and
 * causes finite state machine actions if timers expire.
 */
void
tcp_slowtimo(ev, unusedArg)
    Event	ev;
    VOID 	*unusedArg;
{
	register struct inpcb *ip, *ipnxt;
	register struct tcpcb *tp;
	/* 
	 * The original code protected this routine with splnet().  If
	 * the timeout thread blocks, there could be a problem.
	 */
	/* int s = splnet(); */
	register int i;

	/*
	 * Search through tcb's and update active timers.
	 */
	ip = tcb.inp_next;
	if (ip != 0) {
	    for (; ip != &tcb; ip = ipnxt) {
		ipnxt = ip->inp_next;
		tp = intotcpcb(ip);
		if (tp == 0)
		  continue;
                if (tp->t_timer[0] && --tp->t_timer[0] == 0) {
                        (void) tcp_usrreq(tp->t_inpcb->inp_session,
                                          PRU_SLOWTIMO, (Msg *)0,
                                          (Msg *)0);
                        if (ipnxt->inp_prev != ip)
                          goto tpgone;
                }
                if (tp->t_timer[1] && --tp->t_timer[1] == 0) {
                        (void) tcp_usrreq(tp->t_inpcb->inp_session,
                                          PRU_SLOWTIMO, (Msg *)0,
                                          (Msg *)1);
                        if (ipnxt->inp_prev != ip)
                          goto tpgone;
                }
                if (tp->t_timer[2] && --tp->t_timer[2] == 0) {
                        (void) tcp_usrreq(tp->t_inpcb->inp_session,
                                          PRU_SLOWTIMO, (Msg *)0,
                                          (Msg *)2);
                        if (ipnxt->inp_prev != ip)
                          goto tpgone;
                }
                if (tp->t_timer[3] && --tp->t_timer[3] == 0) {
                        (void) tcp_usrreq(tp->t_inpcb->inp_session,
                                          PRU_SLOWTIMO, (Msg *)0,
                                          (Msg *)3);
                        if (ipnxt->inp_prev != ip)
                          goto tpgone;
                }
		tp->t_idle++;
		if (tp->t_rtt)
		  tp->t_rtt++;
tpgone:
		;
	    }
	    tcp_iss += TCP_ISSINCR/PR_SLOWHZ;		/* increment iss */
#ifdef TCP_COMPAT_42
	    if ((int)tcp_iss < 0)
	      tcp_iss = 0;				/* XXX */
#endif
	}
	/* splx(s); */
	evDetach(evSchedule(tcp_slowtimo, 0, TCP_SLOW_INTERVAL));
}

/*
 * Cancel all timers for TCP tp.
 */
void
tcp_canceltimers(tp)
	struct tcpcb *tp;
{
        tp->t_timer[0] = 0;
        tp->t_timer[1] = 0;
        tp->t_timer[2] = 0;
        tp->t_timer[3] = 0;
}

int	tcp_backoff[TCP_MAXRXTSHIFT + 1] =
    { 1, 2, 4, 8, 16, 32, 64, 64, 64, 64, 64, 64, 64 };


/*
 * TCP timer processing.
 */
struct tcpcb *
tcp_timers(tp, timer)
	register struct tcpcb *tp;
	int timer;
{
	register int rexmt;

	switch (timer) {

	/*
	 * 2 MSL timeout in shutdown went off.  If we're closed but
	 * still waiting for peer to close and connection has been idle
	 * too long, or if 2MSL time is up from TIME_WAIT, delete connection
	 * control block.  Otherwise, check again in a bit.
	 */
	case TCPT_2MSL:
		if (tp->t_state != TCPS_TIME_WAIT &&
		    tp->t_idle <= TCPTV_MAXIDLE)
			tp->t_timer[TCPT_2MSL] = TCPTV_KEEP;
		else
			tp = tcp_destroy(tp);
		break;

	/*
	 * Retransmission timer went off.  Message has not
	 * been acked within retransmit interval.  Back off
	 * to a longer retransmit interval and retransmit one segment.
	 */
	case TCPT_REXMT:
		xTrace1(tcpp, 4, "Retransmission timeout (%d)",
			tp->t_rxtshift);
		if (++tp->t_rxtshift > TCP_MAXRXTSHIFT) {
		    	xTrace0(tcpp, 4, "Too many rexmits, dropping");
			tp->t_rxtshift = TCP_MAXRXTSHIFT;
			tcpstat.tcps_timeoutdrop++;
			tp = tcp_drop(tp, ETIMEDOUT);
			break;
		}
		tcpstat.tcps_rexmttimeo++;
		rexmt = ((tp->t_srtt >> 2) + tp->t_rttvar) >> 1;
		rexmt *= tcp_backoff[tp->t_rxtshift];
		TCPT_RANGESET(tp->t_rxtcur, rexmt, TCPTV_MIN, TCPTV_REXMTMAX);
		tp->t_timer[TCPT_REXMT] = tp->t_rxtcur;
		/*
		 * If losing, let the lower level know and try for
		 * a better route.  Also, if we backed off this far,
		 * our srtt estimate is probably bogus.  Clobber it
		 * so we'll take the next rtt measurement as our srtt;
		 * move the current srtt into rttvar to keep the current
		 * retransmit times until then.
		 */
		if (tp->t_rxtshift > TCP_MAXRXTSHIFT / 4) {
			tp->t_rttvar += (tp->t_srtt >> 2);
			tp->t_srtt = 0;
		}
		tp->snd_nxt = tp->snd_una;
		/*
		 * If timing a segment in this window, stop the timer.
		 */
		tp->t_rtt = 0;
		/*
		 * Close the congestion window down to one segment
		 * (we'll open it by one segment for each ack we get).
		 * Since we probably have a window's worth of unacked
		 * data accumulated, this "slow start" keeps us from
		 * dumping all that data as back-to-back packets (which
		 * might overwhelm an intermediate gateway).
		 *
		 * There are two phases to the opening: Initially we
		 * open by one mss on each ack.  This makes the window
		 * size increase exponentially with time.  If the
		 * window is larger than the path can handle, this
		 * exponential growth results in dropped packet(s)
		 * almost immediately.  To get more time between 
		 * drops but still "push" the network to take advantage
		 * of improving conditions, we switch from exponential
		 * to linear window opening at some threshhold size.
		 * For a threshhold, we use half the current window
		 * size, truncated to a multiple of the mss.
		 *
		 * (the minimum cwnd that will give us exponential
		 * growth is 2 mss.  We don't allow the threshhold
		 * to go below this.)
		 */
		{
		u_short win = MIN(tp->snd_wnd, tp->snd_cwnd) / 2 / tp->t_maxseg;
		if (win < 2)
			win = 2;
		tp->snd_cwnd = tp->t_maxseg;
		tp->snd_ssthresh = win * tp->t_maxseg;
		}
		(void) tcp_output(tp);
		break;

	/*
	 * Persistance timer into zero window.
	 * Force a byte to be output, if possible.
	 */
	case TCPT_PERSIST:
		tcpstat.tcps_persisttimeo++;
		tcp_setpersist(tp);
		tp->t_force = 1;
		(void) tcp_output(tp);
		tp->t_force = 0;
		break;

	/*
	 * Keep-alive timer went off; send something
	 * or drop connection if idle for too long.
	 */
	case TCPT_KEEP:
		tcpstat.tcps_keeptimeo++;
		if (tp->t_state < TCPS_ESTABLISHED)
			goto dropit;
#define SHOULDDOKEEPS(X) 0
		if (SHOULDDOKEEPS(tp->t_inpcb->inp_session) &&
		    tp->t_state <= TCPS_CLOSE_WAIT) {
		    	if (tp->t_idle >= TCPTV_MAXIDLE)
				goto dropit;
			/*
			 * Send a packet designed to force a response
			 * if the peer is up and reachable:
			 * either an ACK if the connection is still alive,
			 * or an RST if the peer has closed the connection
			 * due to timeout or reboot.
			 * Using sequence number tp->snd_una-1
			 * causes the transmitted zero-length segment
			 * to lie outside the receive window;
			 * by the protocol spec, this requires the
			 * correspondent TCP to respond.
			 */
			tcpstat.tcps_keepprobe++;
#ifdef TCP_COMPAT_42
			/*
			 * The keepalive packet must have nonzero length
			 * to get a 4.2 host to respond.
			 */
			tcp_respond(tp, &tp->t_template->ti_t, 
				    &tp->t_template->ti_p,
				    tp->rcv_nxt - 1, tp->snd_una - 1, 0);
#else
			tcp_respond(tp, &tp->t_template->ti_t, 
				    &tp->t_template->ti_p,
				    tp->rcv_nxt, tp->snd_una - 1, 0);
#endif
		}
		tp->t_timer[TCPT_KEEP] = TCPTV_KEEP;
		break;
	dropit:
		tcpstat.tcps_keepdrops++;
		tp = tcp_drop(tp, ETIMEDOUT);
		break;
	}
	return (tp);
}
