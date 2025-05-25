/*     
 * $RCSfile: uproxy.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.32 $
 * $Date: 1993/11/10 17:42:53 $
 */


/* 
 * Uproxy provides proxy protocols for protocols which live in other
 * protection domains and are configured above protocols in this
 * domain.  It effectively sits at the top of the protocol graph in
 * the domain where it is configured.
 *
 * Uproxy should only be instantiated once.  The actual instantiated
 * protocol object doesn't take part in many UPI operations, but it
 * creates other proxy protocol objects as necessary.
 *
 * Uproxy does not create its own sessions.  Lower sessions for which
 * proxy objects exist in other protection domains are kept in Uproxy
 * maps with reference counts representing the number of such foreign
 * proxy objects.  Uproxy and Lproxy cooperate to maintain these
 * counts. 
 */


#ifdef XKMACHKERNEL
#  include <vm/vm_kern.h>
#else
#  include <mach.h>
#  include <servers/netname.h>
#endif XKMACHKERNEL

#include "xkernel.h"
#include "xk_mach.h"
#include "xk_mig_t.h"
#include "uproxy.h"
#include "proxy_util.h"
#include "xk_lproxy.h"
#include "msg_internal.h"
#include "proxy_util.h"


int	traceuproxyp;


extern	boolean_t	xk_uproxy_server();

#ifdef __STDC__

void	uproxy_init( XObj );

static void		bindSession( XObj, XObj );
static XObj		findProxyXObj( PORT_TYPE, xobj_ext_id_t );
static VOID * 		startReader( VOID * );
static xkern_return_t	uproxyCallDemux( XObj, XObj, Msg *, Msg * );
static xkern_return_t	uproxyDemux( XObj, XObj, Msg * );
static xkern_return_t	uproxyOpenDone( XObj, XObj, XObj, XObj );
static void		watchLproxyPorts( Event, VOID * );


#else

static void		bindSession();
static XObj		findProxyXObj();
static VOID * 		startReader();
static xkern_return_t	uproxyCallDemux();
static xkern_return_t	uproxyDemux();
static xkern_return_t	uproxyOpenDone();
static void		watchLproxyPorts();


#endif


/* 
 * This is the state structure for each of the proxy protocol objects
 * representing foreign protocols.  The instantiated Uproxy protocol
 * maintains its state in static variables and doesn't have one of
 * these structures attached.
 */
typedef struct {
    PORT_TYPE		sendPort;
    xobj_ext_id_t	peerId;
    /* 
     * mySessns is a bag of lower sessions 
     * for which this protocol is the hlpRcv.  Used to determine which
     * sessions to close when this proxy protocol is shut down.
     */
    Map			mySessns;
    /* 
     * llpMap is a bag of all llp's which this protl has openenabled. 
     */
    Map			llpMap;
} PState;

#define SESSN_MAP_SZ	11
#define LLP_MAP_SZ	1


typedef struct {
    PORT_TYPE		port;
    xobj_ext_id_t	xobjRef;
} ProxyMapKey;



/* 
 * map of external object ID's to local proxy objects
 */
static	Map		proxyMap;
#define PROXY_MAP_SZ	113

/* 
 * Map of local proxy objects.  This is used to determine whether 
 * a local protocol object is one of our proxy protocols.
 */
static	Map		revProxyMap;
#define PROXY_MAP_SZ	113

/* 
 * A collection of send rights to protection domains which have
 * references to local objects.  If one of these send rights goes bad,
 * we need to take action.
 */
static	Map		sendRightMap;
#define SEND_RIGHT_MAP_SZ	5


#define WATCH_PORTS_INTERVAL	(30 * 1000 * 1000)
#define UPROXY_NUM_INPUT_THREADS	5

static	mach_port_t 	rcvPort;
static	bool		instantiated = FALSE;


#ifndef XKMACHKERNEL

static VOID *
startReader( readPort )
    VOID	*readPort;
{
    mach_msg_return_t	mr;
    LingeringMsg	*lMsg;

    lMsg = X_NEW(LingeringMsg);
    lMsg->valid = 0;
    cthread_set_data(cthread_self(), lMsg);
    /* 
     * This call should read request messages forever and never
     * return. 
     */
    mr = mach_msg_server(xk_uproxy_server, XK_MAX_MIG_MSG_LEN, *(mach_port_t *)readPort);
    xError("mach_msg_server(uproxy) returns!");
    if ( mr != MACH_MSG_SUCCESS ) {
	sprintf(errBuf, "mach_msg_server error: %s\n", mach_error_string(mr));
	Kabort(errBuf);
    }
    return 0;
}

#endif ! XKMACHKERNEL


#if defined(XKMACHKERNEL) && defined(XK_DEBUG)

static void
vmStatsEvent( ev, arg )
    Event	*ev;
    VOID	*arg;
{
    xIfTrace(uproxyp, TR_MAJOR_EVENTS) xkDumpVmStats();
    evDetach(evSchedule(vmStatsEvent, arg, (u_int)arg));
}
#endif

void
uproxy_init( self )
    XObj	self;
{
    kern_return_t	kr;
    int			i;

    if ( instantiated ) {
	xError("uproxy can not be instantiated more than once!");
	return;
    } else {
	instantiated = TRUE;
    }
    xTraceP0(self, TR_GROSS_EVENTS, "init called");
#if defined(XKMACHKERNEL) && defined(XK_DEBUG)
    evDetach(evSchedule(vmStatsEvent, (VOID *)(30 * 1000 * 1000), 30 * 1000 * 1000));
#endif

#ifndef XKMACHKERNEL
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			    &rcvPort);
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(self, TR_ERRORS, "mach_port_allocate error: %s\n", mach_error_string(kr));
	exit(1);
    }
    xkIncreaseCthreadLimit(UPROXY_NUM_INPUT_THREADS);
    for ( i=0; i < UPROXY_NUM_INPUT_THREADS; i++ ) {
	cthread_fork( startReader, (VOID *)&rcvPort );
    }

    kr = netname_check_in(name_server_port, self->instName, mach_task_self(), rcvPort);
    if ( kr == KERN_SUCCESS ) {
	xTraceP0(self, TR_EVENTS, "upProxyReg succeeds");
    } else {
	sprintf(errBuf, "upProxyReg fails (code %s)!",
		 mach_error_string(kr));
	xError(errBuf);
	return;
    }
