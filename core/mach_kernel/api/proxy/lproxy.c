/*     
 * $RCSfile: lproxy.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.26 $
 * $Date: 1993/11/09 17:18:38 $
 */

/* 
 * Implementation of x-kernel boundary crossing routines received by
 * a user task from the kernel.
 */

#include <stdio.h>
#include <mach.h> 
#include <servers/netname.h>
#include "xkernel.h"
#include "xk_mach.h"
#include "xk_mig_t.h"
#include "lproxy.h"
#include "xk_uproxy.h"
#include "proxy_util.h"
#include "proxy_offset.h"
#include "proxy_util.h"


typedef struct {
    mach_port_t		sendPort;
    mach_port_t		rcvPort;
    xobj_ext_id_t	peerId;
} PState;

typedef struct {
    xobj_ext_id_t	peerId;
} SState;


int	tracelproxyp;

boolean_t		xk_lproxy_server();
mach_msg_return_t	xk_mach_msg_server();


/* 
 * Variables shared by all instantiations of the LPROXY protocol.
 */
static	mach_port_t	rcvPortSet;	/* My receive port */

/* 
 * Maps ports that we receive message on to the corresponding proxy protocols. 
 */
static	Map		portMap;
#define PORT_MAP_SZ	5


/* 
 * map of xobj_ext_id_t's for real objects in other domains to their
 * proxy objects in this domain
 */
static  Map		proxyMap;
#define PROXY_MAP_SZ	113

#define LPROXY_NUM_INPUT_THREADS	10


#ifdef __STDC__

void		lproxy_init( XObj );
xkern_return_t	lproxyPingDownTest( XObj, int );
xkern_return_t	lproxyPingUpTest( XObj, int );

static XObj		createPassiveSessn( PORT_TYPE, xobj_ext_id_t );
static XObj		createProxySessn( mach_port_t, xobj_ext_id_t, XObj, XObj );
static xkern_return_t	lproxyCall( XObj, Msg *, Msg * );
static xkern_return_t 	lproxyCallPop( XObj, XObj, Msg *, VOID *, Msg * );
static xkern_return_t	lproxyClose( XObj );
static int		lproxyControlProtl( XObj, int, char *, int );
static int		lproxyControlSessn( XObj, int, char *, int );
static xkern_return_t	lproxyOpenDisable( XObj, XObj, XObj, Part * );
static XObj		lproxyOpen( XObj, XObj, XObj, Part * );
static xkern_return_t	lproxyOpenEnable( XObj, XObj, XObj, Part * );
static xkern_return_t 	lproxyPop( XObj, XObj, Msg *, VOID * );
static xmsg_handle_t	lproxyPush( XObj, Msg * );

#endif

static xkern_return_t
findSendPort( self, xkPeer )  
    XObj	self;
    char	*xkPeer;
{
    mach_port_t		mdp;
    PState		*ps = (PState *)self->state;
    kern_return_t	kr;

    mdp = mach_master_device_port();
    xAssert(xkPeer);
    if ( *xkPeer == 0 ) {
	xTraceP0(self, TR_MAJOR_EVENTS, "Talking to in-kernel proxy peer");
	ps->sendPort = mdp;
    } else {
	xTraceP1(self, TR_EVENTS, "master port == %x\n", mdp);
	kr = netname_look_up(name_server_port, "", xkPeer, &ps->sendPort);
	if ( kr == KERN_SUCCESS ) {
	    xTrace1(lproxyp, TR_EVENTS, "upProxyFind succeeds, kernPort == %x\n", ps->sendPort);
	} else {
	    sprintf(errBuf, "upProxyFind fails (%s)!", mach_error_string(kr));
	    xError(errBuf);
	    return XK_FAILURE;
	}
    }
    return XK_SUCCESS;
}

#define ERROR { sprintf(errBuf,	 "%s ROM file format error in line %d", self->fullName, i+1); \
		xError(errBuf); }


static void
loadRomInfo( self, peer )
    XObj	self;
    char	**peer;
{
    int	i;

    *peer = "";
    for ( i=0; rom[i][0]; i++ ) {
	if ( ! strcmp(rom[i][0], self->name) ||
	     ! strcmp(rom[i][0], self->fullName) ) {
	    if ( ! rom[i][1] || ! rom[i][2] ) {
		ERROR;
		continue;
	    }
	    if ( ! strncmp(rom[i][1], "uproxy") ) {
		*peer = rom[i][2];
	    } else {
		ERROR;
	    }
	}
    }
}


#ifdef XK_PROXY_MSG_HACK
#  define MSG_SERVER	xk_mach_msg_server
#else
#  define MSG_SERVER	mach_msg_server
#endif

static VOID *
startReader( readPort )
    VOID	*readPort;
{
    mach_msg_return_t	mr;
    LingeringMsg	*lMsg;

    xTrace0(lproxyp, TR_EVENTS, "lproxy reading thread starts");
    lMsg = X_NEW(LingeringMsg);
    lMsg->valid = 0;
    cthread_set_data(cthread_self(), lMsg);
    mr = MSG_SERVER(xk_lproxy_server, XK_MAX_MIG_MSG_LEN, *(mach_port_t *)readPort);
    /* 
     * Should never return
     */
    xError("mach_msg_server(lproxy) returns!");
    if ( mr != MACH_MSG_SUCCESS ) {
	sprintf(errBuf, "mach_msg_server error: %s\n", mach_error_string(mr));
	Kabort(errBuf);
    }
    return 0;
}



