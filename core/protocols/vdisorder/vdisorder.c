/* 
 * $RCSfile: vdisorder.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/05/05 22:48:40 $
 */


#include "xkernel.h"
#include "vdisorder.h"
#include "vdisorder_i.h"

#ifdef __STDC__

static void		protlFuncInit( XObj );
static void		sessnInit( XObj );
static XObj		vdisorderCreateSessn( XObj, XObj, XObj, XObj );
static XObj		vdisorderOpen( XObj, XObj, XObj, Part * );
static xkern_return_t	vdisorderOpenDisable( XObj, XObj, XObj, Part * );
static xkern_return_t	vdisorderOpenDone( XObj, XObj, XObj, XObj );
static xkern_return_t	vdisorderOpenEnable( XObj, XObj, XObj, Part * );
static void 		vdisorderTimeout(Event, VOID *);
static void 		vdisorderReleaseMsgs(XObj);

#else

static void		protlFuncInit();
static void		sessnInit();
static XObj		vdisorderCreateSessn();
static XObj		vdisorderOpen();
static xkern_return_t	vdisorderOpenDisable();
static xkern_return_t	vdisorderOpenDone();
static xkern_return_t	vdisorderOpenEnable();
static void 		vdisorderTimeout();
static void 		vdisorderReleaseMsgs();

#endif __STDC__


int tracevdisorderp;


void
vdisorder_init( self )
    XObj 	self;
{
    PState	*ps;
    
    xTrace0(vdisorderp, TR_GROSS_EVENTS, "VDISORDER init");
    xAssert(xIsProtocol(self));
    if ( ! xIsProtocol(xGetDown(self, 0)) ) {
	xError("VDISORDER down vector is misconfigured");
	return;
    }
    ps = X_NEW(PState);
    self->state = (VOID *)ps;
    protlFuncInit(self);
    ps->activeMap = mapCreate(VDISORDER_ACT_MAP_SZ, sizeof(XObj));
    ps->passiveMap = mapCreate(VDISORDER_PAS_MAP_SZ, sizeof(XObj));
}


static XObj
vdisorderOpen( self, hlpRcv, hlpType, p )
    XObj	self, hlpRcv, hlpType;
    Part 	*p;
{
    XObj	s, lls;
    PState	*ps = (PState *)self->state;
    
    xTrace0(vdisorderp, TR_MAJOR_EVENTS, "VDISORDER open");
    if ( (lls = xOpen(self, hlpType, xGetDown(self, 0), p)) == ERR_XOBJ ) {
	xTrace0(vdisorderp, TR_ERRORS, 
            "vdisorderOpen: could not open lower session");
	return ERR_XOBJ;
    }
    if ( mapResolve(ps->activeMap, &lls, &s) == XK_SUCCESS ) {
	xTrace0(vdisorderp, TR_MORE_EVENTS, 
            "vdisorderOpen -- found an existing one");
    } else {
	xTrace0(vdisorderp, TR_MAJOR_EVENTS, 
            "vdisorderOpen -- creating a new one");
	if ( (s = vdisorderCreateSessn(self, hlpRcv, hlpType, lls)) == 
            ERR_XOBJ ) {
	    xClose(lls);
	}
    }
    return s;
}


static XObj
vdisorderCreateSessn( self, hlpRcv, hlpType, lls )
    XObj 	self, hlpRcv, hlpType, lls;
{
    XObj	s;
    PState	*ps = (PState *)self->state;
    SState	*ss;
    XTime	t;
    
    if ( (s = xCreateSessn(sessnInit, hlpRcv, hlpType, self, 1, &lls))
			== ERR_XOBJ ) {
	xTrace0(vdisorderp, TR_ERRORS, "create sessn fails in vdisorderOpen");
	return ERR_XOBJ;
    }
    s->binding = mapBind(ps->activeMap, (char *)&lls, (int)s);
    if ( s->binding == ERR_BIND ) {
	xTrace0(vdisorderp, TR_ERRORS, "mapBind fails in vdisorderCreateSessn");
	return ERR_XOBJ;
    }
    /*
     * The lower sessions' up field is made to point to this
     * vdisorder session (not the protocol)
     */
    xSetUp(lls, s);
    ss = X_NEW(SState);
    s->state = (VOID *)ss;
    xGetTime(&t);
    ss->arrivalInterval = VDISORDER_DEF_ARRIVAL_INTERVAL;
    xTrace1(vdisorderp, TR_MAJOR_EVENTS, 
            "VDISORDER reordering after every %d pckts", ss->arrivalInterval);
    ss->disorderInterval = VDISORDER_DEF_DISORDER_INTERVAL;
    xTrace1(vdisorderp, TR_MAJOR_EVENTS, 
            "VDISORDER reordering %d pckts after interval", 
            ss->disorderInterval);
    ss->timeoutInterval = VDISORDER_DEF_TIMEOUT_INTERVAL;
    xTrace1(vdisorderp, TR_MAJOR_EVENTS, 
            "VDISORDER timing out after %d milliseconds", ss->timeoutInterval);
    ss->arrivalCount = 0;
    ss->disorderCount = 0;
    ss->disorderState = VDISORDER_STATE_PASSTHROUGH;
    xTrace1(vdisorderp, TR_MAJOR_EVENTS, "VDISORDER open returns %x", s);
    return s;
}


