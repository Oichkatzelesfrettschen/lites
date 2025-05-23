/* 
 * vsize.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.27 $
 * $Date: 1993/07/20 20:10:57 $
 */


#include "xkernel.h"
#include "romopt.h"
#include "vsize.h"
#include "vsize_i.h"

#ifdef __STDC__

static void	protlFuncInit( XObj );
static xkern_return_t	readCutoff( XObj, char **, int, int, VOID * );
static void	sessnInit( XObj );
static XObj	vsizeCreateSessn( XObj, XObj, XObj, XObj * );
static XObj	vsizeOpen( XObj, XObj, XObj, Part * );
static xkern_return_t	vsizeOpenDisable( XObj, XObj, XObj, Part * );
static xkern_return_t	vsizeOpenDone( XObj, XObj, XObj, XObj );
static xkern_return_t	vsizeOpenEnable( XObj, XObj, XObj, Part * );
static xkern_return_t	vsizePop( XObj, XObj, Msg *, VOID * );

#else

static void	protlFuncInit();
static xkern_return_t	readCutoff();
static void	sessnInit();
static XObj	vsizeCreateSessn();

#endif __STDC__

int tracevsizep;


static XObjRomOpt	vsizeOpts[] = {
    { "cutoff", 3, readCutoff }
};


void
vsize_init(self)
    XObj self;
{
    PSTATE	*pstate;
    int		i;
    
    xTrace0(vsizep, TR_GROSS_EVENTS, "VSIZE init");
    xAssert(xIsProtocol(self));
    xAssert(self->numdown <= VSIZEMAXDOWN);
    for (i=0; i< self->numdown; i++ ) { 
        if ( ! xIsProtocol(xGetDown(self, i))) { 
	    xError("vsize down vector is misconfigured");
	    return;
        }
    }
    pstate = X_NEW(PSTATE);
    pstate->numdown = self->numdown; 
    for (i=0; i< pstate->numdown; i++ ) { 
	pstate->cutoff[i] = -1;
    }
    

    self->state = (VOID *)pstate;
    findXObjRomOpts(self, vsizeOpts, sizeof(vsizeOpts)/sizeof(XObjRomOpt), 0);
    protlFuncInit(self);
    pstate->activeMap = mapCreate(11, sizeof(XObj)*pstate->numdown);
    pstate->passiveMap = mapCreate(11, sizeof(PassiveId));
}


static XObj
vsizeOpen(self, hlpRcv, hlpType, p)
    XObj self, hlpRcv, hlpType;
    Part *p;
{
    XObj	s;
    XObj	lls[VSIZEMAXDOWN];
    Part  	savedPart[3];
    PSTATE	*pstate;
    int		plen;
    int 	i,j;
    
    xTrace0(vsizep, TR_MAJOR_EVENTS, "VSIZE open");
    pstate = (PSTATE *)self->state;
    if ( ! p || partLen(p) < 1 || partLen(p) > (3)) { 
	xTrace0(vsizep, TR_ERRORS, "VSIZE open -- bad participants");
	return ERR_XOBJ;
    }								
    /* 
     * Save the original participants before opening since we need to
     * use it in both opens and it will get munged in the first open
     */
    plen = partLen(p) * sizeof(Part);
    bcopy((char *)p, (char *)savedPart, plen);

    for (i=0; i<pstate->numdown; i++) {
    	lls[i] = xOpen(self, hlpType, xGetDown(self, i), p);
    	if ( lls[i] == ERR_XOBJ ) {
	    xTrace0(vsizep, TR_ERRORS, "vsize_open: could not open session");
    	    for (j=0; j<i; j++) {
	        xClose(lls[j]);
	    }
	    return ERR_XOBJ;
        }
        bcopy((char *)savedPart, (char *)p, plen);
    }
    if ( mapResolve(pstate->activeMap, lls, &s) == XK_SUCCESS ) {
	xTrace0(vsizep, TR_MAJOR_EVENTS, "found an existing one");
    	for (i=0; i<pstate->numdown; i++) {
	    xClose(lls[i]);
	}
    } else {
	xTrace0(vsizep, TR_MAJOR_EVENTS, "creating a new one");
	if ( (s = vsizeCreateSessn(self, hlpRcv, hlpType, lls)) == ERR_XOBJ ) {
    	    for (i=0; i<pstate->numdown; i++) {
	        xClose(lls[i]);
	    }
	}
    }
    return s;
}


/* 
 * vsizeCreateSessn --
 * Create and initialize a new VSIZE session using the lls's in the array.
 * Assumes no session already exists corresponding to the lls's in the array
 */