static xkern_return_t
lproxyPop( s, lls, msg, arg )
    XObj	s, lls;
    Msg		*msg;
    VOID	*arg;
{
    xTraceP0(s, TR_EVENTS, "lproxyPop");
    return xDemux(s, msg);
}


static xkern_return_t
lproxyCallPop( s, lls, msg, arg, rmsg )
    XObj	s, lls;
    Msg		*msg, *rmsg;
    VOID	*arg;
{
    xTraceP0(s, TR_EVENTS, "lproxyPop");
    return xCallDemux(s, msg, rmsg);
}


static XObj
createPassiveSessn( reqPort, extLls )
    PORT_TYPE		reqPort;
    xobj_ext_id_t	extLls;
{
    PState		*ps;
    XObj		lproxyProtl = 0;
    XObj		lls;
    kern_return_t	kr;
    xkern_return_t	xkr;

    if ( mapResolve(portMap, &reqPort, &lproxyProtl) == XK_FAILURE ) {
	xError("portMap query failure in do_lproxy_xDemux");
	lls = ERR_XOBJ;
    } else {
	xAssert(xIsValidXObj(lproxyProtl));
	ps = (PState *)lproxyProtl->state;
	lls = createProxySessn(ps->sendPort, extLls, 0, 0);
	if ( xIsXObj(lls) ) {
	    /* 
	     * Other messages may come through and use our newly
	     * created session ... that should be OK, but we'll
	     * artificially increase the ref count to make sure it
	     * doesn't go away before our message has a chance to 
	     * increase the ref count in xPop
	     */
	    xDuplicate(lls);
	    xAssert(xIsXObj(lls->up));
	    xk_master_unlock();
	    kr = call_uproxy_xDuplicate(ps->sendPort, ps->rcvPort, &xkr, (xobj_ext_id_t)lls->up, extLls);
	    xk_master_lock();
	    xAssert(lls->rcnt > 0);
	    lls->rcnt--;
	    if ( kr != KERN_SUCCESS && xkr != XK_SUCCESS ) {
		xTraceP0(lproxyProtl, TR_ERRORS, "couldn't duplicate remote lls");
		xClose(lls);
		lls = ERR_XOBJ;
	    }
	}
    }
    if ( ! xIsXObj(lls) ) {
	xTraceP0(lproxyProtl, TR_ERRORS,
		 "Could not create passive session");
    }
    return lls;
}


kern_return_t
do_lproxy_xDemux( reqPort, xkr, extLls, msgData, msgDataCnt, machMsgStart, msgOol, msgOolLen, msgIsInline, attr, attrLen )
    PORT_TYPE 		reqPort;
    xkern_return_t	*xkr;
    xobj_ext_id_t 	extLls;		/* remote object */
    char	 	*msgData;
    mach_msg_type_number_t msgDataCnt;
    xk_msg_attr_t	attr;
    mach_msg_type_number_t	attrLen;
    int			machMsgStart;
    xk_large_msg_data_t		msgOol;
    mach_msg_type_number_t	msgOolLen;
    boolean_t		msgIsInline;
{
    XObj		lls;
    Msg			msg;

    xTrace0(lproxyp, TR_EVENTS, "do_lproxy_xDemux called");

    xk_master_lock();

    if ( mapResolve(proxyMap, &extLls, &lls) == XK_FAILURE ) {
	xTrace1(lproxyp, TR_SOFT_ERRORS, "proxyMap lookup fails for extLls %x", (int)extLls);
	lls = createPassiveSessn(reqPort, extLls);

    }
    if ( xIsXObj(lls) ) {
	if ( msgIsInline ) {
#ifdef XK_PROXY_MSG_HACK
	    int	offset;
	    bool	res;
	    
	    /* 
	     * The message has been dynamically allocated in the MIG server
	     * and we have implicitly been given the reference to it.
	     * We'll free it whenever this x-kernel msg is destroyed.
	     * We thus avoid an extra bcopy.
	     */
	    xAssert(machMsgStart);
	    offset = (int)(msgData - machMsgStart);
	    xAssert(offset > 0);
	    xTrace1(lproxyp, TR_MORE_EVENTS, "xkernel data offset is %d", offset);
	    /* 
	     * We need to originally construct the message with the full
	     * buffer length (and then trim it down) to be able to take
	     * advantage of other message optimizations.
	     */
	    msgConstructInplace(&msg, (char *)machMsgStart, XK_MAX_MIG_MSG_LEN, xFree);
	    res = msgPopDiscard(&msg, offset);
	    xAssert( res == TRUE );
	    msgTruncate(&msg, msgDataCnt);
	    xAssert( msgLen(&msg) == msgDataCnt );
#else
	    char		*msgBuf;
	    
	    msgConstructAllocate(&msg, msgDataCnt, &msgBuf);
	    bcopy(msgData, msgBuf, msgDataCnt);
#endif /* XK_PROXY_MSG_HACK */
	} else {
#ifdef XK_PROXY_MSG_HACK
	    xFree((char *)machMsgStart);
#endif	    
	    xTrace0(lproxyp, TR_MORE_EVENTS, "demux message is out-of-line");
	    xAssert(msgOol);
	    msgConstructInplace(&msg, msgOol, msgOolLen, oolFree);
	}
	if ( attrLen ) {
	    xkern_return_t	xrv;

	    xTraceP1(lls, TR_EVENTS, "attaching message attribute of len %d", attrLen);
	    xrv = msgSetAttr(&msg, 0, attr, attrLen);
	    xAssert( xrv == XK_SUCCESS );
	}
	/* 
	 * Second argument is just a dummy arg.
	 */
	*xkr = xPop(lls, lls, &msg, 0);
	msgDestroy(&msg);
    } else {
	*xkr = XK_FAILURE;
    }
    xk_master_unlock();

    return KERN_SUCCESS;
}


