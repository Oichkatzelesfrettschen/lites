/*
 * $RCSfile: common_test_async.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.4 $
 * $Date: 1993/06/17 21:50:22 $
 */


#ifdef __STDC__

#  ifdef STREAM_TEST

static xkern_return_t	clientStreamDemux( XObj, XObj, Msg * );

#  else /* ! STREAM_TEST */

static xkern_return_t	clientDemux( XObj, XObj, Msg * );

#  endif /* STREAM_TEST */

#  else /* __STDC__ */

#  ifdef STREAM_TEST

static xkern_return_t	clientStreamDemux( );

#  else /* ! STREAM_TEST */

static xkern_return_t	clientDemux( );

#  endif /* STREAM_TEST */

#endif /* __STDC__ */


static void
realmServerFuncs( self )
    XObj	self;
{
    self->demux = serverDemux;
}


static void
realmClientFuncs( self )
    XObj	self;
{
#ifdef STREAM_TEST
    self->demux = clientStreamDemux;
#else
    self->demux = clientDemux;
#endif
}


static void
checkHandle( h, str )
    xmsg_handle_t	h;
    char		*str;
{
    switch ( h ) {
      case XMSG_ERR_HANDLE:
      case XMSG_ERR_WOULDBLOCK:
	sprintf(errBuf, "%s returns error handle %d", str, h);
	Kabort(errBuf);
      default:
	;
    }
}



static int
defaultRunTest( self, len, testNumber )
    XObj	self;
    int 	len;
    int		testNumber;
{
    static Msg	msga[100];
    int 	test, m;
    char	*buf;
    static int	noResponseCount = 0;
    PState	*ps = (PState *)self->state;
    
    xAssert(ps);
    msgConstructAllocate(&ps->savedMsg, len, &buf);

#ifdef CHECK_MESSAGE_CONTENTS
    /* Hack to initialize the message contents. */
    printf("Checking message contents.\n");
    for (m=0;m<len;m++) *(buf+m) = 3*m+len;
#ifdef PRINT_MESSAGE_CONTENTS
    printf("msg "); for (m=0;m<len && m<40;m++) printf("%02x",0xff & *(buf+m)); printf("\n");
    if (len>=40) { printf("msg ");
      for (m=(len<80)?40:(len-40);m<len;m++) printf("%02x",0xff & *(buf+m)); printf("\n"); }
#endif
#endif

    for (test = 0; test < TIMES; test++) {
	for (m=0; m<simul; m++) {msgConstructCopy(&msga[m], &ps->savedMsg);};
	printf("Sending (%d) ...\n", testNumber);
	printf("msg length: %d\n", msgLen(&msga[0]));
	ps->clientRcvd = 0;
#ifdef PROFILE
	startProfile();
#endif
#ifdef TIME
	xGetTime(&starttime);
#endif
#ifdef XK_MEMORY_THROTTLE
	while ( memory_unused < XK_INCOMING_MEMORY_MARK)
	  Delay(DELAY * 1000);
#endif XK_MEMORY_THROTTLE
	for (m=0; m<simul; m++) {
	    ps->clientPushResult = xPush(ps->lls, &msga[m]);
#ifdef TCP_PUSH
	    xControl(ps->lls, TCP_PUSH, NULL, 0);
#endif
	    checkHandle(ps->clientPushResult, "client push");
	}
	do {
	    ps->idle = 1;
	    Delay(DELAY * 1000);
	} while ( ! ps->idle );
	if ( ps->clientRcvd < trips ) {
	    printf("Test failed after receiving %d packets\n", ps->clientRcvd);
#ifdef STREAM_TEST
	    ps->receivedLength = 0;
#endif
	} 
	if ( ps->clientRcvd == 0 ) {
	    if ( noResponseCount++ == FAILURE_LIMIT ) {
		printf("Server looks dead.  I'm outta here.\n");
		return 1;
	    }
	} else {
	    noResponseCount = 0;
	}
	for (m=0; m<simul; m++) { msgDestroy(&msga[m]); };
    }
    msgDestroy(&ps->savedMsg); 
    return 0;
}

#ifdef CHECK_MESSAGE_CONTENTS
static long revbcopy0(to,from,len,ignore)
    char *to, *from, *ignore; long len;
{   bcopy(from,to,len); ignore++; return 0; }
#endif


