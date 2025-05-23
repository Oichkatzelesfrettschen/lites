/*
 * join.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/06/24 20:47:34 $
 */

#include "xkernel.h"
#include "join.h"
#include "join_i.h"


#ifdef __STDC__

static XObj		joinCreateSessn( XObj, XObj, XObj, XObj);
static XObj 		joinCreateTagSessn (XObj, XObj, XObj, XObj, char *, int);
static void		joinSessnInit( XObj );
static void		joinTagSessnInit( XObj );
static int		joinControlProtl( XObj, int, char *, int );
static xkern_return_t	joinDemux( XObj, XObj, Msg * );
static XObj		joinOpen( XObj, XObj, XObj, Part * );
static xkern_return_t	joinOpenEnable( XObj, XObj, XObj, Part * );
static xkern_return_t	joinOpenDisable( XObj, XObj, XObj, Part * );
static xkern_return_t	joinOpenDone( XObj, XObj, XObj, XObj);
static xkern_return_t	joinClose( XObj );
static xkern_return_t	joinTagClose( XObj );
static xmsg_handle_t	joinPush( XObj, Msg * );
static xmsg_handle_t	joinTagPush( XObj, Msg * );
static xkern_return_t	joinPop( XObj, XObj, Msg *, void * );
static xkern_return_t	joinTagPop( XObj, XObj, Msg *, void * );
static int		joinControlSessn( XObj, int, char *, int );
static int		joinTagControlSessn( XObj, int, char *, int );
static int 		part2ToId(Part *, char *, int);
static int 		partToId(Part *, char *, int);
static long 		joinHdrLoad1(VOID *, char *, long int, VOID *);
static long 		joinHdrLoad2(VOID *, char *, long int, VOID *);
static void 		joinHdrStore(VOID *, char *, long int, VOID *);
static int 		isOrderValid( int *, int);
static void 		processRomFile(XObj);

#else

static XObj		joinCreateSessn();
static XObj 		joinCreateTagSessn ();
static void		joinSessnInit();
static void		joinTagSessnInit();
static int		joinControlProtl();
static xkern_return_t	joinDemux();
static XObj		joinOpen();
static xkern_return_t	joinOpenEnable();
static xkern_return_t	joinOpenDisable();
static xkern_return_t	joinOpenDone();
static xkern_return_t	joinClose();
static xkern_return_t	joinTagClose();
static xmsg_handle_t	joinPush();
static xmsg_handle_t	joinTagPush();
static xkern_return_t	joinPop();
static xkern_return_t	joinTagPop();
static int		joinControlSessn();
static int		joinTagControlSessn();
static int 		part2ToId();
static int 		partToId();
static long 		joinHdrLoad1();
static long 		joinHdrLoad2();
static void 		joinHdrStore();
static int 		isOrderValid( int *, int);
static void 		processRomFile();

#endif __STDC__

int tracejoinp;

void
join_init( self )
    XObj self;
{
    PState	*ps;
    XObj	llp;
    int 	i;

    xTrace0(joinp, TR_EVENTS, "join_init entered");
    xAssert(xIsProtocol(self));
    
    if ( ! xIsProtocol(llp = xGetDown(self, 0)) ) {
	xError("join_init: cannot get lower protocol object");
	return;
    }

    ps = X_NEW(PState);
    bzero((char *)ps,sizeof(ps));
    self->state = (VOID *)ps;
    ps->actMap = mapCreate(JOIN_ACTIVE_MAP_SZ, sizeof(ActiveId));
    ps->tagIdLen = 0;
    ps->pasIdLen = 0;
    ps->numSeg = DEFAULT_NUMSEG;
    for (i=0; i<JOINMAXSEG; i++) {
        ps->order[i] =i;
    } 
    processRomFile(self);

    xTrace1(joinp, TR_EVENTS, "join_init numSeg %d", ps->numSeg);

    if (!isOrderValid(ps->order,ps->numSeg)) {
	Kabort("joinControlProtl: invalid order");
    }

    self->control = joinControlProtl;
    self->open = joinOpen;
    self->openenable = joinOpenEnable;
    self->opendisable = joinOpenDisable;
    self->opendone = joinOpenDone;
    self->close = joinClose;  
    self->demux = joinDemux;

    xTrace0(joinp, TR_EVENTS, "join_init exiting");
}