kern_return_t
do_lproxy_xCallDemux( reqPort, xkr, extLls, reqMsgData, reqMsgLen, machMsgStart,
		      reqOol, reqOolLen, reqIsInline, repMsgData, repMsgLen,
		      repOol, repOolLen, reqAttr, reqAttrLen )
    PORT_TYPE 			reqPort;
    xkern_return_t		*xkr;
    xobj_ext_id_t 		extLls;		/* remote object */
    char	 		*reqMsgData;
    mach_msg_type_number_t 	reqMsgLen;
    xk_large_msg_data_t		reqOol;
    mach_msg_type_number_t	reqOolLen;
    boolean_t			reqIsInline;
    char	 		*repMsgData;
    mach_msg_type_number_t 	*repMsgLen;
    int				machMsgStart;	/* address of request msg */
    xk_large_msg_data_t		*repOol;
    mach_msg_type_number_t	*repOolLen;
    xk_msg_attr_t		reqAttr;
    mach_msg_type_number_t	reqAttrLen;
{
    XObj		lls;
    Msg			reqMsg, repMsg;

    xTrace0(lproxyp, TR_EVENTS, "do_lproxy_xCallDemux called");

    xk_master_lock();
    lingeringMsgClear();
    if ( mapResolve(proxyMap, &extLls, &lls) == XK_FAILURE ) {
	xTrace1(lproxyp, TR_SOFT_ERRORS, "proxyMap lookup fails for extLls %x", (int)extLls);
	lls = createPassiveSessn(reqPort, extLls);
    }
    if ( xIsXObj(lls) ) {
	if ( reqIsInline ) {

#ifdef XK_PROXY_MSG_HACK
	    int	offset;
	    bool	res;
	    
	    /* 
	     * The message has been dynamically allocated in the MIG server
	     * and we have implicitly been given the reference to it.
	     * We'll free it whenever this x-kernel msg is destroyed.
	     * We thus avoid an extra bcopy.
	     */
	    xAssert(machMsgStart);
	    offset = (int)(reqMsgData - machMsgStart);
	    xAssert(offset > 0);
	    xTrace1(lproxyp, TR_MORE_EVENTS, "xkernel data offset is %d", offset);
	    msgConstructInplace(&reqMsg, (char *)machMsgStart, XK_MAX_MIG_MSG_LEN, xFree);
	    res = msgPopDiscard(&reqMsg, offset);
	    xAssert( res == TRUE );
	    msgTruncate(&reqMsg, reqMsgLen);
	    xAssert( msgLen(&reqMsg) == reqMsgLen );
#else
	    char	*msgBuf;
	    
	    /* 
	     * We have to copy the message because someone above us could
	     * save a reference to it.
	     */
	    msgConstructAllocate(&reqMsg, reqMsgLen, &msgBuf);
	    bcopy(reqMsgData, msgBuf, reqMsgLen);
#endif
	} else {
#ifdef XK_PROXY_MSG_HACK
	    xFree((char *)machMsgStart);
#endif	    
	    xTrace0(lproxyp, TR_MORE_EVENTS, "callDemux request is out-of-line");
	    xAssert(reqOol);
	    msgConstructInplace(&reqMsg, reqOol, reqOolLen, oolFree);
	}
	if ( reqAttrLen ) {
	    xkern_return_t	xrv;

	    xTraceP1(lls, TR_EVENTS,
		     "attaching message attribute of len %d", reqAttrLen);
	    xrv = msgSetAttr(&reqMsg, 0, reqAttr, reqAttrLen);
	    xAssert( xrv == XK_SUCCESS );
	}
	msgConstructEmpty(&repMsg);
	/* 
	 * Second argument is just a dummy arg.
	 */
	*xkr = xCallPop(lls, lls, &reqMsg, 0, &repMsg);
	*repOolLen = 0;
	if ( *xkr == XK_SUCCESS ) {
	    xTrace1(lproxyp, TR_EVENTS, "xCallPop returns success, %d bytes",
		    msgLen(&repMsg));
	    if ( msgLen(&repMsg) > XK_MAX_MSG_INLINE_LEN ) {
		DeallocFunc	dFunc;
		VOID		*dArg;

		msgToOol(&repMsg, repOol, &dFunc, &dArg);
		*repOolLen = msgLen(&repMsg);
		lingeringMsgSave(dFunc, dArg, *repOolLen);
	    } else {
		xAssert(msgLen(&repMsg) <= *repMsgLen);
		msg2buf(&repMsg, repMsgData);
		*repMsgLen = msgLen(&repMsg);
	    }
	} else {
	    xTrace0(lproxyp, TR_EVENTS, "xCallPop returns failure");
	}
	msgDestroy(&reqMsg);
	msgDestroy(&repMsg);
    } else {
	*xkr = XK_FAILURE;
    }
    xk_master_unlock();

    return KERN_SUCCESS;
}




