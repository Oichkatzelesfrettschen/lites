/* 
 * vtap.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993  Arizona Board of Regents
 *
 *
 */


#include "xkernel.h"
#include "vtap.h"
#include "vtap_i.h"

#ifdef __STDC__

static void		protlFuncInit( XObj );
static void		sessnInit( XObj );
static XObj		vtapCreateSessn( XObj, XObj, XObj, XObj );
static XObj		vtapOpen( XObj, XObj, XObj, Part * );
static xkern_return_t	vtapOpenDisable( XObj, XObj, XObj, Part * );
static xkern_return_t	vtapOpenDone( XObj, XObj, XObj, XObj );
static xkern_return_t	vtapOpenEnable( XObj, XObj, XObj, Part * );

#else

static void		protlFuncInit();
static void		sessnInit();
static XObj		vtapCreateSessn();
static XObj		vtapOpen();
static xkern_return_t	vtapOpenDisable();
static xkern_return_t	vtapOpenDone();
static xkern_return_t	vtapOpenEnable();

#endif __STDC__


int tracevtapp;


void
vtap_init( self )
    XObj 	self;
{
    PState	*ps;
    
    xTrace0(vtapp, TR_GROSS_EVENTS, "VTAP init");
    xAssert(xIsProtocol(self));
    if ( ! xIsProtocol(xGetDown(self, 0)) ) {
	xError("VTAP down vector is misconfigured");
	return;
    }
    ps = X_NEW(PState);
    self->state = (VOID *)ps;
    protlFuncInit(self);
    ps->activeMap = mapCreate(VTAP_ACT_MAP_SZ, sizeof(XObj));
    ps->passiveMap = mapCreate(VTAP_PAS_MAP_SZ, sizeof(XObj));
}


static XObj
vtapOpen( self, hlpRcv, hlpType, p )
    XObj	self, hlpRcv, hlpType;
    Part 	*p;
{
    XObj	s, lls;
    PState	*ps = (PState *)self->state;
    
    xTrace0(vtapp, TR_MAJOR_EVENTS, "VTAP open");
    if ( (lls = xOpen(self, hlpType, xGetDown(self, 0), p)) == ERR_XOBJ ) {
	xTrace0(vtapp, TR_ERRORS, "vtapOpen: could not open lower session");
	return ERR_XOBJ;
    }
    if ( mapResolve(ps->activeMap, &lls, &s) == XK_SUCCESS ) {
	xTrace0(vtapp, TR_MORE_EVENTS, "vtapOpen -- found an existing one");
    } else {
	xTrace0(vtapp, TR_MAJOR_EVENTS, "vtapOpen -- creating a new one");
	if ( (s = vtapCreateSessn(self, hlpRcv, hlpType, lls)) == ERR_XOBJ ) {
	    xClose(lls);
	}
    }
    return s;
}


static XObj
vtapCreateSessn( self, hlpRcv, hlpType, lls )
    XObj 	self, hlpRcv, hlpType, lls;
{
    XObj	s;
    PState	*ps = (PState *)self->state;
    SState	*ss;
    XTime	t;
    
    if ( (s = xCreateSessn(sessnInit, hlpRcv, hlpType, self, 1, &lls))
			== ERR_XOBJ ) {
	xTrace0(vtapp, TR_ERRORS, "create sessn fails in vtapOpen");
	return ERR_XOBJ;
    }
    s->binding = mapBind(ps->activeMap, (char *)&lls, (int)s);
    if ( s->binding == ERR_BIND ) {
	xTrace0(vtapp, TR_ERRORS, "mapBind fails in vtapCreateSessn");
	return ERR_XOBJ;
    }
    /*
     * The lower sessions' up field is made to point to this
     * vtap session (not the protocol)
     */
    xSetUp(lls, s);
    ss = X_NEW(SState);
    s->state = (VOID *)ss;
    xGetTime(&t);
    ss->tap_enabled = 1;
    ss->print_chars = 0;
    xTrace0(vtapp, TR_EVENTS, "VTAP the tap is enabled");
    xTrace0(vtapp, TR_EVENTS, "VTAP will print hex values");

    xTrace1(vtapp, TR_MAJOR_EVENTS, "VTAP open returns %x", s);
    return s;
}


static int
vtapControlSessn( s, opcode, buf, len )
    XObj 	s;
    int 	opcode, len;
    char 	*buf;
{
    SState	*ss = (SState *)s->state;

    xTrace0(vtapp, TR_EVENTS, "VTAP controlsessn");
    switch ( opcode ) {

      case VTAP_ENABLETAP:
	ss->tap_enabled = 1;
	xTrace0(vtapp, TR_EVENTS,
		"VTAP controlsessn enabled the tap");
	return 0;

      case VTAP_DISABLETAP:
	ss->tap_enabled = 0;
	xTrace0(vtapp, TR_EVENTS,
		"VTAP controlsessn disabled the tap");
	return 0;

      case VTAP_PRINTCHARS:
	ss->print_chars = 1;
	xTrace0(vtapp, TR_EVENTS,
		"VTAP controlsessn will print characters");
	return 0;

      case VTAP_PRINTHEX:
	ss->print_chars = 0;
	xTrace0(vtapp, TR_EVENTS,
		"VTAP controlsessn will print hex values");
	return 0;

      default:
	/*
	 * All other opcodes are forwarded to the lower session.  
	 */
	return xControl(xGetDown(s, 0), opcode, buf, len);
    }
}