static XObj
vsizeCreateSessn(self, hlpRcv, hlpType, lls)
    XObj self, hlpRcv, hlpType;
    XObj *lls;
{
    XObj	s;
    SSTATE	*sstate;
    PSTATE	*pstate = (PSTATE *)self->state;
    int 	i;
    
    if ( (s = xCreateSessn(sessnInit, hlpRcv, hlpType, self, pstate->numdown, lls))
			== ERR_XOBJ ) {
	xTrace0(vsizep, TR_ERRORS, "create sessn fails in vsizeOpen");
	return ERR_XOBJ;
    }
    sstate = X_NEW(SSTATE);
    s->state = (VOID *)sstate;
    sstate->numdown = pstate->numdown;
    s->binding = mapBind(pstate->activeMap, (char *)lls, (int)s);
    if ( s->binding == ERR_BIND ) {
	xTrace0(vsizep, TR_ERRORS, "mapBind fails in vsizeCreateSessn");
	xClose(s);
	return ERR_XOBJ;
    }
    if ( pstate->cutoff[0] < 0 ) {
	/* 
	 * Determine our cutoff dynamically from the lower session
	 */
	for (i=0; i<pstate->numdown; i++) {
	    if ( xControl(lls[i], GETOPTPACKET, (char *)&sstate->cutoff[i],
		      sizeof(sstate->cutoff)) <= 0 ) {
	        xError("VSIZE could not get lls opt packet size.");
	        xClose(s);
	        return ERR_XOBJ;
	    }
	}
    } else {
	/* 
	 * Use the static cutoff specified at configuration time
	 */
	for (i=0; i<pstate->numdown; i++) {
	    sstate->cutoff[i] = pstate->cutoff[i];
	}
    }
    xTrace1(vsizep, TR_EVENTS, "new VSIZE sessn uses cutoff of %d",
	    sstate->cutoff);
    /*
     * The lower sessions' up fields are made to point to this
     * vsize session (not the protocol)
     */
    for (i=0; i<pstate->numdown; i++) {
        xSetUp(lls[i], s);
    }

    xTrace1(vsizep, TR_MAJOR_EVENTS, "VSIZE open returns %x", s);
    return s;
}


static int
vsizeControlSessn(s, opcode, buf, len)
    XObj s;
    int opcode;
    char *buf;
    int len;
{
    SSTATE	*sstate = (SSTATE *)s->state;
    xTrace0(vsizep, TR_EVENTS, "VSIZE controlsessn");

    switch ( opcode ) {

      /*
       * All other opcodes are forwarded to the long-pckt session.  
       */
      default:
      return xControl(xGetDown(s, sstate->numdown-1), opcode, buf, len);
    }

}


static int
vsizeControlProtl(self, opcode, buf, len)
    XObj self;
    int opcode;
    char *buf;
    int len;
{
    PSTATE	*pstate = (PSTATE *)self->state;
    /*
     * All opcodes are forwarded to the long-pckt protocol
     */
    return xControl(xGetDown(self, pstate->numdown-1), opcode, buf, len);
}


static xkern_return_t
vsizeOpenEnable( self, hlpRcv, hlpType, p )
    XObj self, hlpRcv, hlpType;
    Part *p;
{
    PSTATE	*ps = (PSTATE *)self->state;
    
    xTrace0(vsizep, TR_MAJOR_EVENTS, "VSIZE open enable");
    return defaultVirtualOpenEnable(self, ps->passiveMap, hlpRcv, hlpType,
				    self->down, p);
}


static xkern_return_t
vsizeOpenDisable(self, hlpRcv, hlpType, p)
    XObj self, hlpRcv, hlpType;
    Part *p;
{
    PSTATE	*ps = (PSTATE *)self->state;

    return defaultVirtualOpenDisable(self, ps->passiveMap, hlpRcv, hlpType,
				     self->down, p);
}


static xkern_return_t
vsizeClose(s)
    XObj s;
{
    PSTATE		*pstate;
    xkern_return_t	res;
    int i;
    
    xTrace1(vsizep, TR_MAJOR_EVENTS, "VSIZE close of session %x", s);
    xAssert(s->rcnt <= 0);
    pstate = (PSTATE *)s->myprotl->state;
    if ( s->binding != ERR_BIND ) {
	res = mapRemoveBinding(pstate->activeMap, s->binding);
	xAssert( res != XK_FAILURE );
    }
    for (i=0; i<pstate->numdown; i++) { 
        xSetUp(xGetDown(s, i), xMyProtl(s));
        xClose(xGetDown(s, i));
    }
    xDestroy(s);
    return XK_SUCCESS;
}


static xmsg_handle_t
vsizePush(s, msg)
    XObj s;
    Msg *msg;
{
    SSTATE	*sstate;
    int i;
    
    xTrace0(vsizep, TR_EVENTS, "in vsize push");
    sstate = (SSTATE *)s->state;
    for (i=0; i<sstate->numdown-1; i++) {
        if ( msgLen(msg) <= sstate->cutoff[i] ) {
	    xTrace1(vsizep, TR_MORE_EVENTS,
		"vsize_push: pushing to session[%d]",i);
	    xAssert(xIsSession(xGetDown(s, i)));
	    return xPush(xGetDown(s, i), msg);
        }
    }
    xTrace1(vsizep, TR_MORE_EVENTS,
	"vsize_push: pushing to session[%d]",sstate->numdown-1);
    xAssert(xIsSession(xGetDown(s, sstate->numdown-1)));
    return xPush(xGetDown(s, sstate->numdown-1), msg);
}