#endif /* ! XKMACHKERNEL */
    evDetach(evSchedule(watchLproxyPorts, 0, WATCH_PORTS_INTERVAL));
    proxyMap = mapCreate( PROXY_MAP_SZ, sizeof(ProxyMapKey) );
    revProxyMap = mapCreate( PROXY_MAP_SZ, sizeof(XObj) );
    sendRightMap = mapCreate( SEND_RIGHT_MAP_SZ, sizeof(PORT_TYPE) );
}



/* 
 * findProxyXObj -- 
 *
 * Find the local XObject which is the proxy
 * for the indicated external XObj reference.
 */
static XObj
findProxyXObj( lproxyPort, extXObj )
    PORT_TYPE		lproxyPort;
    xobj_ext_id_t	extXObj;
{
    ProxyMapKey	key;
    XObj	localObj;

    xTrace2(uproxyp, TR_DETAILED,
	    "uproxy findProxyXObj, port == %x, extXObj == %x",
	    (int)lproxyPort, (int)extXObj);
    bzero((char *)&key, sizeof(key));
    key.port = lproxyPort;
    key.xobjRef = extXObj;
    xAssert(proxyMap);
    if ( mapResolve(proxyMap, &key, &localObj) == XK_SUCCESS ) {
	xTrace1(uproxyp, TR_MORE_EVENTS, "findProxyXObj found existing object %x",
		localObj);
	return localObj;
    }
    return ERR_XOBJ;
}      



/* 
 * Invoked when this protocol's peer's protection domain goes away. 
 */
static xkern_return_t
uproxyProtlClose( p )
    XObj	p;
{
    PState *ps = (PState *)p->state;

    mapClose(ps->llpMap);
    mapClose(ps->mySessns);
    xDestroy(p);
    return XK_SUCCESS;
}


static int
doClose( key, val, arg )
    VOID	*key, *val, *arg;
{
    XObj	lls = *(XObj *)key;

    xTrace1(uproxyp, TR_MORE_EVENTS, "closing lls %x", (int)lls);
    xAssert(xIsValidXObj(lls));
    xClose(lls);
    return TRUE;
}


static int
doDisableAll( key, val, arg )
    VOID	*key, *val, *arg;
{
    XObj	llp = (XObj)val;
    XObj	self = (XObj)arg;

    xAssert(xIsProtocol(self));
    xAssert(xIsProtocol(llp));
    xTraceP1(self, TR_EVENTS, "doing disableAll on llp %s", llp->fullName);
    xOpenDisableAll(self, llp);
    return TRUE;
}


static xkern_return_t
noop()
{
    xTrace0(uproxyp, TR_SOFT_ERRORS, "uproxy noop function invoked");
    return XK_FAILURE;
}


/* 
 * This is called externally, not through normal upi operations. 
 */
static void
closeProtl( p )
    XObj	p;
{
    PState 		*ps = (PState *)p->state;
    xkern_return_t	xkr;

    xTraceP0(p, TR_MAJOR_EVENTS, "closeProtl");
    mapForEach(ps->llpMap, doDisableAll, p);
    xTraceP1(p, TR_EVENTS, "closing sessions in sessn map %x",
	     ps->mySessns);
    mapForEach(ps->mySessns, doClose, 0);
    if ( p->rcnt > 1 ) {
	xTraceP1(p, TR_ERRORS, "rcnt (%d) > 1", p->rcnt);
    }
    xkr = mapRemoveBinding(proxyMap, p->binding);
    xAssert( xkr == XK_SUCCESS );
    xkr = mapUnbind(revProxyMap, &p);
    xAssert( xkr == XK_SUCCESS );
    /* 
     * This protocol object won't really go away until all sessions
     * with references to it go away.  We'll disable its functions in
     * case this doesn't happen right away.
     */
    p->calldemux = (Pfk)noop;
    p->demux = (Pfk)noop;
    p->opendone = (Pfk)noop;
    xClose(p);
}


static void
protlInit( p )
    XObj	p;
{
    PState	*ps;

    p->state = xMalloc(sizeof(PState));
    ps = (PState *)p->state;
    ps->mySessns = mapCreate(SESSN_MAP_SZ, sizeof(XObj));
    ps->llpMap = mapCreate(LLP_MAP_SZ, sizeof(XObj));
    p->calldemux = uproxyCallDemux;
    p->demux = uproxyDemux;
    p->close = uproxyProtlClose;
    p->opendone = uproxyOpenDone;
}
    

static void
addProxyXObj( obj, port, extXObj )
    XObj		obj;
    PORT_TYPE		port;
    xobj_ext_id_t	extXObj;
{
    ProxyMapKey	key;
    Bind	b;

    bzero((char *)&key, sizeof(key));
    key.port = port;
    key.xobjRef = extXObj;
    xAssert(proxyMap);
    obj->binding = mapBind(proxyMap, &key, obj);
    xAssert( obj->binding != ERR_BIND );
    xAssert(revProxyMap);
    b = mapBind(revProxyMap, &obj, obj);
    xAssert( b != ERR_BIND );
    xTrace1(uproxyp, TR_EVENTS, "bound obj %x in revProxyMap",
	    (int)obj);
}