static XObj
joinOpen( self, hlpRcv, hlpType, part )
    XObj	self, hlpRcv, hlpType;
    Part 	*part;
{
    XObj 	joinSessn;
    XObj 	tagSessn;
    XObj	lls;
    PState	*ps = (PState *)self->state;
    char	idBuf[MAXID];
    char	partBuf[MAXID*2];
    Part	fullPart[MAXPART];
    int		idLen;
    
    xTrace0(joinp, TR_EVENTS, "joinOpen entered");

    if ( part == 0 || partLen(part) < 1 || partLen(part) > 2 ) {
	xTrace0(joinp, TR_SOFT_ERRORS, "joinOpen -- bad participants");
	return ERR_XOBJ;
    }

    idLen = partToId(part,idBuf, MAXID);

    if ((ps->tagIdLen == 0) || 
        (idLen != ps->tagIdLen) ||
        (mapResolve(ps->tagMap,idBuf, &tagSessn) != XK_SUCCESS)) {

	/* new connection required */
    	xTrace0(joinp, TR_EVENTS, "joinOpen-new connection");
        lls = xOpen(self, hlpType, xGetDown(self, 0), part);
        if ( lls == ERR_XOBJ ) {
	    xTrace0(joinp, TR_SOFT_ERRORS, "joinOpen -- could not open llp");
            return ERR_XOBJ;
        }
        if (xControl(lls, GETPARTICIPANTS, partBuf, MAXID*2) <= 0) {
	    xTrace0(joinp, TR_SOFT_ERRORS, "joinOpen -- could not getparticipants");
	    xClose(lls);
            return ERR_XOBJ;
        }
    	partInternalize(fullPart,partBuf);
    	idLen = partToId(fullPart,idBuf,MAXID); 
	if (idLen < 1) {
	    xTrace0(joinp, TR_SOFT_ERRORS, "joinOpen -- bad participants");
	    xClose(lls);
            return ERR_XOBJ;
        }

        tagSessn = joinCreateTagSessn(self, hlpRcv, hlpType, lls, idBuf, idLen);
        if (tagSessn == ERR_XOBJ) {
	    xTrace0(joinp, TR_SOFT_ERRORS, "joinOpen -- could not create tag session");
            return ERR_XOBJ;
        }
    	xTrace0(joinp, TR_EVENTS, "joinOpen-exiting");
        return tagSessn;
    }

    joinSessn = joinCreateSessn(self, hlpRcv, hlpType, tagSessn);
    if (joinSessn == ERR_XOBJ) {
	xTrace0(joinp, TR_SOFT_ERRORS, "joinOpen -- could not create join session");
        return ERR_XOBJ;
    }

    xTrace0(joinp, TR_EVENTS, "joinOpen-exiting");
    return joinSessn;
}

static XObj
joinCreateTagSessn (self, hlpRcv, hlpType, lls, idBuf, idLen)
    XObj        self, hlpRcv, hlpType, lls;
    char	*idBuf;
    int		idLen;
{
    XObj        s;
    TState      *ts;
    PState      *ps = (PState *)self->state;

    xTrace0(joinp, TR_EVENTS, "joinCreateTagSessn - entered");
    if (ps->tagIdLen == 0) {
        xTrace1(joinp, TR_EVENTS, "joinCreateTagSessn - creating new map idLen %d",idLen);
    	ps->tagMap = mapCreate(JOIN_TAG_MAP_SZ, idLen);
        ps->tagIdLen = idLen;
    }

    if (idLen != ps->tagIdLen) {
        xTrace0(joinp, TR_ERRORS, "joinCreateTagSessn - bad key size");
        return ERR_XOBJ;
    }
       
    ts = X_NEW(TState);
    bzero((char *)ts, sizeof(TState));
    semInit(&ts->wait, 1);
    bcopy(idBuf,(char *)&ts->id,idLen);
    ts->numSeg = ps->numSeg;
    bcopy((char *) &ps->order, (char *)&ts->order, sizeof(ts->order));
 

    s = xCreateSessn(joinTagSessnInit, hlpRcv, hlpType, self, 1, &lls);
    s->rcnt=1;
    s->binding = (Bind) mapBind(ps->actMap, (char *)&lls, (int)s);
    if ( s->binding == ERR_BIND ) {
        xTrace0(joinp, TR_ERRORS, "joinCreateTagSessn - could not bind actMap");
        xDestroy(s);
        xFree((char *) ts);
        return ERR_XOBJ;
      }
    ts->tagBinding = (Bind) mapBind(ps->tagMap, (char *)ts->id, (int)s);
    if ( ts->tagBinding == ERR_BIND ) {
        xTrace0(joinp, TR_ERRORS, "joinCreateTagSessn - could not bind tagMap");
	mapRemoveBinding(ps->tagMap,ts->tagBinding);
	s->binding = 0;
        xDestroy(s);
        xFree((char *) ts);
        return ERR_XOBJ;
      }

    s->state = (VOID *) ts;

    xTrace0(joinp, TR_EVENTS, "joinCreateTagSessn - exiting");
    return(s);
}