static int
vdisorderControlSessn( s, opcode, buf, len )
    XObj 	s;
    int 	opcode, len;
    char 	*buf;
{
    SState	*ss = (SState *)s->state;
    int		temp;

    xTrace0(vdisorderp, TR_EVENTS, "VDISORDER controlsessn");
    switch ( opcode ) {

      case VDISORDER_SET_ARRIVAL_INTERVAL:
	ss->arrivalInterval = *(int *)buf;
	xTrace1(vdisorderp, TR_EVENTS,
		"VDISORDER controlsessn resets arrival interval to %d",
		ss->arrivalInterval);
	return 0;

      case VDISORDER_GET_ARRIVAL_INTERVAL:
	*(int *)buf = ss->arrivalInterval;
	return sizeof(int);

      case VDISORDER_SET_DISORDER_INTERVAL:
	temp = *(int *)buf;
        if (temp >= VDISORDER_MAX_QUEUED_MESSAGES)
        {
	    xTrace1(vdisorderp, TR_ERRORS,
		"VDISORDER controlsessn attempted bad disorder interval %d",
                temp);
            return -1;
        }
	ss->disorderInterval = temp;
	xTrace1(vdisorderp, TR_EVENTS,
		"VDISORDER controlsessn resets disorder interval to %d",
		ss->disorderInterval);
	return 0;

      case VDISORDER_GET_DISORDER_INTERVAL:
	*(int *)buf = ss->disorderInterval;
	return sizeof(int);

      case VDISORDER_SET_TIMEOUT_INTERVAL:
	ss->timeoutInterval = *(int *)buf;
	xTrace1(vdisorderp, TR_EVENTS,
		"VDISORDER controlsessn resets timeout interval to %d",
		ss->arrivalInterval);
	return 0;

      case VDISORDER_GET_TIMEOUT_INTERVAL:
	*(int *)buf = ss->timeoutInterval;
	return sizeof(int);

      default:
	/*
	 * All other opcodes are forwarded to the lower session.  
	 */
	return xControl(xGetDown(s, 0), opcode, buf, len);
    }
}


static int
vdisorderControlProtl( self, opcode, buf, len )
    XObj 	self;
    int 	opcode, len;
    char 	*buf;
{
    /*
     * All opcodes are forwarded to the lower protocol
     */
    return xControl(xGetDown(self, 0), opcode, buf, len);
}


static xkern_return_t
vdisorderOpenEnable( self, hlpRcv, hlpType, p )
    XObj 	self, hlpRcv, hlpType;
    Part 	*p;
{
    PState	*ps = (PState *)self->state;
    
    xTrace0(vdisorderp, TR_MAJOR_EVENTS, "VDISORDER open enable");
    return defaultVirtualOpenEnable(self, ps->passiveMap, hlpRcv, hlpType,
				    self->down, p);
}


static xkern_return_t
vdisorderOpenDisable( self, hlpRcv, hlpType, p )
    XObj 	self, hlpRcv, hlpType;
    Part 	*p;
{
    PState	*ps = (PState *)self->state;

    return defaultVirtualOpenDisable(self, ps->passiveMap, hlpRcv, hlpType,
				     self->down, p);
}


