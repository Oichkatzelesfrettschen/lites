/*
 * common_test.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.61 $
 * $Date: 1993/07/26 23:56:31 $
 */

/*
 * This code implements a ping-pong test for another protocol.
 * The same protocol code is used for both the client and the server.  
 *
 * It expects the definitions of following macros which describe
 * the lower protocol:
 *
 *	HOST_TYPE (e.g., ETHhost)
 *	INIT_FUNC (e.g., ethtest_init)
 *	TRACE_VAR (e.g., ethtestp)
 *	PROT_STRING (e.g., "eth")
 *
 * It also needs declarations for Client and Server addresses, e.g.:
 *	HOST_TYPE ServerAddr = { 0xC00C, 0x4558, 0x04d2 };  
 *	HOST_TYPE ClientAddr = { 0xC00C, 0x4558, 0x2694 };  
 *
 * It also needs definitions for the following macros controlling the test:
 *	TRIPS  (number of round trips to make)
 *	TIMES  (number of tests for each length)
 *	DELAY  (number of seconds to delay between each test
 *		(and timeout for declaring failure))
 *
 * Define the macro TIME to do timing calculations.
 *
 * *Finally*, define an array of integers 'lens[]' with the lengths for the
 * tests:
 *
 *	static int lens[] = { 
 *		  1, 200, 400, 600, 800, 1000, 1200
 *	};
 *
 */

/* STREAM_TEST may not work if simul>1 */

#include "xkernel.h"
#ifndef XKMACHKERNEL
#include "x_stdio.h"
#else
#include "assert.h"
#endif ! XKMACHKERNEL
#ifdef XK_MEMORY_THROTTLE
#include "xk_malloc.h"
#endif XK_MEMORY_THROTTLE

typedef struct {
    XObj	lls;
    /* 
     * Client only 
     */
    XObj	controlSessn;
    int		sentMsgLength;
    Msg		savedMsg;
    /* 
     * Async only
     */
    int			clientRcvd;
    xmsg_handle_t	clientPushResult;
    xmsg_handle_t	serverPushResult;
    int			idle;
    /* 
     * stream only
     */
    int			receivedLength;
} PState;


int	TRACE_VAR ;

#ifdef __STDC__

int	INIT_FUNC( XObj );

static	void 	client( Event, void * );
static 	void 	server( Event, void * );
static 	int	isServerDefault( XObj );
static 	int	isClientDefault( XObj );
static 	int	(*isServer)( XObj ) = isServerDefault;
static 	int	(*isClient)( XObj ) = isClientDefault;
static 	int	defaultRunTest( XObj, int, int );
static  void	testInit( void );
static 	void	realmClientFuncs( XObj );
static 	void	realmServerFuncs( XObj );


#else

static	void 	client();
static 	void 	server();
static 	int	isServerDefault();
static 	int	isClientDefault();
static 	int	(*isServer)() = isServerDefault;
static 	int	(*isClient)() = isClientDefault;
/* static 	int	asyncRunTest(); */
static 	int	defaultRunTest();
static  void	testInit();
static 	void	realmClientFuncs();
static 	void	realmServerFuncs();

#endif __STDC__



#ifndef STR2HOST
#  define STR2HOST str2ipHost
#endif


static	HOST_TYPE 	myHost;


/* 
 * Parameters reflecting defaults and command line parameters.  There
 * are OK to be static as they shouldn't change across multiple
 * instantiations. 
 */
static	int	serverParam, clientParam, testsSpecified, myTestSpecified;
static	char	*serverString;
static	int	trips = TRIPS;
static  int	simul = 1;  /* number of simultaneous messages circulating */
/* if simul>1, the trips are randomly divided among the messages */


#ifdef RPCTEST

#  ifdef __STDC__
static xkern_return_t	serverCallDemuxDefault( XObj, XObj, Msg *, Msg * );
static int		tryCallDefault( XObj, int, int );
#  else
static xkern_return_t	serverCallDemuxDefault();
static int		tryCallDefault();
#  endif __STDC__