static XObj
joinCreateSessn (self, hlpRcv, hlpType, tagSessn)
    XObj	self, hlpRcv, hlpType, tagSessn;
{
    XObj	s;
    SState	*ss;
    TState	*ts = (TState *)tagSessn->state;

    xTrace1(joinp, TR_EVENTS, "joinCreateSessn - entered numSeg = %d", ts->numSeg);

    if (ts->state != JOININIT) {
        xTrace0(joinp, TR_ERRORS, "joinCreateSessn - to many opens");
	return ERR_XOBJ;
    }
    s = xCreateSessn(joinSessnInit, hlpRcv, hlpType, self, 1, &tagSessn);
    ss = X_NEW(SState);
    s->state = (VOID *) ss;
    ts->up[ts->next] = s;
    ss->seg = ts->next;
    ts->next++;
    tagSessn->rcnt++;
    if (ts->next == ts->numSeg) ts->state = JOINREADY;


    return s;
}


static xkern_return_t
joinOpenEnable(self, hlpRcv, hlpType, part)
    XObj self, hlpRcv, hlpType;
    Part *part;
{
    PState	*ps = (PState *)self->state;
    int		idLen;
    char 	buf[MAXID];
    JEnable	*e;
    xkern_return_t val;

    xTrace1(joinp, TR_EVENTS, "joinOpenEnable - entered ps->pasIdLen = %d",ps->pasIdLen);

    idLen = partToId(part,buf, MAXID);

    if (ps->pasIdLen == 0) {
        xTrace2(joinp, TR_EVENTS, "joinOpenEnable - creating new passive map idlen = %d ps->pasIdLen = %d", idLen, ps->pasIdLen);
    	ps->pasMap = mapCreate(JOIN_PASSIVE_MAP_SZ, idLen);
        ps->pasIdLen = idLen;
    }

    if (idLen != ps->pasIdLen) {
        xTrace0(joinp, TR_ERRORS, "joinOpenEnable - bad idLen");
        return XK_FAILURE;
    }

    if ( mapResolve(ps->pasMap, buf, &e) != XK_FAILURE ) {
        xTrace0(joinp, TR_EVENTS, "joinOpenEnable - enable object found");
        if ( e->next < ps->numSeg + 1) {
            xTrace0(joinp, TR_EVENTS, "joinOpenEnable - add new hlp");
	    e->hlpRcv[e->next] = hlpRcv;
	    e->hlpType[e->next] = hlpType;
            e->next++;
	    e->rcnt++;
            return XK_SUCCESS;
        }
        xTrace0(joinp, TR_ERRORS, "joinOpenEnable - to many open enables");
        return XK_FAILURE;
    }

    xTrace0(joinp, TR_EVENTS, "joinOpenEnable - openenable llp");
    val = xOpenEnable(self, hlpType, xGetDown(self,0), part);
    if ( val == XK_FAILURE ) {
        xTrace0(joinp, TR_ERRORS, "joinOpenEnable - could not openenable llp");
        return XK_FAILURE;
    }

    xTrace0(joinp, TR_EVENTS, "joinOpenEnable - create enable object");
    e = (JEnable *)xMalloc(sizeof(JEnable));
    e->saveHlpType = hlpType;
    e->hlpRcv[0] = hlpRcv;
    e->hlpType[0] = hlpType;
    e->next = 1;
    e->rcnt = 1;
    e->binding = mapBind(ps->pasMap, (char *)buf, (int) e);

    if (e->binding == ERR_BIND) {
      xTrace0(joinp, TR_ERRORS, "joinOpenEnable - could not bind enable object");
      xFree((char *)e);
      return XK_FAILURE;
    }
    xTrace0(joinp, TR_EVENTS, "joinOpenEnable - exiting");
    return XK_SUCCESS;

}