kern_return_t
do_lproxy_xOpenDone( reqPort, xkr, extLls, extLlp, hlpRcv, hlpType )
    mach_port_t		reqPort;
    xkern_return_t	*xkr;
    xobj_ext_id_t	extLls, extLlp, hlpRcv, hlpType;
{
    XObj	s, llp;
    PState	*ps;

    xTrace1(lproxyp, TR_EVENTS, "do_lproxy_xOpenDone called, extLls == %x", (int)extLls);

    xk_master_lock();

    /* 
     * We should have pre-existing XObjects corresponding to llp,
     * hlpRcv and hlpType.  We will need to create a new proxy session
     * corresponding to lls.
     */
    if ( ! xIsValidXObj((XObj)hlpRcv) ) {
	xTrace1(lproxyp, TR_ERRORS, "do_openDone -- xobj %x is unsafe", hlpRcv);
	*xkr = XK_FAILURE;
    } else if ( ! xIsValidXObj((XObj)hlpType) ) {
	xTrace1(lproxyp, TR_ERRORS, "do_openDone -- xobj %x is unsafe", hlpType);
	*xkr = XK_FAILURE;
    } else if ( mapResolve(proxyMap, &extLlp, &llp) == XK_FAILURE ) {
	xTrace1(lproxyp, TR_ERRORS, "do_openDone -- no proxy object for llp %x",
		(int)extLlp);
	*xkr = XK_FAILURE;
    } else if ( mapResolve(proxyMap, &extLls, 0) == XK_SUCCESS ) {
	xTraceP1((XObj)llp, TR_SOFT_ERRORS,
		 "proxy object for %x already exists", (int)extLls);
	*xkr = XK_SUCCESS;
    } else {
	ps = (PState *)((XObj)llp)->state;
	s = createProxySessn(ps->sendPort, extLls, (XObj)hlpRcv, (XObj)hlpType);
	if ( s == ERR_XOBJ ) {
	    *xkr = XK_FAILURE;
	} else {
	    *xkr = xOpenDone((XObj)hlpRcv, s, (XObj)llp);
	}
    }
    xk_master_unlock();

    return KERN_SUCCESS;
}


kern_return_t
do_lproxy_dumpXObj( reqPort, xkr, extObj, dump )
    mach_port_t		reqPort;
    xkern_return_t	*xkr;
    xobj_ext_id_t	extObj;
    xk_xobj_dump_t	*dump;
{
    xTrace0(lproxyp, TR_DETAILED, "do_lproxy_dumpXObj called");
    xk_master_lock();
    if ( ! xIsValidXObj((XObj)extObj) ) {
	xTrace1(lproxyp, TR_ERRORS, "do_dumpXObj -- xobj %x is unsafe", extObj);
	*xkr = XK_FAILURE;
    } else {
	XObj obj = (XObj)extObj;
	xIfTrace(lproxyp, TR_DETAILED) {
	    xPrintXObj(obj);
	}
	dump->type = obj->type;
	dump->myprotl = (xobj_ext_id_t)obj->myprotl;
/*	if ( xIsProtocol(obj) ) { */
	    dump->hlpRcv = (xobj_ext_id_t)ERR_XOBJ;
	    dump->hlpType = (xobj_ext_id_t)ERR_XOBJ;
#if 0
	} else {
	    dump->hlpRcv = peerProtl(obj->up);
	    dump->hlpType = peerProtl(xHlpType(obj));
	}
#endif
	strcpy(dump->name, obj->name);
	strcpy(dump->instName, obj->instName);
	*xkr = XK_SUCCESS;
	xTrace2(lproxyp, TR_FULL_TRACE,
		"dumpXObj: sending name: %s  instName: %s",
		dump->name, dump->instName);
    } 
    xk_master_unlock();
    return KERN_SUCCESS;
}


kern_return_t
do_lproxy_ping( reqPort )
    mach_port_t		reqPort;
{
    xTrace0(lproxyp, TR_EVENTS, "lproxy Ping");
    return KERN_SUCCESS;
}




/* 
 * Normal UPI operations (called by objects from within my own
 * protection domain)
 */


static xkern_return_t
lproxyOpenDisable( self, hlpRcv, hlpType, p )
    XObj self, hlpRcv, hlpType;
    Part *p;
{
    xk_part_t		extPart;
    xkern_return_t	xkr;
    kern_return_t	kr;
    PState		*ps = (PState *)self->state;
    int			bufLen;

    xTraceP0(self, TR_DETAILED, "openDisable");

    bufLen = sizeof(extPart);    
    if ( partExternalize(p, &extPart, &bufLen) == XK_FAILURE ) {
	xTraceP0(self, TR_ERRORS, "could not externalize participant");
	return XK_FAILURE;
    }
    xk_master_unlock();
    kr = call_uproxy_xOpenDisable(ps->sendPort, ps->rcvPort, &xkr,
				 (xobj_ext_id_t)hlpRcv, (xobj_ext_id_t)hlpType,
				 ps->peerId, extPart);
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(self, TR_ERRORS, "mach msg error %s in call_xOpenDisable",
		 mach_error_string(kr));
	xkr = XK_FAILURE;
    } else {
	if ( xkr == XK_FAILURE ) {
	    xTraceP0(self, TR_SOFT_ERRORS, "call_xOpenDisable fails");
	}
    }
    xk_master_lock();
    return xkr;
}


