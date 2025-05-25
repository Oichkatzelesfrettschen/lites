/*
 * blast_output.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.36 $
 * $Date: 1993/07/20 19:35:39 $
 */

#include "blast_internal.h"

#ifdef __STDC__

static void	blastSendTimeout( Event, void * );
static void	freeMstate( MSG_STATE * );
static void	unmapMstate( MSG_STATE * );

#else

static void	blastSendTimeout();
static void	freeMstate();
static void	unmapMstate();

#endif __STDC__

/* 
 * mstate sending reference counts:
 *
 * Reference counts represent both:
 *	presence in the senders map (<= 1 ref count)
 *	retransmission requests (>= 0 ref counts)
 *
 * When a sending mstate is removed from the senders map (through
 * either a timeout or user notification), the reference count
 * associated with the map binding is freed.  After this has happened,
 * the mstate will be deallocated once all active retransmission
 * requests complete.
 */



static xmsg_handle_t
sendShortMsg(m, state)
    Msg *m;
    SState *state;  
{
    state->short_hdr.len = msgLen(m);
    xIfTrace(blastp, TR_DETAILED) {
	blast_phdr(&state->short_hdr);
    }
    xTrace1(blastp, TR_DETAILED, "in blast_push2 down_s = %x",
	    xGetDown(state->self, 0));
    msgPush(m, blastHdrStore, &state->short_hdr, BLASTHLEN, 0);
    xIfTrace(blastp,TR_DETAILED) {
	xTrace0(blastp, TR_ALWAYS, "message in header");
	blast_phdr(&state->short_hdr);
    }
    xTrace1(blastp, TR_DETAILED, "in blast_push2 msg len = %d", msgLen(m));
    if (xPush(xGetDown(state->self, 0), m) ==  -1) {
	xTrace0(blastp, TR_ERRORS, "blast_push: can't send message");
	return XMSG_ERR_HANDLE;
    }
    return XMSG_NULL_HANDLE;
}


static void
push_fragment(s, frag, h)
    XObj s;
    Msg *frag;
    BLAST_HDR *h;
{
    Msg msgToPush;

    xIfTrace(blastp, TR_DETAILED) {
	xTrace0(blastp, TR_ALWAYS, "Outgoing message ");
	blast_phdr(h);
    }
    msgConstructCopy(&msgToPush, frag);
    msgPush(&msgToPush, blastHdrStore, h, BLASTHLEN, 0);
    if (xPush(s, &msgToPush) ==  -1) {
	xTrace0(blastp, TR_ERRORS, "blast_push: can't send fragment");
    }
    msgDestroy(&msgToPush);
}

    	
xmsg_handle_t
blastPush(s, msg)
    XObj s;
    Msg *msg;
{
    SState	*state;
    PState 	*pstate;
    MSG_STATE  	*mstate;
    BLAST_HDR 	*hdr;
    int 	seq;
    int 	num_frags;
    int 	i;
    XObj	lls;
    chan_info_t *info;
    
    xTrace0(blastp, TR_EVENTS, "in blast_push");
    xAssert(xIsSession(s));

    pstate = (PState *) s->myprotl->state;
    state = (SState *) s->state;
    xTrace1(blastp, TR_MORE_EVENTS, "blast_push: state = %d", state);
    xTrace1(blastp, TR_MORE_EVENTS,
	    "blast_push: outgoing msg len (no blast hdr): %d",
	    msgLen(msg));

    if (msgLen(msg) > state->fragmentSize * BLAST_MAX_FRAGS ) {
	xTrace2(blastp, TR_SOFT_ERRORS,
		"blast_push: message length %d > max size %d",
	       msgLen(msg), state->fragmentSize * BLAST_MAX_FRAGS);
	return XMSG_ERR_HANDLE;
    }
    
    /* if message is short, by-pass everthing */
    if ( msgLen(msg) < state->fragmentSize ) {
	return sendShortMsg(msg, state);
    }
    
    xTrace0(blastp, TR_DETAILED, "in blast_push3");

    /* get chan info */
    info = (chan_info_t *) msgGetAttr(msg,0);
    

    /* get new sequence number */
    if ( pstate->max_seq == BLAST_MAX_SEQ ) {
	xTraceP0(s, TR_MAJOR_EVENTS, "sequence number wraps");
	pstate->max_seq = 0;
    }
    seq = ++pstate->max_seq;
    
    /* need new message state */
    mstate = blastNewMstate(s);
    /* 
     * Add a reference count for this message
     */
    state->ircnt++;
    
    /* bind mstate to seqence number */
    mstate->binding = mapBind(pstate->send_map, (char *)&seq, mstate);
    if (mstate->binding == ERR_BIND) {
	xTrace0(blastp, TR_ERRORS, "blast_push: can't bind seqence number");
	if (mstate) xFree((char *)mstate);
	return XMSG_ERR_HANDLE;
    } 
    mstate->state = state;
    
    /* fill in header */
    hdr = &(mstate->hdr);
    *hdr = state->short_hdr;
    hdr->seq = seq;
    xIfTrace(blastp, TR_DETAILED) {
	xTrace0(blastp, TR_ALWAYS, "blasts_push: static header:");
	blast_phdr(hdr);
    }
    
    num_frags = (msgLen(msg) + state->fragmentSize - 1)/(state->fragmentSize);
    hdr->num_frag = num_frags;
    BLAST_MASK_SET_BIT(&hdr->mask, 1);
    
    /* send off fragments */
    xTrace1(blastp, TR_MORE_EVENTS, "blast: outstanding messages (P): %d",
	    pstate->outstanding_messages.count);
    semWait(&pstate->outstanding_messages);
    mstate->wait = SEND_CONST;
    lls = xGetDown(s, 0);
    for ( i=1; i <= num_frags; i++ ) {
	Msg *packet = &mstate->frags[i];
	msgConstructEmpty(packet);
	msgChopOff(msg, packet, state->fragmentSize);
	xTrace2(blastp, TR_MORE_EVENTS,
		"fragment: len[%d] = %d", i, msgLen(packet));
	/* fill in dynamic parts of header */
	hdr->len = msgLen(packet);
	xTrace1(blastp, TR_MORE_EVENTS, "blast_push: sending frag %d", i);
	push_fragment(lls, packet, hdr);
	BLAST_MASK_SHIFT_LEFT(hdr->mask, 1);
    }
    /* set timer to free message if no NACK is received */
    if ( ! mstate->event ) {
	xTrace1(blastp, TR_MORE_EVENTS,
		"scheduling blastSendTimeout for mstate %x", mstate);
	mstate->event = evSchedule(blastSendTimeout, mstate, mstate->wait);
    }
    if (info) {
	info->transport = s->myprotl;
	info->ticket = seq;
	info->reliable = 0;
	info->expensive = 1;
    }
    return XMSG_NULL_HANDLE;
}