static xkern_return_t
joinOpenDisable(self, hlpRcv, hlpType, part)
    XObj self, hlpRcv, hlpType;
    Part *part;
{
    PState	*ps = (PState *)self->state;
    int		idLen;
    char 	buf[MAXID];
    JEnable	*e;
    int 	i;
    XObj	hlp;

    

    xTrace0(joinp, TR_EVENTS, "joinOpenDisable - entered");

    idLen = partToId(part,buf, MAXID);

    if (idLen != ps->pasIdLen) {
        xTrace0(joinp, TR_ERRORS, "joinOpenEnable - bad idLen");
        return XK_FAILURE;
    }

    if ( mapResolve(ps->pasMap, buf, &e) != XK_FAILURE ) {
        xTrace0(joinp, TR_EVENTS, "joinOpenDisable - enable object found");
        for (i=0; i<ps->numSeg + 1; i++) {
	    if ((e->hlpRcv[i] == hlpRcv) && (e->hlpType[i] == hlpType)) {
                xTrace0(joinp, TR_EVENTS, "joinOpenDisable - valid hlp");
        	e->rcnt--;
	    	e->hlpRcv[i] = ERR_XOBJ;
	    	e->hlpType[i] = ERR_XOBJ;
	    }
  	}
       	if ( e->rcnt < 1 ) {
            xTrace0(joinp, TR_EVENTS, "joinOpenDisable - removing e");
   	    mapRemoveBinding(ps->pasMap, e->binding);
	    hlp = e->saveHlpType;
	    xFree((char *)e);
    	    return xOpenDisable(self, hlp, xGetDown(self,0), part);
	} else {
            xTrace0(joinp, TR_EVENTS, "joinOpenDisable - rcnt>0");
    	    return XK_SUCCESS;
	}
    }
    return XK_FAILURE;

}

static xkern_return_t
joinOpenDone(self, lls, llp, hlpType)
    XObj self, lls, llp, hlpType;
{
    XObj        tagSessn;
    XObj        joinSessn;
    PState      *ps;
    JEnable      *e;
    char	pBuf[MAXID*2];
    Part	p[5];
    char	idBuf[MAXID];
    int		idLen;
    xkern_return_t val;
    int		i;
   
    xTrace0(joinp, TR_EVENTS, "joinOpenDone - entered");

    if ( self == hlpType ) {
        xTrace0(joinp, TR_ERRORS, "self == hlpType in joinOpenDone");
        return XK_FAILURE;
    }

    ps = (PState *)self->state;

    if (xControl(lls,GETPARTICIPANTS,pBuf,MAXID*2) < 1) {
        xTrace0(joinp, TR_ERRORS, "joinOpenDone - getparticpants fails");
       return XK_FAILURE;
    }

    partInternalize(p,pBuf);
    idLen= part2ToId(p,idBuf,MAXID);
    
    if ((idLen != ps->pasIdLen) ||
        (mapResolve(ps->pasMap, &idBuf, &e) == XK_FAILURE )) {
        xTrace0(joinp, TR_ERRORS, "joinOpenDone - no passive open");
        return XK_FAILURE;
    }

    xDuplicate(lls);

    idLen = partToId(p,idBuf,MAXID);

    tagSessn = joinCreateTagSessn(self,e->hlpRcv[0],e->hlpType[0], lls, idBuf, idLen);
    if (tagSessn == ERR_XOBJ) {
        xTrace0(joinp, TR_ERRORS, "joinOpenDone - could not create tag session");
        xClose(lls);
        return XK_FAILURE;
    }
    val = xOpenDone(e->hlpRcv[0], tagSessn, self);
    if (val != XK_SUCCESS) {
        xTrace0(joinp, TR_ERRORS, "joinOpenDone - could not opendone hlp");
        xClose(lls);
        return XK_FAILURE;
    }

    for (i=1; i<ps->numSeg +1; i++) {
        joinSessn = joinCreateSessn(self, e->hlpRcv[i], e->hlpType[i], tagSessn);
	if (joinSessn == ERR_XOBJ) {
            xTrace0(joinp, TR_ERRORS, "joinOpenDone - could not create join session");
            xClose(lls);
            return XK_FAILURE;
        }

        val = xOpenDone(e->hlpRcv[i], joinSessn, self);
	if (val != XK_SUCCESS) {
            xTrace0(joinp, TR_ERRORS, "joinOpenDone - could not opendone hlp");
            xClose(lls);
            return XK_FAILURE;
        }
    }
    xTrace0(joinp, TR_EVENTS, "joinOpenDone - exiting");
    return val;
}