static xkern_return_t
lproxyOpenEnable( self, hlpRcv, hlpType, p )
    XObj self, hlpRcv, hlpType;
    Part *p;
{
    xk_part_t		extPart;
    xkern_return_t	xkr;
    kern_return_t	kr;
    PState		*ps = (PState *)self->state;
    int			bufLen;

    xTraceP0(self, TR_DETAILED, "openEnable");

    bufLen = sizeof(extPart);    
    if ( partExternalize(p, &extPart, &bufLen) == XK_FAILURE ) {
	xTraceP0(self, TR_ERRORS, "could not externalize participant");
	return XK_FAILURE;
    }
    xk_master_unlock();
    kr = call_uproxy_xOpenEnable(ps->sendPort, ps->rcvPort, &xkr,
				 (xobj_ext_id_t)hlpRcv, (xobj_ext_id_t)hlpType,
				 ps->peerId, extPart);
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(self, TR_ERRORS, "mach msg error %s in call_xOpenEnable",
		 mach_error_string(kr));
	xkr = XK_FAILURE;
    } else {
	if ( xkr == XK_FAILURE ) {
	    xTraceP0(self, TR_SOFT_ERRORS, "call_xOpenEnable fails");
	}
    }
    xk_master_lock();
    return xkr;
}


#define CHECK_KERN_RESULT( _res, _routine, _retVal )				\
{ 										\
       if ( (_res) != KERN_SUCCESS ) {						\
		xTraceP2(s, TR_ERRORS, "mach msg error %s in call_uproxy_%s",	\
		 			(_routine), mach_error_string(_res));	\
		return (_retVal);						\
       }									\
}						  



static int
callControl( port, obj, extObj, op, buf, len )
    PORT_TYPE		port;
    XObj		obj;
    xobj_ext_id_t	extObj;
    int		op;
    char	*buf;
    int		len;
{
    kern_return_t	kr;
    int			bufLen;
    xk_ctl_buf_t	ctlBuf;

    xTraceP0(obj, TR_EVENTS, "callControl called");

    xTraceP1(obj, TR_EVENTS, "callControl: send port is %x\n", port);

    xk_master_unlock();
    bcopy(buf, ctlBuf, len);
    bufLen = len;
    kr = call_uproxy_xControl(port, extObj, op, ctlBuf, &bufLen);
    xk_master_lock();
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(obj, TR_ERRORS, "mach msg error %s in call_xControl",
		 mach_error_string(kr));
	return -1;
    }
    if ( bufLen > len ) {
	xError("buffer size grew in lproxyControl");
	xTraceP2(obj, TR_ERRORS, "was %d, now %d", len, bufLen);
	bufLen = -1;
    } else {
	if ( bufLen > 0 ) {
	    bcopy(ctlBuf, buf, bufLen);
	}
    }
    return bufLen;
}


static int
lproxyControlProtl( s, op, buf, len )
    XObj	s;
    int		op;
    char	*buf;
    int		len;
{
    PState		*ps = (PState *)s->state;

    xTraceP0(s, TR_EVENTS, "lproxyControlProtl called");

    return callControl(ps->sendPort, s, ps->peerId, op, buf, len);
}


static int
lproxyControlSessn( s, op, buf, len )
    XObj	s;
    int		op;
    char	*buf;
    int		len;
{
    SState		*ss = (SState *)s->state;
    PState		*ps = (PState *)xMyProtl(s)->state;

    xTraceP0(s, TR_EVENTS, "lproxyControlSessn called");

    return callControl(ps->sendPort, s, ss->peerId, op, buf, len);
}


static void
sessnInit( s )
    XObj	s;
{
    s->push = lproxyPush;
    s->close = lproxyClose;
    s->call = lproxyCall;
    s->control = lproxyControlSessn;
    s->pop = lproxyPop;
    s->callpop = lproxyCallPop;
}


static xkern_return_t
checkHlpValidity( hlp, expectedHlp )
    xobj_ext_id_t	hlp;
    XObj		expectedHlp;
{
    if ( ! xIsValidXObj((XObj)hlp) ) {
	xTrace1(lproxyp, TR_ERRORS,
		"lproxyFindProxySessn: lls->hlp %x is not valid XObj",
		hlp);
	return XK_FAILURE;
    }
    if ( expectedHlp && expectedHlp != (XObj)hlp ) {
	xTrace2(lproxyp, TR_ERRORS,
		"lproxyFindProxySessn: hlp mismatch (expected: %x, internal: %x)",
		(int)expectedHlp, (int)hlp);
	return XK_FAILURE;
    }
    return XK_SUCCESS;
}


/* 
 * Find (or create) an appropriate proxy session.  expHlpRcv and
 * expHlpType will only be used for consistency checks and may be
 * null.  We assume that the other proxy is keeping a reference count
 * representing this new session we are creating.  Thus if we don't
 * actually create a session, we will execute a close on the lower
 * proxy. 
 */