/*
 * Retransmit the message referenced by mstate.  1's in 'mask' indicate
 * fragments received, 0's indicate fragments that need to be retransmitted.
 */
static void
retransmit(mask, mstate)
    BlastMask	mask;
    MSG_STATE 	*mstate;
{
    int 	i;
    SState	*state;
    BLAST_HDR 	*hdr;
    XObj	lls;
    
    xTrace2(blastp, REXMIT_T, "blast retransmit: called seq = %d mask = %s",
	    mstate->hdr.seq, blastShowMask(mask));
    if (mstate == 0) {
	xTrace0(blastp, REXMIT_T, "retransmit: no message state");
	return;
    }
    if ((state = (SState *)mstate->state) == 0) {
	xTrace0(blastp, TR_ERRORS, "retransmit: no state");
	return;
    }
    mstate->nacks_recieved++;
    hdr = &(mstate->hdr);
    hdr->op = BLAST_RETRANSMIT;
    /* send off fragments */
    lls = xGetDown(mstate->state->self, 0);
    for ( i=1; i<=hdr->num_frag; i++ ) {
	if ( ! BLAST_MASK_IS_BIT_SET(&mask, i) ) {
	    Msg *packet = &mstate->frags[i];
	    xTrace2(blastp, REXMIT_T,
		    "retransmit: retransmitting fragment %d, msgptr %x",
		    i, packet);
	    /* fill in dynamic parts of header */
	    BLAST_MASK_CLEAR(hdr->mask);
	    BLAST_MASK_SET_BIT(&hdr->mask, i);
	    hdr->len = msgLen(packet);
	    push_fragment(lls, packet, hdr);
	}
    }
}


/* 
 * Removes the mstate from the senders map, kills the timeout event,
 * and removes the mstate reference count associated with the map
 * binding.
 */
static void
unmapMstate( ms )
    MSG_STATE	*ms;
{
#ifdef XK_DEBUG
    XObj	sessn;
#endif
    SState	*ss;
    PState	*ps;

    ss = ms->state;
#ifdef XK_DEBUG    
    sessn = ss->self;
#endif
    xTraceP0(sessn, TR_MORE_EVENTS, "unmapping mstate");
    ps = (PState *)xMyProtl(ss->self)->state;
    if ( ms->event ) {
	evCancel(ms->event);
	ms->event = 0;
    }
    if (ms->binding) {
	if ( mapRemoveBinding(ps->send_map, ms->binding) == XK_SUCCESS ) {
	    /* 
	     * Release reference count for the map binding
	     */
	    freeMstate(ms);
	} else {
	    xTraceP0(sessn, TR_ERRORS, "unmapMstate: can't unbind!");
	}
    } else {
	xTraceP0(sessn, TR_ERRORS, "unmapMstate: no binding!");
    }
}


/*
 * Free the message and state associated with the outgoing message referenced
 * by mstate.
 */