static xkern_return_t
joinDemux( self, lls, msg )
    XObj	self, lls;
    Msg		*msg;
{
  PState	*ps = (PState *)self->state;
  XObj 	s = 0;
  xkern_return_t val;
  
  xTrace0(joinp, TR_EVENTS, "joinDemux - entered");

  if ( mapResolve(ps->actMap, &lls, &s) == XK_FAILURE ) { 
	xTrace0(joinp, TR_EVENTS, " joinDemux - map resolve fails");
  	return XK_FAILURE;
  }

  val = xPop(s, lls, msg, 0);

  xTrace0(joinp, TR_EVENTS, "joinDemux - exiting");

  return val;
}


static xkern_return_t
joinClose( s )
    XObj	s;
{
    SState *ss;
    TState *ts;
    XObj tagSessn;

    xTrace0(joinp, TR_EVENTS, "joinClose - entered");

    s->rcnt = s->rcnt -1;
    if (s->rcnt > 0) {
    	xTrace0(joinp, TR_ERRORS, "joinClose - non-zero reference count ");
	return XK_SUCCESS;
    }

    ss = (SState *)s->state;
    tagSessn = xGetDown(s,0);
    ts = (TState *)tagSessn->state;

    ts->state = JOINDYING;
    ts->up[ss->seg] = ERR_XOBJ;

    xTrace0(joinp, TR_ERRORS, "joinClose - done");
    xClose(tagSessn);
    xDestroy(s);
    
    xTrace0(joinp, TR_ERRORS, "joinClose - exiting");

    return XK_SUCCESS;
}


static xkern_return_t
joinTagClose( s )
    XObj	s;
{
    TState *ts = (TState *)s->state;
    PState *ps = (PState *)s->myprotl->state;


   
    ts->state = JOINDYING;
    s->rcnt = s->rcnt -1;
    if (s->rcnt > 0) {
    	xTrace0(joinp, TR_EVENTS, "joinTagClose - non-zero reference count ");
	return XK_SUCCESS;
    }
    xAssert(ts->state == JOINDYING);

    mapRemoveBinding(ps->actMap,s->binding);
    mapRemoveBinding(ps->tagMap,ts->tagBinding);

    xClose(xGetDown(s, 0));
    xDestroy(s);

    xTrace0(joinp, TR_ERRORS, "joinTagClose - complete");
    return XK_SUCCESS;
}

static xmsg_handle_t
joinTagPush( s, msg )
    XObj	s;
    Msg 	*msg;
{
    	xTrace0(joinp, TR_ERRORS, "joinTagPush - called!");
        return XK_FAILURE;
}

static xmsg_handle_t
joinPush( s, msg )
    XObj	s;
    Msg 	*msg;
{
    SState *ss;
    TState *ts;
    XObj   tagSessn;

    xTrace0(joinp, TR_EVENTS, "joinPush - entered");

    ss = (SState *)s->state;
    tagSessn = xGetDown(s,0);
    ts = (TState *)tagSessn->state;

    if (ts->state != JOINREADY) {
        xTrace0(joinp, TR_ERRORS, "joinPush - session not ready");
        return XK_FAILURE;
    }
    if (ts->downSegFull[ss->seg]) {
        xTrace0(joinp, TR_ERRORS, "joinPush - slot already filled!");
        return XK_FAILURE;
    }

    msgConstructCopy(&(ts->downSeg[ss->seg]),msg);
    ts->downSegFull[ss->seg] = 1;

    xTrace0(joinp, TR_EVENTS, "joinPush - exiting");
    return XK_SUCCESS;
}

static xkern_return_t
joinPop( s, lls, m, h)
    XObj	s, lls;
    Msg 	*m;
    void        *h;
{
    xTrace0(joinp, TR_EVENTS, "joinPop - entered");
    return xDemux(s, m);
}