static xkern_return_t
vdisorderClose( s )
    XObj 	s;
{
    PState		*ps;
    xkern_return_t	res;
    XObj		lls;
    SState		*ss = (SState *)s->state;
    int			i;
    
    xTrace1(vdisorderp, TR_MAJOR_EVENTS, "VDISORDER close of session %x", s);
    xAssert(s->rcnt == 0);
    if (ss->disorderEvent)
        evCancel(ss->disorderEvent);
    /* 
     * Free any queued messages without delivering upstairs.
     * This is what happens when you kill a session, right?
     */
    for (i=0; i < ss->disorderCount; i++) {
        msgDestroy(&(ss->disorderMsg[i]));
    }
    ps = (PState *)xMyProtl(s)->state;
    res = mapRemoveBinding(ps->activeMap, s->binding);
    xAssert( res != XK_FAILURE );
    lls = xGetDown(s, 0);
    xAssert(xIsSession(lls));
    xSetUp(lls, xMyProtl(s));
    xClose(lls);
    xDestroy(s);
    return XK_SUCCESS;
}


static xmsg_handle_t
vdisorderPush( s, msg )
    XObj	s;
    Msg 	*msg;
{
    xTrace0(vdisorderp, TR_EVENTS, "vdisorderPush");
    return xPush(xGetDown(s, 0), msg);
}


static xkern_return_t
vdisorderOpenDone( self, lls, llp, hlpType )
    XObj	self, lls, llp, hlpType;
{
    XObj	s;
    PState	*ps = (PState *)self->state;
    Enable	*e;
    
    xTrace0(vdisorderp, TR_MAJOR_EVENTS, "In VDISORDER openDone");
    if ( self == hlpType ) {
	xTrace0(vdisorderp, TR_ERRORS, "self == hlpType in vdisorderOpenDone");
	return XK_FAILURE;
    }
    /*
     * check for openEnable
     */
    
    if ( mapResolve(ps->passiveMap, &hlpType, &e) == XK_FAILURE ) {
	/* 
	 * This shouldn't happen
	 */
	xTrace0(vdisorderp, TR_ERRORS,
		"vdisorderOpenDone: Couldn't find hlp for incoming session");
	return XK_FAILURE;
    }
    if ( (s = vdisorderCreateSessn(self, e->hlpRcv, e->hlpType, lls))
							== ERR_XOBJ ) {
	return XK_FAILURE;
    }
    xDuplicate(lls);
    xTrace0(vdisorderp, TR_EVENTS,
	    "vdisorder Passively opened session successfully created");
    return xOpenDone(e->hlpRcv, s, self);
}


static xkern_return_t
vdisorderProtlDemux( self, lls, m )
    XObj	self, lls;
    Msg		*m;
{
    xTrace0(vdisorderp, TR_ERRORS, "vdisorderProtlDemux called!!");
    return XK_SUCCESS;
}


/* 
 * vdisorderPop and vdisorderSessnDemux must be used (i.e., they can't be
 * bypassed) for the UPI reference count mechanism to work properly. 
 */
static xkern_return_t
vdisorderPop( s, lls, msg, h )
    XObj 	s, lls;
    Msg 	*msg;
    VOID	*h;
{
    SState		*ss = (SState *)s->state;
    xkern_return_t 	res;
    Msg                 *tmp_msg;
    EvCancelReturn	evRes;

    /* don't need a xTrace here, since each state has one first thing */
    switch(ss->disorderState) {

        case VDISORDER_STATE_PASSTHROUGH:
            xTrace0(vdisorderp, TR_EVENTS, "vdisorderPop in state PASSTHROUGH");
	    if ( ++ss->arrivalCount >= ss->arrivalInterval ) {
		xTrace0(vdisorderp, TR_MAJOR_EVENTS, 
                    "vdisorderPop switching states to DISORDERING");
                ss->arrivalCount = 0;
                ss->disorderState = VDISORDER_STATE_DISORDERING;
                ss->disorderEvent = evSchedule(vdisorderTimeout, s, 
                    ss->timeoutInterval * 1000);
	    }
            return xDemux(s, msg);
            break;

        case VDISORDER_STATE_DISORDERING:
            xTrace0(vdisorderp, TR_EVENTS, "vdisorderPop in state DISORDERING");
            xAssert(ss->disorderCount < VDISORDER_MAX_QUEUED_MESSAGES);
	    tmp_msg = &(ss->disorderMsg[ss->disorderCount]);
            xTrace1(vdisorderp, TR_EVENTS, 
                "vdisorderPop placing message in slot %d", ss->disorderCount);
            msgConstructCopy(tmp_msg, msg);
            if (++ss->disorderCount >= ss->disorderInterval)
            {
		xTrace0(vdisorderp, TR_MAJOR_EVENTS, 
                    "vdisorderPop switching states to RELEASE");
                ss->disorderState = VDISORDER_STATE_RELEASE;
            }   
            return XK_SUCCESS;
            break;

        case VDISORDER_STATE_RELEASE:
            xTrace0(vdisorderp, TR_EVENTS, "vdisorderPop in state RELEASE");
            evRes = evCancel(ss->disorderEvent);
            res = xDemux(s, msg);
            vdisorderReleaseMsgs(s);
            return res;
            break;

        default:
            xTrace0(vdisorderp, TR_ERRORS, "vdisorderPop in BAD STATE!");
            return xDemux(s, msg);
            break;

    }
    /* NOTREACHED */
    xTrace0(vdisorderp, TR_ERRORS, "vdisorderPop FELL THROUGH!");
    xAssert(0);

}