static XObj
createProxySessn( uproxyPort, extLls, expHlpRcv, expHlpType )
    mach_port_t		uproxyPort;
    xobj_ext_id_t	extLls;
    XObj		expHlpRcv;
    XObj		expHlpType;
{
    XObj		lproxyProtl;
    XObj		lls;
    kern_return_t	kr;
    xkern_return_t	xkr;
    xk_xobj_dump_t	objDump;
    SState		*ss;
    
    xAssert(proxyMap);
    if ( mapResolve(proxyMap, &extLls, &lls) == XK_FAILURE ) {
	/* 
	 * Get the poop on the real session for which we are
	 * creating the proxy  
	 */
	kr = call_uproxy_dumpXObj(uproxyPort, &xkr, extLls, &objDump);
	if ( kr != KERN_SUCCESS || xkr == XK_FAILURE ) {
	    xTrace1(lproxyp, TR_ERRORS, "mach_msg error for call_dumpXObj (%s)",
		    mach_error_string(kr));
	    return ERR_XOBJ;
	}
	if ( xkr == XK_FAILURE ) {
	    xTrace1(lproxyp, TR_ERRORS, "call_dumpXObj(%x) fails", extLls);
	    return ERR_XOBJ;
	}
	/* 
	 * Find the local proxy protocol which should be used to
	 * create this session.  This is unnecessary for xOpens but
	 * provides a sanity check.
	 */
	if ( mapResolve(proxyMap, &objDump.myprotl, &lproxyProtl) == XK_FAILURE ) {
	    xTrace2(lproxyp, TR_ERRORS,
		    "No proxy protl corresponds to  external lls->myprotl %s/%s",
		    objDump.name, objDump.instName);
	    return ERR_XOBJ;
	}
	if ( checkHlpValidity(objDump.hlpRcv, expHlpRcv) == XK_FAILURE ||
	     checkHlpValidity(objDump.hlpType, expHlpType) == XK_FAILURE ) {
	    return ERR_XOBJ;
	}
	lls = xCreateSessn(sessnInit, (XObj)objDump.hlpRcv, (XObj)objDump.hlpType, lproxyProtl, 0, 0);
	if ( lls == ERR_XOBJ ) {
	    xTraceP0(lproxyProtl, TR_ERRORS, "xCreateSessn fails!");
	    return ERR_XOBJ;
	}
	lls->binding = mapBind(proxyMap, &extLls, lls);
	ss = (SState *)xMalloc(sizeof(SState));
	ss->peerId = extLls;
	lls->state = ss;
	xAssert(lls->binding != ERR_BIND);
    } else {
	xTrace1(lproxyp, TR_MORE_EVENTS, "lproxy session %x already exists", lls);
	kr = call_uproxy_xClose(uproxyPort, &xkr, extLls);
	if ( kr != KERN_SUCCESS ) {
	    xTrace1(lproxyp, TR_ERRORS, "mach msg error %s in call_xClose",
		     mach_error_string(kr));
	}
    }
    return lls;
}


static XObj
lproxyOpen( self, hlpRcv, hlpType, p )
    XObj self, hlpRcv, hlpType;
    Part *p;
{
    xk_part_t		extPart;
    kern_return_t	kr;
    xobj_ext_id_t	extLls;
    XObj		lls;
    PState		*ps = (PState *)self->state;
    int			bufLen;
    
    xTraceP0(self, TR_DETAILED, "open");

    bufLen = sizeof(extPart);    
    if ( partExternalize(p, &extPart, &bufLen) == XK_FAILURE ) {
	xTraceP0(self, TR_ERRORS, "could not externalize participant");
	return ERR_XOBJ;
    }
    xk_master_unlock();
    kr = call_uproxy_xOpen(ps->sendPort, ps->rcvPort, &extLls,
			   (xobj_ext_id_t)hlpRcv, (xobj_ext_id_t)hlpType,
			   ps->peerId, extPart);
    xk_master_lock();
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(self, TR_ERRORS, "mach msg error %s in call_xOpen",
		 mach_error_string(kr));
	lls = ERR_XOBJ;
    } else {
	xTraceP0(self, TR_DETAILED, "call_xOpen returns success");
	if ( xIsXObj((XObj)extLls) ) {
	    lls = createProxySessn(ps->sendPort, extLls, hlpRcv, hlpType);
	} else {
	    xTraceP0(self, TR_SOFT_ERRORS, "call_xOpen fails");
	    lls = ERR_XOBJ;
	}
    }
    return lls;
}


