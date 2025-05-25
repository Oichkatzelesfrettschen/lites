/*     
 * $RCSfile: common_test_rpc.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.9 $
 * $Date: 1993/04/12 23:25:04 $
 */


#ifdef __STDC__
static 	xkern_return_t	testCallDemux( XObj, XObj, Msg *, Msg * );
#else
static 	xkern_return_t	testCallDemux();
#endif __STDC__



static int
tryCallDefault( sessn, times, length )
  XObj sessn;
  int times;
  int length;
{
    xkern_return_t ret_val;
    int i;
    Msg	savedMsg, request, reply;
    char *buf;
    int c = 0;
    
    msgConstructAllocate(&savedMsg, length, &buf);
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


/* 
 * RPC runTest
 */
static int
defaultRunTest( self, len, testNumber )
    XObj	self;
    int 	len;
    int		testNumber;
{
    int 	test;
    static int	noResponseCount = 0;
    XObj	lls;
#ifdef TIME    
    XTime 	startTime, endTime, netTime;
#endif
    int		count;

    xAssert(xIsXObj(self));
    lls = ((PState *)self->state)->lls;
    xAssert(xIsSession(lls));
    for (test = 0; test < TIMES; test++) {
	printf("Sending (%d) ...\n", testNumber);
	count = 0;
#ifdef PROFILE
	startProfile();
#endif
#ifdef TIME
	xGetTime(&startTime);
#endif
	if ( (count = tryCall(lls, trips, len)) == trips ) {
#ifdef TIME
	    xGetTime(&endTime);
	    subtime(&startTime, &endTime, &netTime);
	    printf("\nlen = %4d, %d trips: %6d.%06d\n", 
		   len, trips, netTime.sec, netTime.usec);
#else
	    printf("\nlen = %4d, %d trips\n", len, trips);
#endif
	}
#ifdef USE_CONTROL_SESSN
	if ( xIsXObj(controlSessn) ) {
	    Msg		m;
	    char	*strbuf;
	    
	    msgConstructAllocate(&m, 80, &strbuf);
	    strbuf[0] = '\n';
	    sprintf(strbuf+1, "End of test %d", testNumber);
	    
	    xCall(controlSessn, &m, 0);
	    msgDestroy(&m);
	    
	}
#endif USE_CONTROL_SESSN
	if ( count == 0 ) {
	    if ( noResponseCount++ == FAILURE_LIMIT ) {
		printf("Server looks dead.  I'm outta here.\n");
		return 1;
	    }
	} else {
	    noResponseCount = 0;
	}
	Delay(DELAY * 1000);
#ifdef XK_MEMORY_THROTTLE
	{
	  extern int max_thread_pool_used, min_memory_avail,
	             max_xk_threads_inuse;
	  printf("used %d incoming threads; %d regular threads; max %d bytes memory",
		 max_thread_pool_used,
		 max_xk_threads_inuse,
		 min_memory_avail);
	}
#endif XK_MEMORY_THROTTLE
    }
    return 0;
}


static xkern_return_t
serverCallDemuxDefault(self, lls, dg, rMsg)
    XObj self, lls;
    Msg *dg, *rMsg;
{
    msgAssign(rMsg, dg);
    return XK_SUCCESS;
}


static xkern_return_t
testCallDemux( self, lls, dg, rMsg )
    XObj self, lls;
    Msg *dg, *rMsg;
{
    static int c = 0;
    
    xIfTrace(prottest, TR_MAJOR_EVENTS) {
	putchar('.');
	if (! (++c % 50)) {
	    putchar('\n');
	}
    }
    return serverCallDemux(self, lls, dg, rMsg);
}


static void
realmServerFuncs( self )
    XObj	self;
{
    self->calldemux = testCallDemux;
}


static void
realmClientFuncs( self )
    XObj	self;
{
}