static XObj
findProxyProtl( lproxyPort, extXObj )
    PORT_TYPE		lproxyPort;
    xobj_ext_id_t	extXObj;
{
    XObj		localObj;
    char		instName[100];
    xk_xobj_dump_t	objDump;
    kern_return_t	kr;
    xkern_return_t	xkr;
    PState		*ps;

    xTrace2(uproxyp, TR_DETAILED,
	    "uproxy findProxyProtl, port == %x, extXObj == %x",
	    (int)lproxyPort, (int)extXObj);
    if ( (localObj = findProxyXObj(lproxyPort, extXObj)) != ERR_XOBJ ) {
	return localObj;
    }
    /* 
     * Create a new proxy protocol
     */
    xTrace0(uproxyp, TR_GROSS_EVENTS, "Creating new proxy protocol object");
    kr = call_lproxy_dumpXObj(lproxyPort, &xkr, extXObj, &objDump);
    if ( kr != KERN_SUCCESS ) {
	xTrace1(uproxyp, TR_ERRORS, "mach_msg error %d for call_dumpXObj", kr);
	return ERR_XOBJ;
    }
    if ( xkr == XK_FAILURE ) {
	xTrace1(uproxyp, TR_ERRORS, "call_dumpXObj(%x) fails", extXObj);
	return ERR_XOBJ;
    }
    sprintf(instName, "%s%s%s", objDump.instName,
	    (*objDump.instName == 0) ? "" : "_", "proxy");
    xTrace2(uproxyp, TR_GROSS_EVENTS, "new proxy protocol name is %s/%s",
	    objDump.name, instName);
    localObj = xCreateProtl(protlInit, objDump.name, instName, &traceuproxyp, 0, 0);
    if ( localObj == ERR_XOBJ ) {
	return ERR_XOBJ;
    }
    xTrace2(uproxyp, TR_EVENTS, "binding proxy (%x) to ext protl (%x)",
	    (int)localObj, (int)extXObj);
    addProxyXObj(localObj, lproxyPort, extXObj);
    ps = (PState *)localObj->state;
    ps->sendPort = lproxyPort;
    ps->peerId = extXObj;
    mapBind(sendRightMap, &lproxyPort, 0);
    return localObj;
}


/* 
 * uproxy_xDuplicate is called to explicitly indicate that an
 * additional foreign-domain proxy object has been created for the
 * indicated session.  We have to be told about the hlp because it's
 * possible for us to remove this session from our maps and then have
 * the foreign Lproxy decide to duplicate the proxy session.
 *
 * lls is a session in this domain.  hlp is an upper protocol in the
 * foreign domain.  
 */
kern_return_t
do_uproxy_xDuplicate( reqPort, lproxyPort, retVal, extHlp, extLls )
    PORT_TYPE		reqPort;
    PORT_TYPE		lproxyPort;
    xkern_return_t	*retVal;
    xobj_ext_id_t	extHlp;
    xobj_ext_id_t	extLls;
{
    XObj	lls = (XObj)extLls;
    XObj	hlp;

    xTrace1(uproxyp, TR_EVENTS, "do_xDuplicate (%x) called", (int)lls);
    xk_master_lock();
    hlp = findProxyXObj(lproxyPort, extHlp);
    if ( xIsValidXObj(lls) && xIsValidXObj(hlp) ) {
	bindSession(lls, hlp);
	*retVal = XK_SUCCESS;
    } else {
	if ( ! xIsValidXObj(lls) ) {
	    xTrace1(uproxyp, TR_SOFT_ERRORS,
		    "uproxy_xDuplicate: lls %x is invalid", (int)lls);
	}
	if ( ! xIsValidXObj(hlp) ) {
	    xTrace2(uproxyp, TR_SOFT_ERRORS,
		    "uproxy_xDuplicate: hlp %x (proxy of %x) is invalid",
		    (int)hlp, (int)extHlp);
	}
	*retVal = XK_FAILURE;
    }
    xk_master_unlock();
    return KERN_SUCCESS;
}



#define RETURN_FAILURE	{ xk_master_unlock(); *retVal = XK_FAILURE; return KERN_SUCCESS; }


kern_return_t
do_uproxy_xOpenEnable( reqPort, lproxyPort, retVal, extHlpRcv, extHlpType, llp, extPart )
    PORT_TYPE		reqPort;
    PORT_TYPE		lproxyPort;
    xkern_return_t	*retVal;
    xobj_ext_id_t	extHlpRcv;
    xobj_ext_id_t	extHlpType;
    xobj_ext_id_t	llp;
    xk_part_t		extPart;
{
    Part	*p, emptyPart;
    int		numParts;
    XObj	hlpRcv, hlpType;
    PState	*ps;

    xTrace0(uproxyp, TR_EVENTS, "do_openEnable called");

    xk_master_lock();

    xTrace1(uproxyp, TR_EVENTS, "do_openEnable, reply port %d", (int)lproxyPort);
    if ( (hlpRcv = findProxyProtl(lproxyPort, extHlpRcv)) == ERR_XOBJ ) {
	RETURN_FAILURE;
    }
    ps = (PState *)hlpRcv->state;
    if ( (hlpType = findProxyProtl(lproxyPort, extHlpType)) == ERR_XOBJ ) {
	RETURN_FAILURE;
    }
    if ( ! xIsValidXObj((XObj)llp) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_xOpenEnable -- llp object %x is unsafe", llp);
	RETURN_FAILURE;
    }
    mapBind(ps->llpMap, &llp, llp);

    numParts = *(int *)extPart;
    if ( numParts != 0 ) {
        p = (Part *)xMalloc(numParts * sizeof(Part));
	partInternalize(p, extPart);
	*retVal = xOpenEnable(hlpRcv, hlpType, (XObj)llp, p);
        xFree((char *)p);
    }
    else {
        partInit(&emptyPart, 0);
	*retVal = xOpenEnable(hlpRcv, hlpType, (XObj)llp, &emptyPart);
    }

    xk_master_unlock();

    xTrace0(uproxyp, TR_FUNCTIONAL_TRACE, "do_xOpenEnable returns");
    return KERN_SUCCESS;
}