static xkern_return_t
vsizeOpenDone(self, lls, llp, hlpType)
    XObj self, lls, llp, hlpType;
{
    XObj	s;
    Part 	p[2];
    char	partBuf[100];
    PSTATE	*pstate;
    XObj	llsArray[VSIZEMAXDOWN];
    Enable	*e;
    int		i,j;
    int		llsIndex;
    
    xTrace0(vsizep, TR_MAJOR_EVENTS, "In VSIZE openDone");
    if ( self == hlpType ) {
	xTrace0(vsizep, TR_ERRORS, "self == hlpType in vsizeOpenDone");
	return XK_FAILURE;
    }
    pstate = (PSTATE *)self->state;
    /* 
     * Figure out which of my lower protocols owns the lower session
     */

    llsIndex=-1;
    for (i=0; i<pstate->numdown; i++) {
        if ( llp == xGetDown(self, i) ) {
	    llsIndex = i;
        } 
    }
    if (llsIndex == -1) {
	xError("Impossible lower level session in vsize openDone");
	return XK_FAILURE;
    }
    /*
     * check for open enables
     */
    if ( mapResolve(pstate->passiveMap, &hlpType, &e) == XK_FAILURE ) {
	/* 
	 * This shouldn't happen
	 */
	xTrace0(vsizep, TR_ERRORS,
		"vsizeOpenDone: Couldn't find hlp for incoming session");
	return XK_FAILURE;
    }
    /* 
     * Open the lls for the other protocol (not the one which owns
     * lls) using participants from a control op on lls
     */
    if ( xControl(lls, GETPARTICIPANTS, partBuf, sizeof(partBuf)) <= 0 ) {
	xTrace0(vsizep, TR_ERRORS,
		"Could not get participants from lls in vsizeOpenDone");
	return XK_FAILURE;
    }
    for (i=0; i<pstate->numdown; i++) {
        partInternalize(p, partBuf);
	if (i == llsIndex) {
            llsArray[i] = lls;
	} else {
            llsArray[i] = xOpen(self, hlpType, xGetDown(self, i), p);
            if ( llsArray[i] == ERR_XOBJ ) {
	        xTrace1(vsizep, TR_ERRORS, "vsizeOpenDone couldn't open lls[%d]",i);
                for (j=0; j<i; j++) {
	            xClose(llsArray[j]);
                }
	        return XK_FAILURE;
            }
        }
    }
    xDuplicate(lls);
    if ( (s = vsizeCreateSessn(self, e->hlpRcv, e->hlpType, llsArray))
		== ERR_XOBJ ) {
        for (i=0; i<pstate->numdown; i++) {
	    xClose(llsArray[i]);
        }
	return XK_FAILURE;
    }
    xTrace0(vsizep, TR_EVENTS,
	    "vsize Passively opened session successfully created");
    return xOpenDone(e->hlpRcv, s, self);
}


static xkern_return_t
vsizeProtlDemux( self, lls, m )
    XObj	self, lls;
    Msg		*m;
{
    xTrace0(vsizep, TR_ERRORS, "vsizeProtlDemux called!!");
    return XK_SUCCESS;
}


/* 
 * vsizePop and vsizeSessnDemux must be used (i.e., they can't be
 * bypassed) for the UPI reference count mechanism to work properly. 
 */
static xkern_return_t
vsizePop(self, lls, msg, h)
    XObj self;
    XObj lls;
    Msg *msg;
    VOID *h;
{
    xTrace0(vsizep, TR_EVENTS, "vsize Pop");
    return xDemux(self, msg);
}


static xkern_return_t
vsizeSessnDemux(self, lls, msg)
    XObj self;
    XObj lls;
    Msg *msg;
{
    xTrace0(vsizep, TR_EVENTS, "vsize Session Demux");
    return xPop(self, lls, msg, 0);
}


static void
sessnInit(s)
    XObj s;
{
    xAssert(xIsSession(s));
    
    s->push = vsizePush;
    s->pop = vsizePop;
    s->close = vsizeClose;
    s->control = vsizeControlSessn;
    /* 
     * VSIZE sessions will look like a protocol to lower sessions, so we
     * need a demux function
     */
    s->demux = vsizeSessnDemux;
}


static void
protlFuncInit(p)
    XObj p;
{
    xAssert(xIsProtocol(p));

    p->control = vsizeControlProtl;
    p->open = vsizeOpen;
    p->openenable = vsizeOpenEnable;
    p->opendisable = vsizeOpenDisable;
    p->demux = vsizeProtlDemux;
    p->opendone = vsizeOpenDone;
}


static xkern_return_t
readCutoff( self, str, nFields, line, arg )
    XObj	self;
    char	**str;
    int		nFields, line;
    VOID	*arg;
{
    PSTATE	*ps = (PSTATE *)self->state;
    int i,val;

    for (i=2; i<nFields; i++) {
#ifdef XKMACHKERNEL
        val = sscanf1 (str[i], "%d", &ps->cutoff[i-2]);
#else
        val = sscanf (str[i], "%d", &ps->cutoff[i-2]);
#endif
        if (val < 1) {
	    return XK_FAILURE;
	}
    }
    return XK_SUCCESS;
}	