static void
freeMstate(mstate)
    MSG_STATE *mstate;
{
    PState	*pstate;
    int		i;
    XObj	sessn;

    xTrace3(blastp, TR_MORE_EVENTS,
	    "blast output freeMstate, seq == %d, mstate == %x, rcnt == %d",
	    mstate->hdr.seq, mstate, mstate->rcnt);
    xAssert(mstate->rcnt > 0);
    mstate->rcnt--;
    if ( mstate->rcnt > 0 ) {
	return;
    }
    /* 
     * if rcnt == 0, unmapMstate should have been called.
     */
    xAssert(mstate->event == 0);
    sessn = ((SState *)mstate->state)->self;
    xTraceP0(sessn, TR_MORE_EVENTS, "truly deallocating sending mstate");
    pstate = (PState *)xMyProtl(sessn)->state;
    xTrace1(blastp, TR_MORE_EVENTS, "blast freeMstate: num_frags = %d",
	    mstate->hdr.num_frag);
    for ( i = 1; i <= mstate->hdr.num_frag; i++ ) {
	msgDestroy(&mstate->frags[i]);
    }
    if ( stackPush(pstate->mstateStack, (VOID *)mstate) ) {
	xTrace1(blastp, TR_MORE_EVENTS,
		"free_send_seq: mstate stack is full, freeing %x", mstate);
	xFree((char *)mstate);
    }
    xTrace1(blastp, TR_MORE_EVENTS, "blast: outstanding messages (v): %d",
	    pstate->outstanding_messages.count);
    semSignal(&pstate->outstanding_messages);
    /* 
     * Remove ref count corresponding to this message
     */
    blastDecIrc(sessn);
}

/*
 * Are we making progress? 
 */
int
blast_Retransmit(pstate, seq)
    PState *pstate;
    int seq;
{
    MSG_STATE   *mstate;


    xTrace1(blastp, TR_MORE_EVENTS, "free_send_msg: begin seq = %d", seq);
    if ( mapResolve(pstate->send_map, &seq, &mstate) == XK_FAILURE ) {
        xTrace0(blastp, TR_SOFT_ERRORS, "free_send_seq: nothing to free");
        return -1;
    }
    if (mstate->nacks_recieved) {
	mstate->nacks_recieved = 0;
	return 0;
    }
    return 1;
}


/* 
 * Kill the timeout event and free the outgoing message state
 */
int
blast_freeSendSeq(pstate, seq)
    PState *pstate;
    int seq;
{
    MSG_STATE	*mstate;
   
    
    xTrace1(blastp, TR_MORE_EVENTS, "free_send_msg: begin seq = %d", seq);
    if ( mapResolve(pstate->send_map, &seq, &mstate) == XK_FAILURE ) {
	xTrace0(blastp, TR_SOFT_ERRORS, "free_send_seq: nothing to free");
	return -1;
    } 
    unmapMstate(mstate);
    return 0;
}


/*
 * send_timout garbage collects the storage associated with 
 * a given sequence number. Since blast uses nacks it has 
 * to have some way of getting rid of storage 
 */
static void
blastSendTimeout(ev, arg)
    Event	ev;
    VOID 	*arg;
{
    MSG_STATE	*mstate = (MSG_STATE *)arg;

    xTrace0(blastp, REXMIT_T, "blast: send_timeout: timeout!");
    unmapMstate(mstate);
}

    
/* 
 * blastSenderPop -- process a retransmission request from out peer
 */
xkern_return_t
blastSenderPop(s, dg, hdr)
    XObj s;
    Msg *dg;
    BLAST_HDR *hdr;
{
    SState	*state;
    PState	*pstate;
    MSG_STATE 	*mstate;
    
    xTrace0(blastp, TR_EVENTS, "BLAST_pop");
    xAssert(xIsSession(s));
    
    state = (SState *)s->state;
    pstate = (PState *)s->myprotl->state;
    /*
     * look for message state
     */
    if ( mapResolve(pstate->send_map, &hdr->seq, &mstate) == XK_FAILURE ) {
	xTraceP0(s, TR_EVENTS, "senderPop: unmatched nack");
	return XK_SUCCESS;
    }
    xAssert(mstate->rcnt > 0);
    mstate->rcnt++;
    /* 
     * Cancel and restart timeout event
     */
    if ( mstate->event ) {
	xTraceP1(s, TR_MORE_EVENTS,
		"rescheduling send timeout for mstate %x", mstate);
	evCancel(mstate->event);
	mstate->event = evSchedule(blastSendTimeout, mstate, mstate->wait);
	retransmit(hdr->mask, mstate);
    } else {
	/* 
	 * If the sender timeout event hasn't been scheduled yet, either:
	 *
	 *	not all fragments have been sent yet ...
	 *  or
	 *	we're already processing a retransmisstion request
	 */
	xTraceP0(s, TR_SOFT_ERRORS, "retransmission request ignored");
    }
    freeMstate(mstate);	/* decrease ref count */
    return XK_SUCCESS;
} 


