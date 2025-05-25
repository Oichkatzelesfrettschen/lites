/*     
 * $RCSfile: sunrpctest.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.16 $
 * $Date: 1993/05/25 22:44:58 $
 */

/*
 * Ping-pong test of SUNRPC
 */

#if defined(__GNUC__) && ! defined(XK_DEBUG)
#  define XK_USE_INLINE
#endif

#include <sys/types.h>
#include "xkernel.h"
#include <rpc/rpc.h>
#include "sunrpc.h"

/*
 * These definitions describe the lower protocol
 */
#define HOST_TYPE IPhost
#define INIT_FUNC sunrpctest_init
#define TRACE_VAR tracesunrpctestp
#define PROT_STRING "sunrpc"

/* 
 * If a host is booted without client/server parameters and matches
 * one of these addresses, it will come up in the appropriate role.
 */
static HOST_TYPE ServerAddr = { 0, 0, 0, 0 };
static HOST_TYPE ClientAddr = { 0, 0, 0, 0 };

static	long	prog = 100, proc = 5, vers = 2;
static	long	clientPort = 2000, serverPort = 1002;

#define TRIPS 100
#define TIMES 1
#define DELAY 3
/*
 * Define to do timing calculations
 */
#define TIME
#define SAVE_SERVER_SESSN
#define RPCTEST
#define CUSTOM_ASSIGN

static void
setServer( p )
    Part	*p;
{
    partPush(*p, &ServerAddr, sizeof(IPhost));	/* IP host 		*/
    partPush(*p, &serverPort, sizeof(long));
    partPush(*p, &prog, sizeof(long));
    partPush(*p, &vers, sizeof(long));
}

static void
serverSetPart( p )
    Part	*p;
{
    partInit(p, 1);
    setServer(p);
}


static void
clientSetPart( p )
    Part	*p;
{
    partInit(p, 2);
    setServer(p);
#if 1
    partPush(p[1], ANY_HOST, 0);
    partPush(p[1], &clientPort, sizeof(long));
#endif    
}


static int lens[] = { 
  1000, 2000, 4000, 8000, 16000
};


#include "common_test.c"


static xkern_return_t
sunrpctestCallDemux( self, lls, dg, rMsg )
    XObj self, lls;
    Msg *dg, *rMsg;
{
    bool	b;

    msgAssign(rMsg, dg);
    xAssert(msgLen(rMsg) >= sizeof(long));
    b = msgPopDiscard(rMsg, sizeof(long));
    xAssert( b == TRUE );
    return XK_SUCCESS;
}


static int
sunrpcTryCall( sessn, times, length )
  XObj sessn;
  int times;
  int length;
{
    xkern_return_t ret_val;
    int i;
    Msg	savedMsg, request, reply;
    char *buf;
    int c = 0;
    
    /* 
     * We need to push the procedure number on the message itself.  
     * :-/
     */
    msgConstructAllocate(&savedMsg, length + sizeof(long), &buf);
    *(long *)buf = proc;

    msgConstructEmpty(&reply);
    msgConstructEmpty(&request);
    for (i=0; i<times; i++) {
	msgAssign(&request, &savedMsg);
	ret_val = xCall(sessn, &request, &reply);
	xIfTrace(prottest, TR_MAJOR_EVENTS) {
	    putchar('.');
	    if (! (++c % 50)) {
		putchar('\n');
	    }
	}
	if( ret_val == XK_FAILURE ) {
	    printf( "RPC call error %d\n" , ret_val );
	    goto abort;
	}
	if (msgLen(&reply) != length) {
	    printf("Bizarre reply length.  Expected %d, received %d\n",
		   length, msgLen(&reply));
	    goto abort;
	}
	msgTruncate(&reply, 0);
    }

abort:
    msgDestroy(&savedMsg);
    msgDestroy(&reply);
    msgDestroy(&request);
    return i;
}


static void
testInit()
{
    serverCallDemux = sunrpctestCallDemux;
    tryCall = sunrpcTryCall;
}