static xkern_return_t
joinTagPop( s, lls, m, h)
    XObj	s, lls;
    Msg 	*m;
    void        *h;
{
    JOINhdr hdr;
    TState	*ts = (TState *) s->state;
    int i;
    xkern_return_t val;

    xTrace0(joinp, TR_EVENTS, "joinTagPop - entered");

    semWait(&ts->wait);

    xTrace0(joinp, TR_EVENTS, "joinTagPop - past semaphore");

    if (msgPop(m, joinHdrLoad1, (char *)&hdr, 8, 0) == FALSE) {
        xTrace0(joinp, TR_SOFT_ERRORS, "joinTagPop - msgPop fails");
        return XK_SUCCESS;
    }

    if (msgPop(m, joinHdrLoad2, (char *)&hdr, 4*hdr.numSeg, 0) == FALSE) {
        xTrace0(joinp, TR_SOFT_ERRORS, "joinTagPop - msgPop fails");
        return XK_SUCCESS;
    }

    for (i=0;i<hdr.numSeg-1; i++) {
       xTrace2(joinp, TR_EVENTS, "joinTagPop - fragmenting i %d len %d",i,hdr.len[i]);
	if (hdr.len[i]) { 
            msgConstructEmpty(&(ts->upSeg[i]));
            msgChopOff(m, &(ts->upSeg[i]),hdr.len[i]);
            ts->upSegFull[i] = 1;
	}
    }
    if (hdr.len[hdr.numSeg-1]) {  
        msgConstructCopy(&ts->upSeg[hdr.numSeg-1],m); 
        ts->upSegFull[hdr.numSeg-1] = 1;
    }

    for (i=0; i<hdr.numSeg; i++) {
        if (ts->upSegFull[ts->order[i]]) {
            val = xPop(ts->up[ts->order[i]],lls,&ts->upSeg[ts->order[i]],0);
            msgDestroy(&(ts->upSeg[ts->order[i]])); 
        }
    }
   
    xControl(ts->controlSessn, JOINDONE, 0, 0);
    semSignal(&ts->wait);
    
    xTrace0(joinp, TR_EVENTS, "joinTagPop - exits");
    return val;
} 

static int
joinControlSessn(s, op, buf, len)
    XObj	s;
    int 	op, len;
    char 	*buf;
{
    xAssert(xIsSession(s));

    xTrace0(joinp, TR_EVENTS, "joinControlSessn - entered");
    return xControl(xGetDown(s,0), op, buf, len);
}

static int
joinTagControlSessn(s, op, buf, len)
    XObj	s;
    int 	op, len;
    char 	*buf;
{
    TState *ts = (TState *) s->state;
    xAssert(xIsSession(s));

    xTrace0(joinp, TR_EVENTS, "joinTagControlSessn - entered");

    switch (op) {

      case GETMAXPACKET:
      case GETOPTPACKET:
	{ 
	    int max;
	    if (len < sizeof(max)) return 0; 
	    if (xControl(xGetDown(s, 0), op, (char *) &max, sizeof(max)) <= 0) {
		return 0; 
	    }
	    
	    *(int *)buf = (max - sizeof(JOINhdr));
	    return(sizeof(max));
	}

      case JOINGETNUMSEG:
        {   
	    if (len != sizeof(ts->numSeg)) return 0; 
	    *(int *)buf = ts->numSeg;
            return(sizeof(ts->numSeg));
        }
      case JOINGETORDER:
        {   
	    if (len != sizeof(ts->order)) return 0; 
	    bcopy((char *) &ts->order, buf, sizeof(ts->order));
            return(sizeof(ts->order));
        }
      case JOINSETCONTROL:
        {   
	    if (len != sizeof(ts->controlSessn)) return 0; 
	    bcopy(buf, (char *) &ts->controlSessn, sizeof(ts->controlSessn));
            return(sizeof(ts->controlSessn));
        }
      case JOINDONE:
        {   
	    JOINhdr hdr;
	    int i;
	    Msg fullmsg;


            xTrace0(joinp, TR_EVENTS, "joinControl(JOINDONE)- entered");
	    hdr.id = JOINID;
            hdr.numSeg = ts->numSeg;
            msgConstructEmpty(&fullmsg);
            for (i=0;i<ts->numSeg; i++) {
   		if (ts->downSegFull[i]) {
                    hdr.len[i] = msgLen(&(ts->downSeg[i]));
                    msgJoin(&fullmsg, &fullmsg, &(ts->downSeg[i]));
                    msgDestroy(&(ts->downSeg[i]));
		    ts->downSegFull[i] = 0;
		} else {
                    hdr.len[i] = 0;
		}
    	    }
            msgPush(&fullmsg,joinHdrStore, &hdr, 8+(4*hdr.numSeg), 0);

            xTrace0(joinp, TR_EVENTS, "joinControl(JOINDONE) - pushing full message");
            xPush(xGetDown(s, 0), &fullmsg);

            msgDestroy(&fullmsg);

            xTrace0(joinp, TR_EVENTS, "joinControl(JOINDONE)- done");

            return 0;
        }

      default:
	return xControl(xGetDown(s,0), op, buf, len);
    }
}