kern_return_t
do_uproxy_xOpenDisable( reqPort, lproxyPort, retVal, extHlpRcv, extHlpType, llp, extPart )
    PORT_TYPE		reqPort;
    PORT_TYPE		lproxyPort;
    xkern_return_t	*retVal;
    xobj_ext_id_t	extHlpRcv;
    xobj_ext_id_t	extHlpType;
    xobj_ext_id_t	llp;
    xk_part_t		extPart;
{
    Part	*p;
    int		numParts;
    XObj	hlpRcv, hlpType;

    xTrace0(uproxyp, TR_EVENTS, "do_openDisable called");

    xk_master_lock();

    if ( (hlpRcv = findProxyProtl(lproxyPort, extHlpRcv)) == ERR_XOBJ ) {
	RETURN_FAILURE;
    }
    if ( (hlpType = findProxyProtl(lproxyPort, extHlpType)) == ERR_XOBJ ) {
	RETURN_FAILURE;
    }
    if ( ! xIsValidXObj((XObj)llp) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_xOpenDisable -- llp object %x is unsafe", llp);
	RETURN_FAILURE;
    }
    numParts = *(int *)extPart;
    p = (Part *)xMalloc(numParts * sizeof(Part));
    partInternalize(p, extPart);
    *retVal = xOpenDisable(hlpRcv, hlpType, (XObj)llp, p);
    xFree((char *)p);

    xk_master_unlock();

    xTrace0(uproxyp, TR_FUNCTIONAL_TRACE, "do_xOpenDisable returns");
    return KERN_SUCCESS;
}


#undef RETURN_FAILURE
#define RETURN_FAILURE {					\
    			  xk_master_unlock();			\
			  *retObj = (xobj_ext_id_t)ERR_XOBJ;	\
			  return KERN_SUCCESS;			\
		       }

kern_return_t
do_uproxy_xOpen( reqPort, lproxyPort, retObj, extHlpRcv, extHlpType, llp, extPart )
    PORT_TYPE		reqPort;
    PORT_TYPE		lproxyPort;
    xobj_ext_id_t	*retObj;
    xobj_ext_id_t	extHlpRcv;
    xobj_ext_id_t	extHlpType;
    xobj_ext_id_t	llp;
    xk_part_t		extPart;
{
    Part	*p;
    XObj	hlpRcv, hlpType;

    xTrace0(uproxyp, TR_EVENTS, "do_open called");

    xk_master_lock();

    if ( (hlpRcv = findProxyProtl(lproxyPort, extHlpRcv)) == ERR_XOBJ ) {
	RETURN_FAILURE;
    }
    if ( (hlpType = findProxyProtl(lproxyPort, extHlpType)) == ERR_XOBJ ) {
	RETURN_FAILURE;
    }
    if ( ! xIsValidXObj((XObj)llp) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_xOpen -- llp object %x is unsafe", llp);
	RETURN_FAILURE;
    }
    p = (Part *)xMalloc(partExtLen(extPart) * sizeof(Part));
    partInternalize(p, extPart);
    *retObj = (xobj_ext_id_t)xOpen(hlpRcv, hlpType, (XObj)llp, p);
    xFree((char *)p);
    if ( xIsXObj((XObj)*retObj) ) {
	xTrace1(uproxyp, TR_FUNCTIONAL_TRACE, "do_xOpen got session with rcnt == %d",
		((XObj)*retObj)->rcnt);
	bindSession((XObj)*retObj, hlpRcv);
	/* 
	 * We secure our tie to this session in 'bindSession', so we
	 * don't need the extra ref count given to us automatically as
	 * a result of the xOpen.
	 */
	xClose((XObj)*retObj);
    }
    xk_master_unlock();

    xTrace2(uproxyp, TR_FUNCTIONAL_TRACE, "do_xOpen returns %x, rcnt == %d",
	    (int)*retObj, ((XObj)*retObj)->rcnt);
    return KERN_SUCCESS;
}


/* 
 * Bind a session into our map so we can shut it down later if
 * the proxy dies.  The object is bound to a reference count representing
 * the number of foreign references not accounted for by input
 * threads.  We only actually increment the reference count of the
 * object in our address space once, indicating that it is in our map
 * and that >= 1 such foreign references exist.
 */
static void
bindSession( s, hlpRcv )
    XObj	s, hlpRcv;
{      
    Bind		b;
    PState		*ps = (PState *)hlpRcv->state;
    VOID		*count;
    xkern_return_t	xkr;
    
    xTraceP2(hlpRcv, TR_DETAILED, "binding session (%x) to map (%x)",
	     s, ps->mySessns);
    xTraceP1(hlpRcv, TR_DETAILED, "refcnt == %d", s->rcnt);
    if ( mapResolve(ps->mySessns, &s, &count) == XK_SUCCESS ) {
	xTraceP0(hlpRcv, TR_MAJOR_EVENTS,
		 "session already bound, incrementing count");
	count = (VOID *)((int)count + 1);
	xkr = mapUnbind(ps->mySessns, &s);
	xAssert( xkr == XK_SUCCESS );
    } else {
	count = (VOID *)1;
	xDuplicate(s);
    }
    b = mapBind(ps->mySessns, &s, (VOID *)count);
    xAssert(b != ERR_BIND);
    xTraceP2(hlpRcv, TR_DETAILED, "session (%x) bound with count %d",
	     (int)s, (int)count);
}


/* 
 * Decrements the bind count of lls in the protocol map.  If the count
 * is zero, the lower session is removed from the map and closed.
 */
static xkern_return_t
unbindSessn( lls )
    XObj	lls;
{
    PState		*ps;
    VOID		*count;
    xkern_return_t	xkr;
    
    xAssert(xIsProtocol(lls->up));
    ps = (PState *)lls->up->state;
    if ( mapResolve(ps->mySessns, &lls, &count) == XK_FAILURE ) {
	xTrace1(uproxyp, TR_ERRORS,
		"failure resolving lls %x protl map in uproxy unbindSessn", (int)lls);
	return XK_FAILURE;
    }
    xkr = mapUnbind(ps->mySessns, &lls);
    xAssert(xkr == XK_SUCCESS);
    count = (VOID *)((int)count - 1);
    if ( count == 0 ) {
	xTrace0(uproxyp, TR_EVENTS, "bind count == 0, removing");
	xTrace1(uproxyp, TR_EVENTS, "(lls ref count == %d)", lls->rcnt);
	xkr = xClose(lls);
    } else {
	Bind	b;

	xTrace1(uproxyp, TR_EVENTS, "new bind count == %d", (int)count);
	b = mapBind(ps->mySessns, &lls, count);
	xAssert( b != ERR_BIND );
    }
    return xkr;
}