static xkern_return_t	(* serverCallDemux)() = serverCallDemuxDefault;
static int		(* tryCall)() = tryCallDefault;

#else

#  ifdef __STDC__
static xkern_return_t	defaultServerDemux( XObj, XObj, Msg * );
#  else
static xkern_return_t	defaultServerDemux();
#  endif /* __STDC__ */

static Pfk	serverDemux = defaultServerDemux;

#endif RPCTEST

#ifdef TIME
#  ifndef RPCTEST
static	XTime 	starttime;
#  endif
static 	void	subtime(
#ifdef __STDC__
			XTime *t1, XTime *t2, XTime *result
#endif
			);
#endif

#define FAILURE_LIMIT 2



static Pfi	runTest = defaultRunTest;


#ifdef XKMACHKERNEL
static int
sscanf1(str, format, a1)
char *str, format;
int *a1;
{
  int n;

  *a1=0;
  while (*str >= '0' && *str <= '9')
    *a1 = 10*(*a1) + (*str++ - '0');
  return(1);
}
#else
#define sscanf1 sscanf
#endif XKMACHKERNEL


#ifdef __STDC__
#define DOUBLEQUOTEWRAP(x) #x
#define STRINGIFY(z) DOUBLEQUOTEWRAP(z)
#endif

static void
print_compile_options()
{   printf("\nCompiled with options:\n");
    printf(
#ifdef __STDC__
    "HOST_TYPE "  STRINGIFY(HOST_TYPE)
    "; INIT_FUNC " STRINGIFY(INIT_FUNC)
    "; TRACEVAR "  STRINGIFY(TRACEVAR)  "; "
#endif __STDC__
    "PROT_STRING %s\n", PROT_STRING);
    printf("TRIPS = %d  TIMES = %d  DELAY = %d\n", TRIPS, TIMES, DELAY);
    printf("__STDC__ %s  PROFILE %s  TIME %s  XKMACHKERNEL %s\n",
#ifdef __STDC__
	   "on",
#else
	   "off",
#endif
#ifdef PROFILE
	   "on",
#else
	   "off",
#endif
#ifdef TIME
	   "on",
#else
	   "off",
#endif
#ifdef XKMACHKERNEL
	   "on"
#else
	   "off"
#endif
	   );
    printf("XK_MEMORY_THROTTLE %s  RPCTEST %s  STREAM_TEST %s\n",
#ifdef XK_MEMORY_THROTTLE
	   "on",
#else
	   "off",
#endif
#ifdef RPCTEST
	   "on",
#else
	   "off",
#endif
#ifdef STREAM_TEST
	   "on"
#else
	   "off"
#endif
	   );
    printf("FAILURE_LIMIT %d  CONCURRENCY ", FAILURE_LIMIT);
#ifdef CONCURRENCY
    printf("%d",CONCURRENCY);
#else
    printf("off");
#endif
    printf("  XK_INCOMING_MEMORY_MARK ");
#ifdef XK_INCOMING_MEMORY_MARK
    printf("%d", XK_INCOMING_MEMORY_MARK);
#else
    printf("undefined");
#endif
    printf("\n");
#if (defined(CUSTOM_ASSIGN) || defined(CUSTOM_OPENDONE) || \
     defined(CUSTOM_SERVER_DEMUX) || defined(CUSTOM_CLIENT_DEMUX))
    printf("with");
#ifdef CUSTOM_ASSIGN
	printf(" CUSTOM_ASSIGN");
#endif
#ifdef CUSTOM_OPENDONE
	printf(" CUSTOM_OPENDONE");
#endif
#ifdef CUSTOM_SERVER_DEMUX
	printf(" CUSTOM_SERVER_DEMUX");
#endif
#ifdef CUSTOM_CLIENT_DEMUX
	printf(" CUSTOM_CLIENT_DEMUX");
#endif
    printf("\n");