static xkern_return_t
defaultServerDemux( self, lls, dg )
    XObj self, lls;
    Msg *dg;
{
    PState	*ps = (PState *)self->state;
    static int c = 0;
    static Msg msga[100]; static int msgi = 0; int i;

    xIfTrace(prottest, TR_MAJOR_EVENTS) {
	putchar('.');
	if (! (c++ % 50)) {
	    putchar('\n');
	}
    }

#ifdef CHECK_MESSAGE_CONTENTS
    { int len=msgLen(dg), k; char* tbuf=xMalloc(len);
      msgPop(dg,revbcopy0,tbuf,len,0);
      for (k=0;k<len;k++)
	if ( *(tbuf+k) != (char)(3*k+len))
	{ printf("Message compare failed at byte %d, is %x, sb %x.  msg# %d len %d.\n",
		 k, *(tbuf+k), (char)(3*k+len), c, len); break; }
      if (k==len) printf("Message %d len %d compared ok.\n", c, len);
#ifdef PRINT_MESSAGE_CONTENTS
      printf("msg "); for (k=0;k<len && k<40;k++) printf("%02x",0xff & *(tbuf+k)); printf("\n");
      if (len>=40) { printf("msg ");
        for (k=(len<80)?40:(len-40);k<len;k++) printf("%02x",0xff & *(tbuf+k)); printf("\n");}
#endif
      xFree(tbuf); }
#endif

#ifdef CUSTOM_SERVER_DEMUX
    customServerDemux(self, lls, dg);
#endif CUSTOM_SERVER_DEMUX
#ifdef XK_MEMORY_THROTTLE
	while ( memory_unused < XK_INCOMING_MEMORY_MARK)
	  Delay(DELAY * 1000);
#endif XK_MEMORY_THROTTLE
    /* if simul>1, save up a group of messages, then return them */
    /* if things get out of sync, they get really screwed up! */
    if (simul>1) {
      msgConstructCopy(&msga[msgi],dg); msgi++;
      if (msgi==simul) {
	for (i=0; i<simul; i++) {
	    ps->serverPushResult = xPush(lls, &msga[i]);
#ifdef TCP_PUSH
	    xControl(lls, TCP_PUSH, NULL, 0);
#endif
	    checkHandle(ps->serverPushResult, "server push");
	    msgDestroy(&msga[i]);
	}
	msgi=0;
      };
    } else {
	ps->serverPushResult = xPush(lls, dg);
#ifdef TCP_PUSH
	xControl(lls, TCP_PUSH, NULL, 0);
#endif
	checkHandle(ps->serverPushResult, "server push");
    }
    return XK_SUCCESS;
}





#ifdef STREAM_TEST


static xkern_return_t
clientStreamDemux( self, lls, dg )
    XObj self, lls;
    Msg *dg;
{
#ifdef TIME
    XTime 	now, total;
#endif
    Msg			msgToPush;
    xmsg_handle_t	h;
    PState		*ps = (PState *)self->state;

    xAssert(ps);
    ps->idle = 0;
    xTrace1(prottest, TR_EVENTS, "R %d", msgLen(dg));
    ps->receivedLength += msgLen(dg);
    xTrace1(prottest, TR_DETAILED, "total length = %d", ps->receivedLength);
#ifdef CUSTOM_CLIENT_DEMUX
    customClientDemux(self, lls, dg);
#endif CUSTOM_CLIENT_DEMUX
    if (ps->receivedLength == ps->sentMsgLength) {
	/*
	 * Entire response has been received.
	 * Send another message
	 */
	if (++ps->clientRcvd < trips) {
	    xIfTrace(prottest, TR_MAJOR_EVENTS) { 
		putchar('.');
		if (! (ps->clientRcvd % 50)) {
		    putchar('\n');
		}
	    }
	  if (ps->clientRcvd+simul <= trips) {
	    msgConstructCopy(&msgToPush, &ps->savedMsg);
	    ps->receivedLength = 0;
	    xTrace0(prottest, TR_EVENTS, "S");
#ifdef XK_MEMORY_THROTTLE
	while ( memory_unused < XK_INCOMING_MEMORY_MARK)
	  Delay(DELAY * 1000);
#endif XK_MEMORY_THROTTLE
	    h = xPush(lls, &msgToPush);
#ifdef TCP_PUSH
	    xControl(lls, TCP_PUSH, NULL, 0);
#endif
	    checkHandle(h, "client push");
	    msgDestroy(&msgToPush);
	  } else { ps->receivedLength = 0; };
	} else {
#ifdef TIME
	    xGetTime(&now);
	    subtime(&starttime, &now, &total);
	    printf("\nlen = %4d, %d trips: %6d.%06d\n", 
		   ps->receivedLength, trips, total.sec, total.usec);
#else
	    printf("\nlen = %4d, %d trips\n", ps->receivedLength, trips);
#endif
	    ps->receivedLength = 0;
#ifdef PROFILE
	    endProfile();
#endif
	}
    }
    return XK_SUCCESS;
}