kern_return_t
do_uproxy_xGetProtlByName( reqPort, name, objId )
    PORT_TYPE		reqPort;
    xk_string_t		name;
    xobj_ext_id_t	*objId;
{
    XObj	obj;

    xTrace1(uproxyp, TR_EVENTS, "do_xGetProtlByName(%s) called", name);

    xk_master_lock();

    obj = xGetProtlByName(name);
    if ( xIsXObj(obj) ) {
	*objId = (xobj_ext_id_t)obj;
    } else {
	*objId = 0;
    }
    xTrace1(uproxyp, TR_DETAILED, "do_xGetProtlByName returns %x",
	    (int)*objId);

    xk_master_unlock();

    return KERN_SUCCESS;
}


static xobj_ext_id_t
peerProtl( p )
    XObj	p;
{
    if ( ! xIsXObj(p) || ! xIsValidXObj(p) || ! xIsProtocol(p) ) {
	return (xobj_ext_id_t)ERR_XOBJ;
    }
    xAssert(revProxyMap);
    if ( mapResolve(revProxyMap, &p, 0) == XK_FAILURE ) {
	return (xobj_ext_id_t)ERR_XOBJ;
    }
    return ((PState *)p->state)->peerId;
}
    


kern_return_t
do_uproxy_dumpXObj( reqPort, xkr, extObj, dump )
    PORT_TYPE		reqPort;
    xkern_return_t	*xkr;
    xobj_ext_id_t	extObj;
    xk_xobj_dump_t	*dump;
{
    xTrace0(uproxyp, TR_DETAILED, "do_uproxy_dumpXObj called");
    xk_master_lock();
    if ( ! xIsValidXObj((XObj)extObj) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_dumpXObj -- xobj %x is unsafe", (int)extObj);
	*xkr = XK_FAILURE;
    } else {
	XObj obj = (XObj)extObj;
	xIfTrace(uproxyp, TR_DETAILED) {
	    xPrintXObj(obj);
	}
	dump->type = obj->type;
	dump->myprotl = (xobj_ext_id_t)obj->myprotl;
	if ( xIsProtocol(obj) ) {
	    dump->hlpRcv = (xobj_ext_id_t)ERR_XOBJ;
	    dump->hlpType = (xobj_ext_id_t)ERR_XOBJ;
	} else {
	    dump->hlpRcv = peerProtl(obj->up);
	    dump->hlpType = peerProtl(xHlpType(obj));
	}
	strcpy(dump->name, obj->name);
	strcpy(dump->instName, obj->instName);
	*xkr = XK_SUCCESS;
    } 
    xk_master_unlock();
    return KERN_SUCCESS;
}


#ifdef XK_PROXY_MSG_HACK

static void
dummyFree( p )
    char	*p;
{
}

#endif



kern_return_t
do_uproxy_xPush( reqPort, handle, lls, msgData, msgDataCnt, msgOol,
		 msgOolCnt, msgIsInline, attr, attrLen )
    PORT_TYPE 		reqPort;
    xmsg_handle_t	*handle;
    xobj_ext_id_t 	lls;
    char	 	*msgData;
    mach_msg_type_number_t msgDataCnt;
    char *			msgOol;
    mach_msg_type_number_t	msgOolCnt;
    boolean_t			msgIsInline;
    xk_msg_attr_t		attr;
    mach_msg_type_number_t	attrLen;
{
    Msg			msg;
    char		*msgBuf;
#ifdef XK_PROXY_MSG_HACK
    struct MNode	*node;
#endif

    xTrace0(uproxyp, TR_EVENTS, "do_uproxy_xPush called");

    xk_master_lock();

    if ( ! xIsValidXObj((XObj)lls) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_dumpXObj -- xobj %x is unsafe", (int)lls);
	*handle = XMSG_ERR_HANDLE;
    } else {
	if ( msgIsInline ) {
	    xTrace1(uproxyp, TR_DETAILED, "xPush constructing msg of %d bytes", msgDataCnt);
#ifdef XK_PROXY_MSG_HACK
	    /* 
	     * Try to avoid copying the data by forming a message with the
	     * stack data, pushing it, and copying it only if someone else
	     * saves a reference to it during the push sequence.
	     */
	    xTrace0(uproxyp, TR_DETAILED, "Doing constructInPlace hack");
	    msgConstructInplace(&msg, msgData, msgDataCnt, dummyFree);
	    node = msg.stack;
#else
	    msgConstructAllocate(&msg, msgDataCnt, &msgBuf);
	    bcopy(msgData, msgBuf, msgDataCnt);
#endif /* XK_PROXY_MSG_HACK */
	} else {
	    xTrace1(uproxyp, TR_DETAILED, "xPush message of %d bytes (OOL)", msgOolCnt);
	    oolToMsg(msgOol, msgOolCnt, &msg);
	}
	if ( attrLen ) {
	    xkern_return_t	xrv;

	    xTraceP1((XObj)lls, TR_EVENTS, "attaching message attribute of len %d", attrLen);
	    xrv = msgSetAttr(&msg, 0, attr, attrLen);
	    xAssert( xrv == XK_SUCCESS );
	}
	*handle = xPush((XObj)lls, &msg);
#ifdef XK_PROXY_MSG_HACK
	if ( msgIsInline ) {
	    if ( node->refCnt > 1 ) {
		xTrace0(uproxyp, TR_DETAILED,
			"Multiple stack refs, backing out of uproxy inPlace hack");
		xAssert(node->type = t_MNodeUsrPage);
		msgBuf = xMalloc(msgDataCnt);
		bcopy(msgData, msgBuf, msgDataCnt);
		node->b.usrpage.data = msgBuf;
		node->b.usrpage.bFree = xFree;
	    } else {
		xTrace0(uproxyp, TR_DETAILED, "constructInPlace hack works");
	    }
	}
#endif /* XK_PROXY_MSG_HACK */
	msgDestroy(&msg);
    }

    xk_master_unlock();

    return KERN_SUCCESS;
}