static xmsg_handle_t
lproxyPush( s, msg )
    XObj	s;
    Msg		*msg;
{
    PState		*ps = (PState *)xMyProtl(s)->state;
    SState		*ss = (SState *)s->state;
    xmsg_handle_t	handle;
    kern_return_t	kr;
    xkern_return_t	xkr;
    xk_and_mach_msg_t	xmsg;
    char		*oolBuf;
    int			inlineLen, oolLen;
    DeallocFunc		dFunc = 0;
    VOID		*dArg;

    xTraceP0(s, TR_EVENTS, "push called");

    if ( msgLen(msg) <= XK_MAX_MSG_INLINE_LEN ) {
	if ( msgIsOkToMangle(msg, (char **)&xmsg.machMsg, XK_PUSH_REQ_OFFSET) ) {
	    xTraceP0(s, TR_MORE_EVENTS, "push -- msg hack works");
	    xAssert(&xmsg.machMsg);
	    xmsg.xkMsg = 0;
	} else {
	    xmsg.machMsg = 0;
	    xmsg.xkMsg = msg;
	}
	inlineLen = msgLen(msg);
	oolLen = 0;
	oolBuf = 0;
    } else {
	xmsg.machMsg = 0;
	xmsg.xkMsg = 0;
	inlineLen = 0;
	oolLen = msgLen(msg);
	xkr = msgToOol(msg, &oolBuf, &dFunc, &dArg);
	xAssert( xkr == XK_SUCCESS ) ;
    }
    xk_master_unlock();

    kr = call_uproxy_xPush(ps->sendPort, &handle, ss->peerId, (char *)&xmsg,
			   inlineLen, oolBuf, oolLen, msgGetAttr(msg, 0), msg->attrLen);
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(s, TR_ERRORS, "mach msg error %s in call_xPush",
		 mach_error_string(kr));
	handle = XMSG_ERR_HANDLE;
    }
    xTraceP1(s, TR_MORE_EVENTS, "push returns %x", (int)handle);

    xk_master_lock();
    if ( dFunc ) {
	dFunc(dArg, oolLen);
    }
    return handle;
}



static xkern_return_t
lproxyCall( s, msg, rmsg )
    XObj	s;
    Msg		*msg;
    Msg		*rmsg;
{
    PState		*ps = (PState *)xMyProtl(s)->state;
    SState		*ss = (SState *)s->state;
    kern_return_t	kr;
    xkern_return_t	xkr;
    int			rmsgLen;
    Msg			tmpMsg;
    char		*machMsgBuf;
    bool		res;
    int			repoolCnt = 0;
    char		*reqool, *repool;

    xTraceP0(s, TR_EVENTS, "call called");

    msgConstructAllocate(&tmpMsg, XK_MAX_MIG_MSG_LEN, &machMsgBuf);
    res = msgPopDiscard(&tmpMsg, XK_CALL_REP_OFFSET);
    xAssert( res == TRUE );
    rmsgLen = XK_MAX_MSG_INLINE_LEN;

    if ( msgLen(msg) <= XK_MAX_MSG_INLINE_LEN ) {
	xk_master_unlock();
	kr = call_uproxy_xCall(ps->sendPort, &xkr, ss->peerId,
			       (char *)msg, msgLen(msg), (char *)0, 0,
			       machMsgBuf, &rmsgLen,
			       &repool, &repoolCnt, msgGetAttr(msg, 0), msg->attrLen);
    } else {
	DeallocFunc	dFunc = 0;
	VOID		*dArg;

	xTraceP0(s, TR_EVENTS, "call passing request out-of-line");
	xkr = msgToOol(msg, &reqool, &dFunc, &dArg);
	xAssert( xkr == XK_SUCCESS );
	xk_master_unlock();
	kr = call_uproxy_xCall(ps->sendPort, &xkr, ss->peerId,
			       (char *)0, 0, (char *)reqool, msgLen(msg),
			       machMsgBuf, &rmsgLen,
			       &repool, &repoolCnt, msgGetAttr(msg, 0), msg->attrLen);
	if ( dFunc ) {
	    dFunc(dArg, msgLen(msg));
	}
    }

    xk_master_lock();

    CHECK_KERN_RESULT(kr, "xCall", XK_FAILURE);
    if ( xkr == XK_SUCCESS ) {
	if ( repoolCnt > 0 ) {
	    xTraceP0(s, TR_MORE_EVENTS, "reply msg came to us out-of-line");
	    xAssert(repool);
	    msgDestroy(&tmpMsg);
	    msgConstructInplace(&tmpMsg, repool, repoolCnt, oolFree);
	} else {
	    xTraceP1(s, TR_MORE_EVENTS, "call truncates reply msg to %d", rmsgLen);
	    xAssert( msgLen(&tmpMsg) >= rmsgLen );
	    msgTruncate(&tmpMsg, rmsgLen);
	}
	msgAssign(rmsg, &tmpMsg);
    }
    msgDestroy(&tmpMsg);
    xTraceP1(s, TR_MORE_EVENTS, "call returns %d", xkr);
    return xkr;
}


static xkern_return_t
lproxyClose( s )
    XObj	s;
{
    PState		*ps = (PState *)xMyProtl(s)->state;
    SState		*ss = (SState *)s->state;
    kern_return_t	kr;
    xkern_return_t	xkr;

    xTraceP0(s, TR_EVENTS, "close called");

    if ( mapUnbind(proxyMap, &ss->peerId) == XK_FAILURE ) {
	xTraceP1(s, TR_ERRORS, "map unbind failure (%x) in close", (int)ss->peerId);
    }
    xk_master_unlock();
    kr = call_uproxy_xClose(ps->sendPort, &xkr, ss->peerId);
    xk_master_lock();
    if ( kr != KERN_SUCCESS ) {
	xTraceP1(s, TR_ERRORS, "mach msg error %s in call_xClose",
		 mach_error_string(kr));
	xkr = XK_FAILURE;
    }
    xTraceP1(s, TR_MORE_EVENTS, "lproxyClose returns %d", xkr);
    xDestroy(s);
    return xkr;
}