static int
joinControlProtl( self, op, buf, len )
    XObj	self;
    int 	op, len;
    char 	*buf;
{
    PState *ps = (PState *) self->state;
    xAssert(xIsProtocol(self));
  
    xTrace0(joinp, TR_EVENTS, "joinControlProtl - entered");
    switch (op) {
      case GETMAXPACKET:
      case GETOPTPACKET:
        {
            int max;
	    if (len < sizeof(max)) return 0; 
	    if (xControl(xGetDown(self, 0), op, (char *) &max, sizeof(max)) <= 0) {
		return 0; 
	    }
	    *(int *)buf = (max - sizeof(JOINhdr));
            return(sizeof(max));
	}
      case JOINGETNUMSEG:
        {   
	    if (len != sizeof(ps->numSeg)) return 0; 
	    *(int *)buf = ps->numSeg;
            return(sizeof(ps->numSeg));
        }
      case JOINGETORDER:
        {   
	    if (len != sizeof(ps->order)) return 0; 
	    bcopy((char *) &ps->order, buf, sizeof(ps->order));
            return(sizeof(ps->order));
        }

      default: return xControl(xGetDown(self, 0), op, buf, len);
  }
  
}


static void
joinSessnInit(s)
    XObj s;
{
  xTrace0(joinp, TR_EVENTS, "joinSessnInit - entered");
  s->push = joinPush;
  s->pop = joinPop;
  s->close = joinClose;
  s->control = joinControlSessn;
}

static void
joinTagSessnInit(s)
    XObj s;
{
  xTrace0(joinp, TR_EVENTS, "joinTagSessnInit - entered");
  s->push = joinTagPush;
  s->pop = joinTagPop;
  s->close = joinTagClose;
  s->control = joinTagControlSessn;
}

static void
joinHdrStore(hdr, dst, len, arg)
    VOID *hdr;
    char *dst;
    long int len;
    VOID *arg;
{
    int i;
    int numseg;
   
    xTrace0(joinp, TR_EVENTS, "joinHdrStore - entered");
   
    numseg = ((JOINhdr *)hdr)->numSeg;
    xTrace1(joinp, TR_EVENTS, "hdr->id = %d", ((JOINhdr *)hdr)->id);
    ((JOINhdr *)hdr)->id = htonl(((JOINhdr *)hdr)->id);
    xTrace1(joinp, TR_EVENTS, "hdr->numSeg = %d", ((JOINhdr *)hdr)->numSeg);
    ((JOINhdr *)hdr)->numSeg = htonl(((JOINhdr *)hdr)->numSeg);
    for (i=0; i<numseg; i++) {
        xTrace2(joinp, TR_EVENTS, "hdr->len[%d] = %d", i, ((JOINhdr *)hdr)->len[i]);
        ((JOINhdr *)hdr)->len[i] = htonl(((JOINhdr *)hdr)->len[i]);
    }

    bcopy((char *)hdr, dst, len);
    xTrace0(joinp, TR_EVENTS, "joinHdrStore - exiting");
}

static long
joinHdrLoad1(hdr, src, len, arg)
    VOID *hdr;
    char *src;
    long int len;
    VOID *arg;
{
    xTrace0(joinp, TR_EVENTS, "joinHdrLoad1 - entered");

    bcopy(src, (char *)hdr, len);
    ((JOINhdr *)hdr)->id = ntohl(((JOINhdr *)hdr)->id);
    xTrace1(joinp, TR_EVENTS, "hdr->id = %d", ((JOINhdr *)hdr)->id);
    ((JOINhdr *)hdr)->numSeg = ntohl(((JOINhdr *)hdr)->numSeg);
    xTrace1(joinp, TR_EVENTS, "hdr->numSeg = %d", ((JOINhdr *)hdr)->numSeg);

    xTrace0(joinp, TR_EVENTS, "joinHdrLoad - exiting");

    return (len);
}