#endif
#if !(defined(CUSTOM_ASSIGN) && defined(CUSTOM_OPENDONE) && \
      defined(CUSTOM_SERVER_DEMUX) && defined(CUSTOM_CLIENT_DEMUX))
    printf("without");
#ifndef CUSTOM_ASSIGN
	printf(" CUSTOM_ASSIGN");
#endif
#ifndef CUSTOM_OPENDONE
	printf(" CUSTOM_OPENDONE");
#endif
#ifndef CUSTOM_SERVER_DEMUX
	printf(" CUSTOM_SERVER_DEMUX");
#endif
#ifndef CUSTOM_CLIENT_DEMUX
	printf(" CUSTOM_CLIENT_DEMUX");
#endif
    printf("\n");
#endif
    printf("SAVE_SERVER_SESSN %s  CLIENT_OPENABLE %s\n",
#ifdef SAVE_SERVER_SESSN
	   "on",
#else
	   "off",
#endif
#ifdef CLIENT_OPENABLE
	   "on"
#else
	   "off"
#endif
	   );
    printf("USE_CHECKSUM %s  INF_LOOP %s  XK_TEST_ABORT %s\n",
#ifdef USE_CHECKSUM
	   "on",
#else
	   "off",
#endif
#ifdef INF_LOOP
	   "on",
#else
	   "off",
#endif
#ifdef XK_TEST_ABORT
	   "on"
#else
	   "off"
#endif
	   );
  }



static void
processOptions()
{
    int		i;
    char 	*arg;

#define argPrefix(str) ((! strncmp(arg, str, strlen(str))) && strlen(arg)>strlen(str))
#define argEq(str) (! strcmp(arg, str) )

    for (i=1; i < globalArgc; i++) {
	arg = globalArgv[i];
	if ( argEq("-s") ) {
	    serverParam = 1;
	} else if ( argPrefix("-c") ) {
	    clientParam = 1;
	    serverString = arg + 2;
	} else if ( argEq("-c")) {
	  clientParam = 1;
	  serverString = globalArgv[i+1];
	  i++;
	} else if ( argPrefix("-test")) {
	    testsSpecified = 1;
	    arg += strlen("-test");
	    if ( argEq(PROT_STRING) ) {
		myTestSpecified = 1;
	    }
	} else if ( argPrefix("-trips=") ) {
	    sscanf1(arg + strlen("-trips="), "%d", &trips);
	} else if ( argEq ("-trips")) {
	    sscanf1(globalArgv[i+1], "%d", &trips);
	    i++;
	} else if ( argPrefix("-simul=")) {
	    sscanf1(arg + strlen("-simul="), "%d", &simul);
	    if (simul>100) {printf("simul clipped to 100\n"); simul=100;};
	    if (simul<1) {printf("simul increased to 1\n"); simul=1;};
	} else if ( argEq("-simul=")) {
	    sscanf1(globalArgv[i+1], "%d", &simul);
	    if (simul>100) {printf("simul clipped to 100\n"); simul=100;};
	    if (simul<1) {printf("simul increased to 1\n"); simul=1;};
	    i++;
	}
    }
    if (serverString && (serverString[0] > '9' || serverString[0] < '0')) {
        IPhost haddr;

	if( xk_gethostbyname(serverString, &haddr) == XK_SUCCESS ) {
	  xTrace5(prottest, TR_DETAILED,
		  "server: %s addr %d.%d.%d.%d\n", serverString,
		  haddr.a, haddr.b, haddr.c, haddr.d);
	  strcpy(serverString, ipHostStr(&haddr));
	  xTrace1(prottest, TR_DETAILED, "server: %s addr\n", serverString);
	}
	else {
	  xTrace1(prottest, TR_SOFT_ERRORS, "Cannot resolve name %s as host address; check dns entries in rom file",
		  serverString);
	}
      }

    if (trips%simul) {
      trips = simul * (1 + trips/simul);
      printf("Rounded trips up to %d, a multiple of simul(%d).\n",
	     trips, simul);
    };
    printf("Running with trips=%d, simul=%d.\n",trips,simul);
    if (simul>1)
     printf(
      "Be sure that server has a simul value <= %d, and is a divisor of %d.\n",
	    simul, trips);
#undef argPrefix
#undef argEq    
}