kern_return_t
do_uproxy_xCall( reqPort, xkr, lls, reqMsgData, reqMsgDataCnt, reqOol, reqOolCnt,
		 reqIsInline, repMsgData, repMsgDataCnt, repOol, repOolCnt,
		 reqAttr, reqAttrLen )
    PORT_TYPE 			reqPort;
    xkern_return_t *		xkr;
    xobj_ext_id_t 		lls;
    char *	 		reqMsgData;
    mach_msg_type_number_t 	reqMsgDataCnt;
    char *			reqOol;
    mach_msg_type_number_t	reqOolCnt;
    boolean_t			reqIsInline;
    char *	 		repMsgData;
    mach_msg_type_number_t *	repMsgDataCnt;
    char **	 		repOol;
    mach_msg_type_number_t *	repOolCnt;
    xk_msg_attr_t		reqAttr;
    mach_msg_type_number_t	reqAttrLen;
{
    Msg			reqMsg;
    Msg			repMsg;
    char *		reqMsgBuf;
#ifdef XK_PROXY_MSG_HACK
    struct MNode	*node;
#endif

    xTrace0(uproxyp, TR_EVENTS, "do_uproxy_xCall called");

    xk_master_lock();
    lingeringMsgClear();
    if ( ! xIsValidXObj((XObj)lls) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_dumpXObj -- xobj %x is unsafe", (int)lls);
	*xkr = XK_FAILURE;
    } else {
	if ( reqIsInline ) {
#ifdef XK_PROXY_MSG_HACK
	    /* 
	     * Try to avoid copying the data by forming a message with the
	     * stack data, calling with it, and copying it only if someone else
	     * saves a reference to it during the call sequence.
	     */
	    {
		
		xTrace0(uproxyp, TR_DETAILED, "Doing constructInPlace hack");
		msgConstructInplace(&reqMsg, reqMsgData, reqMsgDataCnt, dummyFree);
		node = reqMsg.stack;

	    }
#else
	    msgConstructAllocate(&reqMsg, reqMsgDataCnt, &reqMsgBuf);
	    bcopy(reqMsgData, reqMsgBuf, reqMsgDataCnt);
#endif /* XK_PROXY_MSG_HACK */
	} else {
	    xTrace1(uproxyp, TR_DETAILED, "xCall request of %d bytes (OOL)", reqOolCnt);
	    oolToMsg(reqOol, reqOolCnt, &reqMsg);
	}
	if ( reqAttrLen ) {
	    xkern_return_t	xrv;

	    xTraceP1((XObj)lls, TR_EVENTS, "attaching message attribute of len %d", reqAttrLen);
	    xrv = msgSetAttr(&reqMsg, 0, reqAttr, reqAttrLen);
	    xAssert( xrv == XK_SUCCESS );
	}
	msgConstructEmpty(&repMsg);

	*xkr = xCall((XObj)lls, &reqMsg, &repMsg);
	xTrace2(uproxyp, TR_DETAILED, "xCall returns to proxy stub, xkr == %d,  %d bytes",
		*xkr, msgLen(&repMsg));

#ifdef XK_PROXY_MSG_HACK
	/* 
	 * Copy the in-line request data if necessary
	 */
	if ( reqIsInline ) {
	    if ( node->refCnt > 1 ) {
		xTrace0(uproxyp, TR_DETAILED,
			"Multiple stack refs, backing out of uproxy inPlace hack");
		xAssert(node->type = t_MNodeUsrPage);
		reqMsgBuf = xMalloc(reqMsgDataCnt);
		bcopy(reqMsgData, reqMsgBuf, reqMsgDataCnt);
		node->b.usrpage.data = reqMsgBuf;
		node->b.usrpage.bFree = xFree;
	    } else {
		xTrace0(uproxyp, TR_DETAILED, "constructInPlace hack works");
	    }
	}
#endif /* XK_PROXY_MSG_HACK */

	msgDestroy(&reqMsg);
	if ( msgLen(&repMsg) <= XK_INLINE_THRESHOLD ) {
	    xAssert( msgLen(&repMsg) <= *repMsgDataCnt );
	    xTrace0(uproxyp, TR_EVENTS, "passing reply in-line");
	    msg2buf(&repMsg, repMsgData);
	    *repMsgDataCnt = msgLen(&repMsg);
	    *repOolCnt = 0;
	} else {
	    DeallocFunc	func;
	    VOID	*arg;

	    xTrace0(uproxyp, TR_EVENTS, "passing reply out-of-line");
	    *repMsgDataCnt = 0;
	    if ( msgToOol(&repMsg, repOol, &func, &arg) == XK_FAILURE ) {
		*repOolCnt = 0;
		return XK_FAILURE;
	    } else {
		*repOolCnt = msgLen(&repMsg);
		lingeringMsgSave(func, arg, *repOolCnt);
	    }
	}
	msgDestroy(&repMsg);
    }
    xk_master_unlock();
    return KERN_SUCCESS;
}


kern_return_t
do_uproxy_xControl( reqPort, extObj, op, buf, len )
    PORT_TYPE 		reqPort;
    xobj_ext_id_t 	extObj;
    int			op;
    char		*buf;
    int			*len;
{
    XObj	obj = (XObj)extObj;

    xTrace1(uproxyp, TR_ERRORS, "do_uproxy_xControl called, in length == %d", *len);

    xk_master_lock();

    if ( ! xIsValidXObj(obj) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_xControl -- xobj %x is unsafe", (int)obj);
	*len = -1;
    } else {
	*len = xControl(obj, op, buf, *len);
    }
    xTrace1(uproxyp, TR_ERRORS, "do_xControl returns buffer of length %d", *len);

    xk_master_unlock();

    return KERN_SUCCESS;
}
    