static long
joinHdrLoad2(hdr, src, len, arg)
    VOID *hdr;
    char *src;
    long int len;
    VOID *arg;
{
    int i;
    xTrace0(joinp, TR_EVENTS, "joinHdrLoad2 - entered");

    bcopy(src, (char *)&((JOINhdr *)hdr)->len, len);
    for (i=0; i<len/4; i++) {
        ((JOINhdr *)hdr)->len[i] = ntohl(((JOINhdr *)hdr)->len[i]);
        xTrace2(joinp, TR_EVENTS, "hdr->len[%d] = %d", i, ((JOINhdr *)hdr)->len[
i]);
    }
    xTrace0(joinp, TR_EVENTS, "joinHdrLoad2 - exiting");

    return (len);
}


static int
partToId(p, buf, maxLen)
    Part *p;
    char *buf;
    int maxLen;
{
    int i,j,len;
    char *ptr;
    int numPart = partLen(p);
    u_char *ptr2;
    
    xTrace0(joinp, TR_EVENTS, "partToLen - entered");
    len = 0;
    ptr = (char *)buf;
    for (j=0;j<numPart; j++) {
    	for (i=p[j].stack.top; i>0; i--)  {
       		len += p[j].stack.arr[i-1].len;
		if (len > maxLen) return 0;
        	bcopy((char *)p[j].stack.arr[i-1].ptr,(char *)ptr, p[j].stack.arr[i-1].len);
        	ptr += p[j].stack.arr[i-1].len;
    	}
    }

    xTrace1(joinp, TR_EVENTS, "partToLen - exiting len %d",len);
    xIfTrace(joinp, TR_EVENTS) {
        printf("part2ToLen - id= (");
        ptr2 = (u_char *)buf;
        for (i=0;i<len;i++) printf("%x",ptr2[i]);
        printf(")\n");
    }

    return(len);
}

static int
part2ToId(p, buf, maxLen)
    Part *p;
    char *buf;
    int maxLen;
{
    int i,len;
    char *ptr;
    u_char *ptr2;
    
    xTrace0(joinp, TR_EVENTS, "part2ToLen - entered");
    len = 0;
    ptr = (char *)buf;
    for (i=p[0].stack.top; i>0; i--)  {
       	len += p[1].stack.arr[i-1].len;
	if (len > maxLen) return 0;
       	bcopy((char *)p[1].stack.arr[i-1].ptr,(char *)ptr, p[1].stack.arr[i-1].len);
       	ptr += p[1].stack.arr[i-1].len;
    }
    xTrace1(joinp, TR_EVENTS, "part2ToLen - exiting len %d",len);
    xIfTrace(joinp, TR_EVENTS) {
        printf("part2ToLen - id= (");
        ptr2 = (u_char *)buf;
        for (i=0;i<len;i++) printf("%x",ptr2[i]);
        printf(")\n");
    }

    return(len);
}

static int
isOrderValid(order,numseg)
int *order;
int numseg;
{
    int i,j;
    int found;
    
    for (i=0; i<numseg;i++) {
       found=0;
       for (j=0;j<numseg;j++) {
           if (order[j] == i) found = 1; 
       }
       if (!found) return 0;
    }
    return 1;
}


/*
 * Sets the 'port' field of the protocol state
 */
static void
processRomFile( self )
    XObj        self;
{
    PState      *ps = (PState *)self->state;
    int         i,k;

    for ( i=0; rom[i][0]; i++ ) {
        if ( ! strncmp(rom[i][0], self->name, strlen(self->name)) ) {
            if ( ! strcmp(rom[i][0], self->fullName) ) {
                if ( rom[i][1] ) {
                    if ( ! strcmp(rom[i][1], "segments") && 
			   rom[i][2] &&
                           (sscanf(rom[i][2], "%d", &ps->numSeg) >= 1)
		        ) {
                        continue;
                    }
                    if ( ! strcmp(rom[i][1], "order")) {
 			for (k=2;(k<ps->numSeg+2) && rom[i][k];k++) {
                            if (sscanf(rom[i][k], "%d", &ps->order[k-2]) <1 ) {
            			sprintf(errBuf, "%s format error in line %d of the rom file",
                    			self->fullName, i + 1);
            			xError(errBuf);
            			break;
			    }
			}
                        continue;
                    }
                }
            } else {
                continue;
            }
            sprintf(errBuf, "%s format error in line %d of the rom file",
                    self->fullName, i + 1);
            xError(errBuf);
            break;
        }
    }
}