int
INIT_FUNC( self )
    XObj self;
{
    XObj	llp;
    PState	*ps;

    xIfTrace(prottest, TR_ERRORS) { print_compile_options(); };
    processOptions();
    if ( testsSpecified && ! myTestSpecified ) {
	xTrace1(prottest, TR_SOFT_ERRORS, "Parameters indicate %s test should not run",
		PROT_STRING);
	return 0;
    }
    printf("%s timing test\n", PROT_STRING);
    llp = xGetDown(self, 0);
    if ( ! xIsProtocol(llp) ) {
	Kabort("Test protocol has no lower protocol");
    }
    xControl(xGetDown(self, 0), GETMYHOST, (char *)&myHost, sizeof(HOST_TYPE));
    ps = X_NEW(PState);
    bzero((char *)ps, sizeof(PState));
    self->state = (VOID *)ps;
    /* 
     * Call the per-test initialization function which gives the test
     * the opportunity to override the default functions
     */
    testInit();
    if ((*isServer)(self)) {
	evDetach( evSchedule(server, self, 0) );
    } else if ( (*isClient)(self)) {
#ifdef CONCURRENCY	
	int	i;

	for ( i=0; i < CONCURRENCY; i++ )
#endif
	    evDetach( evSchedule(client, self, 0) );
    } else {
	printf("%stest: I am neither server nor client\n", PROT_STRING);
    }
    return 0;
}



static int
isServerDefault( self )
    XObj	self;
{
    if ( serverParam ) {
	return TRUE;
    }
    if ( ! strcmp(self->instName, "server") ) {
	return TRUE;
    }
    return ! bcmp((char *)&myHost, (char *)&ServerAddr, sizeof(HOST_TYPE));
}


static int
isClientDefault( self )
    XObj	self;
{
    if ( ! strcmp(self->instName, "client") ) {
	return TRUE;
    }
    if ( clientParam ) {
	STR2HOST(&ServerAddr, serverString);
	ClientAddr = myHost;
	return TRUE;
    }
    return ! bcmp((char *)&myHost, (char *)&ClientAddr, sizeof(HOST_TYPE));
}


#ifndef CUSTOM_ASSIGN

static void
clientSetPart( p )
    Part *p;
{
    partInit(p, 1);
    partPush(*p, &ServerAddr, sizeof(IPhost));
}

static void
serverSetPart( p )
    Part *p;
{
    partInit(p, 1);
    partPush(*p, ANY_HOST, 0);
}

#endif ! CUSTOM_ASSIGN


#ifdef SAVE_SERVER_SESSN
#  ifdef __STDC__
static xkern_return_t	saveServerSessn( XObj, XObj, XObj, XObj );
#  endif

static xkern_return_t
saveServerSessn( self, s, llp, hlpType )
    XObj self, s, llp, hlpType;
{
    xTrace1(prottest, TR_MAJOR_EVENTS,
	    "%s test program duplicates lower server session",
	    PROT_STRING);
    xDuplicate(s);
    return XK_SUCCESS;
}
#endif SAVE_SERVER_SESSN


static xkern_return_t
closeDone( lls )
    XObj	lls;
{
    xTrace2(prottest, TR_MAJOR_EVENTS, "%s test -- closedone (%x) called",
	    PROT_STRING, lls);
#ifdef SAVE_SERVER_SESSION
    if ( (*isServer)() ) {
	xClose(lls);
    }
#endif
    return XK_SUCCESS;
}