kern_return_t
do_uproxy_xClose( reqPort, xkr, extLls )
    PORT_TYPE 		reqPort;
    xkern_return_t	*xkr;
    xobj_ext_id_t 	extLls;
{
    XObj	lls = (XObj)extLls;

    xTrace0(uproxyp, TR_DETAILED, "do_uproxy_xClose called");

    xk_master_lock();

    if ( ! xIsValidXObj(lls) ) {
	xTrace1(uproxyp, TR_ERRORS, "do_xClose -- xobj %x is unsafe", (int)lls);
	*xkr = XK_FAILURE;
    } else {
	*xkr = unbindSessn(lls);
    }

    xk_master_unlock();

    return KERN_SUCCESS;
}



/* 
 * Normal UPI operations (called by objects from within my own
 * protection domain)
 */


#define checkKernRes( _kr, _s, _str, _res )			\
    if ( (_kr) != KERN_SUCCESS ) {				\
	xTraceP2((_s), TR_ERRORS, "mach msg error %s in %s",	\
		 mach_error_string(_kr), (_str));		\
	return (_res);						\
    }								\



static xkern_return_t
uproxyOpenDone( self, lls, llp, hlpType )
    XObj	self, lls, llp, hlpType;
{
    xkern_return_t	xkr;
    kern_return_t	kr;
    PState		*ps = (PState *)self->state;

    xTraceP1(self, TR_MAJOR_EVENTS, "openDone called, lls %x", (int)lls);
    bindSession(lls, self);
    xk_master_unlock();
    kr = call_lproxy_xOpenDone(ps->sendPort, &xkr, (xobj_ext_id_t)lls,
			       (xobj_ext_id_t)llp, ((PState *)self->state)->peerId,
			       ((PState *)hlpType->state)->peerId);
    xk_master_lock();
    checkKernRes(kr, self, "opendone", XK_FAILURE);
    return xkr;
}



static xkern_return_t
uproxyDemux( self, lls, msg )
    XObj	self, lls;
    Msg		*msg;
{
    PState		*ps = (PState *)self->state;
    xkern_return_t	xkr;
    kern_return_t	kr;
    xk_and_mach_msg_t	xmsg;
    char		*oolBuf;
    int			inlineLen, oolLen;
    DeallocFunc		oolDeallocFunc = 0;
    VOID		*arg;
    int			deallocMachMsg = 1;

    xTraceP0(self, TR_EVENTS, "demux called");
    xTraceP3(self, TR_FULL_TRACE, "calling lproxyDemux, lls == %x, msg == %x, len == %d",
	     (int)lls, (int)msg, msgLen(msg));
    /* 
     * Note: The MIG stub expects us to have a MIG-sized buffer
     * allocated in xmsg.machMsg 
     */
    xmsg.machMsg = 0;
    if ( msgLen(msg) > XK_MAX_MSG_INLINE_LEN ) {
	xTraceP0(self, TR_EVENTS, "sending message out-of-line"); 
	xmsg.xkMsg = 0;
	xmsg.machMsg = xMalloc(XK_MAX_MIG_MSG_LEN);
	if ( msgToOol(msg, &oolBuf, &oolDeallocFunc, &arg) == XK_FAILURE ) {
	    return XK_FAILURE;
	}
	xAssert(oolBuf);
	oolLen = msgLen(msg);
	inlineLen = 0;
    } else {
	if ( msgIsOkToMangle(msg, (char **)&xmsg.machMsg, XK_DEMUX_REQ_OFFSET) ) {
	    deallocMachMsg = 0;
	    xmsg.xkMsg = 0;
	} else {
	    xmsg.xkMsg = msg;
	    /* 
	     * We have to dynamically allocate the message because the
	     * in-kernel stack size is quite small and putting the MIG message
	     * on the stack causes overflow problems.
	     */
	    xmsg.machMsg = xMalloc(XK_MAX_MIG_MSG_LEN);
	}
	oolBuf = 0;
	oolLen = 0;
	inlineLen = msgLen(msg);
    }
    xAssert(&xmsg.machMsg);
    xk_master_unlock();
    kr = call_lproxy_xDemux(ps->sendPort, &xkr, (xobj_ext_id_t)lls, (char *)&xmsg, inlineLen, 0, oolBuf, oolLen, msgGetAttr(msg, 0), msg->attrLen);
    xk_master_lock();
    if ( deallocMachMsg ) {
	xFree(xmsg.machMsg);
    }
    if ( oolDeallocFunc ) {
	oolDeallocFunc(arg, oolLen);
    }
    checkKernRes(kr, self, "demux", XK_FAILURE);
    xTraceP1(self, TR_MORE_EVENTS, "demux returns %x", xkr);

    return xkr;
}