static xkern_return_t
vdisorderSessnDemux( self, lls, msg )
    XObj 	self, lls;
    Msg 	*msg;
{
    xTrace0(vdisorderp, TR_EVENTS, "vdisorder Session Demux");
    return xPop(self, lls, msg, 0);
}


static void
sessnInit( s )
    XObj 	s;
{
    xAssert(xIsSession(s));
    
    s->push = vdisorderPush;
    s->pop = vdisorderPop;
    s->close = vdisorderClose;
    s->control = vdisorderControlSessn;
    /* 
     * VDISORDER sessions will look like a protocol to lower sessions, so we
     * need a demux function
     */
    s->demux = vdisorderSessnDemux;
}


static void
protlFuncInit(p)
    XObj p;
{
    xAssert(xIsProtocol(p));

    p->control = vdisorderControlProtl;
    p->open = vdisorderOpen;
    p->openenable = vdisorderOpenEnable;
    p->opendisable = vdisorderOpenDisable;
    p->demux = vdisorderProtlDemux;
    p->opendone = vdisorderOpenDone;
}

static void
vdisorderTimeout(ev, arg)
    Event	ev;
    VOID	*arg;
{

    XObj                s = (XObj)arg;
    SState		*ss = (SState *)s->state;

    xAssert(xIsSession(s));
    xTrace0(vdisorderp, TR_EVENTS, "VDISORDER timeout");
    if (ss->disorderState !=  VDISORDER_STATE_PASSTHROUGH )
    {
        /* got some messages to deliver upstairs */
        vdisorderReleaseMsgs(s);
    }

    if ( evIsCancelled(ev) ) {
        xTrace0(vdisorderp, TR_EVENTS, "vdisorderTimeout cancelled");
        return;
    }

    /*
     * Detach myself
     */
    evDetach(ss->disorderEvent);
}

static void
vdisorderReleaseMsgs(s)
    XObj	s;
{
    SState		*ss = (SState *)s->state;
    int 		msgFreed, msgMask, slot;
    XTime		t;
    Msg 		*tmp_msg;
    xkern_return_t 	res;

    xAssert(xIsSession(s));
    msgFreed = 0;
    msgMask = -1;
    while (msgFreed < ss->disorderCount)
    {
        xGetTime(&t);
        slot = (t.usec % ss->disorderCount);
        xAssert(slot < VDISORDER_MAX_QUEUED_MESSAGES);
        if ((1 << slot) & msgMask)
        {
            /* found one to be released */
            xTrace1(vdisorderp, TR_EVENTS, 
                "disorderReleaseMsgs found message in slot %d", slot);
            /* msg needs shephereding upwards */
            tmp_msg = &(ss->disorderMsg[slot]);
            res = xDemux(s, tmp_msg);
            msgDestroy(tmp_msg);
            msgFreed ++;
            msgMask ^= (1 << slot);
        }
    }
    xTrace0(vdisorderp, TR_MAJOR_EVENTS, 
            "vdisorderReleaseMsgs switching states to PASSTHROUGH");
    ss->disorderState = VDISORDER_STATE_PASSTHROUGH;
    ss->disorderCount = 0;
}