static int
vtapControlProtl( self, opcode, buf, len )
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
vtapOpenEnable( self, hlpRcv, hlpType, p )
    XObj 	self, hlpRcv, hlpType;
    Part 	*p;
{
    PState	*ps = (PState *)self->state;
    
    xTrace0(vtapp, TR_MAJOR_EVENTS, "VTAP open enable");
    return defaultVirtualOpenEnable(self, ps->passiveMap, hlpRcv, hlpType,
				    self->down, p);
}


static xkern_return_t
vtapOpenDisable( self, hlpRcv, hlpType, p )
    XObj 	self, hlpRcv, hlpType;
    Part 	*p;
{
    PState	*ps = (PState *)self->state;

    return defaultVirtualOpenDisable(self, ps->passiveMap, hlpRcv, hlpType,
				     self->down, p);
}


static xkern_return_t
vtapClose( s )
    XObj 	s;
{
    PState		*ps;
    xkern_return_t	res;
    XObj		lls;
    
    xTrace1(vtapp, TR_MAJOR_EVENTS, "VTAP close of session %x", s);
    xAssert(s->rcnt == 0);
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



long vtapPrintMsgHex(dest, src, len, ignore)
     void *dest;
     char *src;
     long len;
     void *ignore;
{
  int i;

  xTrace0(vtapp,TR_FULL_TRACE,"vtapPrintMsg");
  printf("Wiretap: len=%d  data=0x", len);
  for(i=0; i < len; i++)
    printf("%x", (u_char)src[i]);
  printf("\n");
  return 0;
}

long vtapPrintMsgChar(dest, src, len, ignore)
     void *dest;
     char *src;
     long len;
     void *ignore;
{
  int i;

  xTrace0(vtapp,TR_FULL_TRACE,"vtapPrintMsg");
  printf("Wiretap: len=%d  data=", len);
  for(i=0; i < len; i++)
    printf("%c", src[i]);
  printf("\n");
  return 0;
}


static xmsg_handle_t
vtapPush( s, msg )
    XObj	s;
    Msg 	*msg;
{
  int len = msgLen(msg);
  
  
  xTrace0(vtapp, TR_EVENTS, "vtapPush");

  if ( ((SState *)s->state)->tap_enabled )
    {
      if ( ((SState *)s->state)->print_chars )
	msgPop(msg, vtapPrintMsgChar, 0, len, 0);
      else
	msgPop(msg, vtapPrintMsgHex, 0, len, 0);
    }

  return xPush(xGetDown(s, 0), msg);
}



static xkern_return_t
vtapOpenDone( self, lls, llp, hlpType )
    XObj	self, lls, llp, hlpType;
{
    XObj	s;
    PState	*ps = (PState *)self->state;
    Enable	*e;
    
    xTrace0(vtapp, TR_MAJOR_EVENTS, "In VTAP openDone");
    if ( self == hlpType ) {
	xTrace0(vtapp, TR_ERRORS, "self == hlpType in vtapOpenDone");
	return XK_FAILURE;
    }
    /*
     * check for openEnable
     */
    
    if ( mapResolve(ps->passiveMap, &hlpType, &e) == XK_FAILURE ) {
	/* 
	 * This shouldn't happen
	 */
	xTrace0(vtapp, TR_ERRORS,
		"vtapOpenDone: Couldn't find hlp for incoming session");
	return XK_FAILURE;
    }
    if ( (s = vtapCreateSessn(self, e->hlpRcv, e->hlpType, lls))
							== ERR_XOBJ ) {
	return XK_FAILURE;
    }
    xDuplicate(lls);
    xTrace0(vtapp, TR_EVENTS,
	    "vtap Passively opened session successfully created");
    return xOpenDone(e->hlpRcv, s, self);
}


static xkern_return_t
vtapProtlDemux( self, lls, m )
    XObj	self, lls;
    Msg		*m;
{
    xTrace0(vtapp, TR_ERRORS, "vtapProtlDemux called!!");
    return XK_SUCCESS;
}


/* 
 * vtapPop and vtapSessnDemux must be used (i.e., they can't be
 * bypassed) for the UPI reference count mechanism to work properly. 
 */
static xkern_return_t
vtapPop( s, lls, msg, h )
    XObj 	s, lls;
    Msg 	*msg;
    VOID	*h;
{
  int            length = msgLen(msg);
  
  xTrace0(vtapp, TR_EVENTS, "vtap Pop");
  
  if ( ((SState *)s->state)->tap_enabled )
    {
      if ( ((SState *)s->state)->print_chars )
	msgPop(msg, vtapPrintMsgChar, 0, length, 0);
      else
	msgPop(msg, vtapPrintMsgHex, 0, length, 0);
    }
  
  return xDemux(s, msg);

}


static xkern_return_t
vtapSessnDemux( self, lls, msg )
    XObj 	self, lls;
    Msg 	*msg;
{
    xTrace0(vtapp, TR_EVENTS, "vtap Session Demux");
    return xPop(self, lls, msg, 0);
}


static void
sessnInit( s )
    XObj 	s;
{
    xAssert(xIsSession(s));
    
    s->push = vtapPush;
    s->pop = vtapPop;
    s->close = vtapClose;
    s->control = vtapControlSessn;
    /* 
     * VTAP sessions will look like a protocol to lower sessions, so we
     * need a demux function
     */
    s->demux = vtapSessnDemux;
}


static void
protlFuncInit(p)
    XObj p;
{
    xAssert(xIsProtocol(p));

    p->control = vtapControlProtl;
    p->open = vtapOpen;
    p->openenable = vtapOpenEnable;
    p->opendisable = vtapOpenDisable;
    p->demux = vtapProtlDemux;
    p->opendone = vtapOpenDone;
}