static void
server( ev, foo )
    Event	ev;
    VOID 	*foo;
{
    Part p;
    XObj myProtl = (XObj)foo;
    
    printf("I am the  server\n");
    xAssert(xIsProtocol(myProtl));
    realmServerFuncs(myProtl);
#ifdef SAVE_SERVER_SESSN
    myProtl->opendone = saveServerSessn;
#endif 
#ifdef CUSTOM_OPENDONE
    myProtl->opendone = customOpenDone;
#endif 
    myProtl->closedone = closeDone;
    serverSetPart(&p);
    if ( xOpenEnable(myProtl, myProtl, xGetDown(myProtl, 0), &p)
		== XK_FAILURE ) {
	printf("%s test server can't openenable lower protocol\n",
	       PROT_STRING);
    } else {
	printf("%s test server done with xopenenable\n", PROT_STRING);
    }
    return;
}


static void
client( ev, foo )
    Event	ev;
    VOID 	*foo;
{
    Part	p[2];
    int 	lenindex;
    XObj	myProtl = (XObj)foo;
    PState	*ps;
    int		testNumber = 0;
    
    xAssert(xIsXObj(myProtl));
    ps = (PState *)myProtl->state;
    printf("I am the client\n");
    realmClientFuncs(myProtl);

#ifdef CLIENT_OPENENABLE
    serverSetPart(p);
    if ( xOpenEnable(myProtl, myProtl, xGetDown(myProtl, 0), p)
		== XK_FAILURE ) {
	printf("%s test client can't openenable lower protocol\n",
	       PROT_STRING);
	return;
    } else {
	printf("%s test client done with xopenenable\n", PROT_STRING);
    }
#endif
    clientSetPart(p);
    if ( ps->lls == 0 ) {
    	ps->lls = xOpen(myProtl, myProtl, xGetDown(myProtl, 0), p);
	if ( ps->lls == ERR_XOBJ ) {
	    printf("%s test: open failed!\n", PROT_STRING);
	    Kabort("End of test");
	    return;
	}
    }
    /* 
     * Delay to allow incoming messages to finalize opens (if necessary)
     */
    Delay(1000);
#ifdef CLIENT_OPENDONE
    myProtl->opendone(myProtl, ps->lls, xGetDown(myProtl, 0), myProtl);
#endif    
#ifdef USE_CONTROL_SESSN
    clientSetPart(p);
    if ( xIsProtocol(xGetDown(myProtl, 1)) ) {
	controlSessn = xOpen(myProtl, myProtl, xGetDown(myProtl, 1), p);
	if ( ! xIsXObj(controlSessn) ) {
	    xError("test protl could not open control sessn");
	}
    } else {
	xTrace1(prottest, TR_EVENTS,
		"%s test client not opening control session",
		PROT_STRING);
    }
#endif USE_CONTROL_SESSN

#ifdef USE_CHECKSUM
    xControl(ps->lls, UDP_ENABLE_CHECKSUM, NULL, 0);
#endif
#ifdef INF_LOOP
    for (lenindex = 0; ; lenindex++) {
	if (lenindex >= sizeof(lens)/sizeof(long)) {
	    lenindex = 0;
	}
#else
    for (lenindex = 0; lenindex < sizeof(lens)/sizeof(long); lenindex++) {
#endif INF_LOOP	
	ps->sentMsgLength = lens[lenindex];
	if ( runTest(myProtl, lens[lenindex], ++testNumber) ) {
	    break;
	}
    }
    printf("End of test\n");
    xClose(ps->lls);
    ps->lls = 0;
#ifdef XK_TEST_ABORT
    Kabort("test client aborts at end of test");
#endif
    xTrace0(prottest, TR_FULL_TRACE, "test client returns");
}


#ifdef TIME
static void
subtime(t1, t2, t3)
    XTime *t1, *t2, *t3;
{
    t3->sec = t2->sec - t1->sec;
    t3->usec = t2->usec - t1->usec;
    if (t3->usec < 0) {
	t3->usec += 1000000;
	t3->sec -= 1;
    }
}
#endif


#ifdef RPCTEST

#  include "common_test_rpc.c"

#else

#  include "common_test_async.c"

#endif