#else /* ! STREAM_TEST */



static xkern_return_t
clientDemux( self, lls, dg )
    XObj self, lls;
    Msg *dg;
{
    PState	*ps = (PState *)self->state;
#ifdef TIME
    XTime now, total;
#endif
    static Msg msga[100]; static int msgi = 0; int i;

#ifdef CHECK_MESSAGE_CONTENTS
    { int len=msgLen(dg), k; char* tbuf=xMalloc(len);
      msgPop(dg,revbcopy0,tbuf,len,0);
      for (k=0;k<len;k++)
	if ( *(tbuf+k) != (char)(3*k+len))
	{ printf("Message compare failed at byte %d, is %x, sb %x.  msg# %d len %d.\n",
		 k, *(tbuf+k), (char)(3*k+len), ps->clientRcvd, len); break; }
      if (k==len) printf("Message %d len %d compared ok.\n", ps->clientRcvd, len);
#ifdef PRINT_MESSAGE_CONTENTS
      printf("msg "); for (k=0;k<len && k<40;k++) printf("%02x",0xff & *(tbuf+k)); printf("\n");
      if (len>=40) { printf("msg ");
        for (k=(len<80)?40:(len-40);k<len;k++) printf("%02x",0xff & *(tbuf+k)); printf("\n");}
#endif
      xFree(tbuf); }
#endif

/* note that customdemux is not called on final response message */
#ifdef CUSTOM_CLIENT_DEMUX
    customClientDemux(self, lls, dg);
#endif CUSTOM_CLIENT_DEMUX
    ps->idle = 0;
    if ( ++ps->clientRcvd < trips ) {
	xIfTrace(prottest, TR_MAJOR_EVENTS) {
	    putchar('.');
	    if (! (ps->clientRcvd % 50)) {
		putchar('\n');
	    }
	}
#ifdef XK_MEMORY_THROTTLE
	while ( memory_unused < XK_INCOMING_MEMORY_MARK)
	  Delay(DELAY * 1000);
#endif XK_MEMORY_THROTTLE
	if (simul>1) {
	  msgConstructCopy(&msga[msgi],&ps->savedMsg); msgi++;
	  if (msgi==simul) {
	    for (i=0; i<simul; i++) {
		ps->clientPushResult = xPush(lls, &msga[i]);
#ifdef TCP_PUSH
		xControl(lls, TCP_PUSH, NULL, 0);
#endif
		checkHandle(ps->clientPushResult, "client push");
		msgDestroy(&msga[i]);
	    }
	    msgi=0;
	  };
	}
	else {
	    /* 
	     * We need to construct a copy of the original message
	     * rather than just loop back the incoming message to
	     * avoid an increasingly fragmented message structure in
	     * the case of loopback.
	     */
	    Msg	tmpMsg;

	    msgConstructCopy(&tmpMsg, &ps->savedMsg);
	    ps->clientPushResult = xPush(lls, &tmpMsg);
	    msgDestroy(&tmpMsg);
#ifdef TCP_PUSH
	  xControl(lls, TCP_PUSH, NULL, 0);
#endif
	  checkHandle(ps->clientPushResult, "client push");
      }
    } else {
        /* should clear out msga when simul>1, but skip it for now */
        msgi=0;
#ifdef TIME
	xGetTime(&now);
	subtime(&starttime, &now, &total);
	printf("\nlen = %4d, %d trips: %6d.%06d\n", 
		msgLen(dg), trips, total.sec, total.usec);
#else
	printf("\nlen = %4d, %d trips\n", msgLen(dg), trips);
#endif
	
#ifdef PROFILE
	endProfile();
#endif
    }
    return XK_SUCCESS;
}


#endif /* STREAM_TEST */