static xkern_return_t
lproxyShutDown( p )
    XObj	p;
{
    PState	*ps = (PState *)p->state;

    xTraceP0(p, TR_MAJOR_EVENTS, "shutdown");
    call_uproxy_abort(ps->sendPort, ps->rcvPort);
    return XK_SUCCESS;
}


void
lproxy_init( self )
    XObj	self;
{
    kern_return_t	kr;
    char		*uproxy;
    PState		*ps;
    Bind		b;
    int			i;

    xTrace1(lproxyp, TR_GROSS_EVENTS, "%s init called", self->fullName);
    ps = (PState *)xMalloc(sizeof(PState));
    self->state = ps;
    loadRomInfo(self, &uproxy);
    if ( findSendPort(self, uproxy) != XK_SUCCESS ) {
	return;
    }
    /* 
     * These intializations should only happen for the first lproxy protocol
     */
    if ( proxyMap == 0 ) {
	proxyMap = mapCreate(PROXY_MAP_SZ, sizeof(xobj_ext_id_t));
	xAssert( proxyMap && (proxyMap != (Map)-1) );
    }
    if ( portMap == 0 ) {
	portMap = mapCreate(PORT_MAP_SZ, sizeof(PORT_TYPE));
	xAssert( portMap && (portMap != (Map)-1) );
    }
    if ( rcvPortSet == 0 ) {
	kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
				&rcvPortSet);
	if ( kr != KERN_SUCCESS ) {
	    xTrace1(lproxyp, TR_ERRORS, "mach_port_allocate (port_set) error: %s\n", mach_error_string(kr));
	    exit(1);
	}
	xkIncreaseCthreadLimit(LPROXY_NUM_INPUT_THREADS);
	for ( i=0; i < LPROXY_NUM_INPUT_THREADS; i++ ) {
	    cthread_fork( startReader, (VOID *)&rcvPortSet );
	}
    }
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			    &ps->rcvPort);
    if ( kr != KERN_SUCCESS ) {
	xTrace1(lproxyp, TR_ERRORS, "mach_port_allocate (receive_port) error: %s\n", mach_error_string(kr));
	exit(1);
    }
    kr = mach_port_move_member(mach_task_self(), ps->rcvPort, rcvPortSet);
    if ( kr != KERN_SUCCESS ) {
	xTrace1(lproxyp, TR_ERRORS, "mach_port_move_member error: %s\n", mach_error_string(kr));
	exit(1);
    }
    b = mapBind(portMap, &ps->rcvPort, self);
    xAssert(b != ERR_BIND);
    /* 
     * Find the external ID for the protocol for which this instantiation
     * is a proxy
     */
    xTraceP0(self, TR_EVENTS, "looking up id of real object");
    kr = call_uproxy_xGetProtlByName(ps->sendPort, self->instName, &ps->peerId);
    if ( kr == KERN_SUCCESS && ps->peerId ) {
	xTraceP1(self, TR_EVENTS,
		"call_uproxy_xGetProtlByName result: obj %x\n", (int)ps->peerId);
    } else {
	xTraceP1(self, TR_ERRORS, 
		"call_uproxy_xGetProtlByName failed: %s",
		 (kr == KERN_SUCCESS) ? "no such object" :
		 mach_error_string(kr));
	return;
    }
    mapBind(proxyMap, &ps->peerId, self);
    self->shutdown = lproxyShutDown;
    self->openenable = lproxyOpenEnable;
    self->opendisable = lproxyOpenDisable;
    self->open = lproxyOpen;
    self->control = lproxyControlProtl;

/*     evSchedule(callShutDown, 0, 5 * 1000 * 1000); */


#if 0
    /* 
     * Just a test ...
     */
    kr = call_uproxy_xOpen(sendPort);
    xTrace1(lproxyp, TR_ERRORS, "call_uproxy_xOpen result: %s\n", mach_error_string(kr));
    kr = call_uproxy_xGetProtlByName(sendPort, "foo", &objId);
    xTrace2(lproxyp, TR_ERRORS,
	    "call_uproxy_xGetProtlByName result: %s, obj %x\n",
	    mach_error_string(kr), (int)objId);
#endif
    
}



xkern_return_t
lproxyPingDownTest( self, n )
    XObj	self;
    int		n;
{
    PState		*ps = (PState *)self->state;
    int			i;
    kern_return_t	kr;

    xTraceP1(self, TR_EVENTS, "run ping down test: %d times", n);
    for ( i=0; i < n; i++ ) {
	kr = call_uproxy_ping(ps->sendPort);
	xAssert( kr == KERN_SUCCESS );
    }
    return XK_SUCCESS;
}


xkern_return_t
lproxyPingUpTest( self, n )
    XObj	self;
    int		n;
{
    PState		*ps = (PState *)self->state;
    kern_return_t	kr;

    xTraceP1(self, TR_EVENTS, "run ping up test: %d times", n);
    kr = call_uproxy_pingtest(ps->sendPort, ps->rcvPort, n);
    return kr == KERN_SUCCESS ? XK_SUCCESS : XK_FAILURE;
}