static xkern_return_t
uproxyCallDemux( self, lls, reqMsg, repMsg )
    XObj	self, lls;
    Msg		*reqMsg, *repMsg;
{
    PState		*ps = (PState *)self->state;
    xkern_return_t	xkr = XK_FAILURE;
    kern_return_t	kr;
    xk_and_mach_msg_t	req;
    Msg			tmpMsg;
    int			repMsgLen, reqLen;
    char		*buf;
    char		*reqOol = 0;      /* the actual memory object to send */
    					  /* (same as reqOolBuf if out-of-kernel) */
    DeallocFunc		oolDeallocFunc;
    VOID		*arg;
    char		*repOol;
    int			repOolLen;
    
    xTraceP0(self, TR_EVENTS, "demux called");
    xTraceP3(self, TR_FULL_TRACE, "calling lproxyDemux, lls == %x, msg == %x, len == %d",
	     (int)lls, (int)reqMsg, msgLen(reqMsg));
    /* 
     * If req.xkMsg is non-zero, the stub will copy the data from the
     * x-kernel message into the mach message.  If req.xkMsg is null,
     * the copy doesn't happen (because we have built the mach message
     * around the existing x-kernel message)
     */
    repMsgLen = XK_MAX_MSG_INLINE_LEN;
    reqLen = msgLen(reqMsg);
    if ( reqLen > XK_INLINE_THRESHOLD ) {
	xTraceP0(self, TR_EVENTS, "sending request out-of-line"); 
	req.xkMsg = 0;
	if ( msgToOol(reqMsg, &reqOol, &oolDeallocFunc, &arg) == XK_FAILURE ) {
	    return XK_FAILURE;
	}
	xAssert(reqOol);
    } else {
	xTraceP0(self, TR_EVENTS, "sending request in-line"); 
	req.xkMsg = reqMsg;
	xAssert(reqOol == 0);
    }
    /* 
     * We have to dynamically allocate the message because the
     * in-kernel stack size is quite small and putting the MIG message
     * on the stack causes overflow problems.
     */
    msgConstructAllocate(&tmpMsg, XK_MAX_MIG_MSG_LEN, &buf);
    msgPopDiscard(&tmpMsg, XK_CALL_REP_OFFSET);
    xAssert(buf);
    req.machMsg = buf;

    xk_master_unlock();
    kr = call_lproxy_xCallDemux(ps->sendPort, &xkr, (xobj_ext_id_t)lls,
				(char *)&req, reqOol ? 0 : reqLen, 0,
				reqOol, reqOol ? reqLen : 0,
				(char *)&tmpMsg, &repMsgLen, &repOol, &repOolLen,
				msgGetAttr(reqMsg, 0), reqMsg->attrLen);
    xk_master_lock();
    if ( reqLen > XK_INLINE_THRESHOLD ) {
	if ( oolDeallocFunc ) {
	    oolDeallocFunc(arg, reqLen);
	}
    }
    checkKernRes(kr, self, "calldemux", XK_FAILURE);
    xTraceP1(self, TR_MORE_EVENTS, "calldemux returns %d", xkr);
    if ( xkr == XK_SUCCESS ) {
	if ( repOolLen > 0 ) {
	    xTraceP1(self, TR_MORE_EVENTS, "out-of-line callDemux reply, len %d",
		     repOolLen);
	    msgDestroy(repMsg);
	    oolToMsg(repOol, repOolLen, repMsg);
	} else {
	    xTraceP1(self, TR_MORE_EVENTS, "inline reply msg len == %d", repMsgLen);
	    xAssert(msgLen(&tmpMsg) >= repMsgLen);
	    msgTruncate(&tmpMsg, repMsgLen);
	    msgAssign(repMsg, &tmpMsg);
	}
    }
    msgDestroy(&tmpMsg);
    return xkr;
}




#ifdef XKMACHKERNEL


static int
portActive( port )
    PORT_TYPE	port;
{
    return ip_active(port);
}

#else /* ! XKMACHKERNEL */

/* 
 * We should really do port-death notification ...  But this does let us
 * simulate the same sort of thing that the kernel does.  Though that
 * can probably be improved, too.
 */
static int
portActive( port )
    PORT_TYPE	port;
{
    kern_return_t	kr;

    kr = call_lproxy_ping(port);
    return kr == XK_SUCCESS;
}

#endif /* XKMACHKERNEL */


static int
checkRights( key, val, arg )
    VOID	*key, *val, *arg;
{
    PORT_TYPE		port = *(PORT_TYPE *)key;

    xTrace1(uproxyp, TR_DETAILED, "checking validity of port %x", port);
    if ( portActive(port) ) {
	xTrace0(uproxyp, TR_FULL_TRACE, "port is active");
    } else {
	xTrace0(uproxyp, TR_EVENTS, "port is inactive");
	do_uproxy_abort(0, port);
    }
    return MFE_CONTINUE;
}



static void
watchLproxyPorts( ev, arg )
    Event	ev;
    VOID	*arg;
{
    xTrace0(uproxyp, TR_EVENTS, "watchLproxyPorts runs");
    mapForEach(sendRightMap, checkRights, 0);
    evDetach(evSchedule(watchLproxyPorts, 0, WATCH_PORTS_INTERVAL));
}



static int
closeProxyProtls( mapKey, val, arg )
    VOID	*mapKey, *val, *arg;
{
    PORT_TYPE	dyingPort = *(PORT_TYPE *)arg;
    ProxyMapKey	*key = (ProxyMapKey *)mapKey;

    if ( dyingPort == key->port ) {
	xTrace1(uproxyp, TR_EVENTS, "closing proxy protl %x", (int)val);
	xAssert(xIsXObj((XObj)val));
	closeProtl((XObj)val);
    } else {
	xTrace2(uproxyp, TR_DETAILED,
		"[%s] port mismatch with proxy protl %x, not closing",
		((XObj)val)->fullName, (int)val);
	xTrace2(uproxyp, TR_DETAILED,
		"(dying port == %x, saved port == %x)", dyingPort, key->port);
    }
    return TRUE;
}



kern_return_t
do_uproxy_abort( reqPort, lproxyPort )
    PORT_TYPE	reqPort;
    PORT_TYPE	lproxyPort;
{
    xkern_return_t	xkr;
    
    xTrace0(uproxyp, TR_MAJOR_EVENTS, "uproxyAbort called");
    if ( mapResolve(sendRightMap, &lproxyPort, 0) == XK_FAILURE ) {
	xTrace1(uproxyp, TR_MAJOR_EVENTS,
		"uproxyAbort already called with port %x",
		(int)lproxyPort);
    } else {
	mapForEach(proxyMap, closeProxyProtls, &lproxyPort);
	xkr = mapUnbind(sendRightMap, &lproxyPort);
	xAssert( xkr == XK_SUCCESS );
    }
    return KERN_SUCCESS;
}



kern_return_t
do_uproxy_ping( reqPort )
    PORT_TYPE		reqPort;
{
    xTrace0(uproxyp, TR_EVENTS, "uproxy Ping");
    return KERN_SUCCESS;
}



kern_return_t
do_uproxy_pingtest( reqPort, lproxyPort, n )
    PORT_TYPE		reqPort;
    PORT_TYPE		lproxyPort;
    int			n;
{
    kern_return_t	kr;
    int			i;

    for ( i=0; i < n; i++ ) {
	kr = call_lproxy_ping(lproxyPort);
	xAssert( kr == KERN_SUCCESS ) ;
    }
    return KERN_SUCCESS;
}


