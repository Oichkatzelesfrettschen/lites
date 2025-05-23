/*
 * sartest.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1991  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/02/11 20:52:25 $
 */

/*
 * Ping-pong test of the SAR protocol
 */

#include "site.h"
#include "eth.h"
#include "sarProt.h"

#define HOST_TYPE ETHhost
#define INIT_FUNC sartest_init
#define TRACE_VAR sartestp
#define PROT_STRING "sar"

static HOST_TYPE ServerAddr = { 0, 0, 0};
static HOST_TYPE ClientAddr = { 0, 0, 0};

#define TRIPS 100000
#define TIMES 1
#define DELAY 3

/*
 * Define to do timing calculations
 */
#define TIME	 


static int lens[] = { 
  44, 1024, 2048, 4096, 8192, 16384
};


#define SAVE_SERVER_SESSN


#include "common_test.c"

static int
sarIsServer()
{
  return 0;
}

static int
sarIsClient()
{
  return 1;
}

void
  showStatistics()
{
  int stats[10];
  
  xControl(xGetDown(myProtl, 0), SAR_GETSTATISTICS, (char*)stats, 
	   sizeof(stats));

  printf("xmitIdles=%d, xmitFull=%d, xmitEmpty=%d\n", 
	 stats[0], stats[1], stats[2]);
  printf("rcvCRCErrors=%d, ", stats[3]);
  printf("rcvBQueueUnderflows=%d, ", stats[4]);
  printf("rcvQueueOverflows=%d, ", stats[5]);
  printf("rcvOutOfOrderCells=%d,\n", stats[6]);
  printf("rcvOutOfOrderFrags=%d, ", stats[7]);
  printf("rcvIdles=%d, ", stats[8]);
  printf("rcvFIFOFull=%d\n", stats[9]);
}


static int word;
static int rcvdMsgs;

static bool 
show(ptr, len, arg)
char *ptr;
int len;
char *arg;
{
  int i, isav, j, *iptr = (int*)ptr;
  int start, *startp;
  int error = 0;

  /*printf("addr=%x, len=%d\n", ptr, len);*/

#ifndef TIME
  for (i = 0; i < len / 4; i++, iptr++, word++) {  

    if ((word % 11) == 0) {
      /* new cell */
      if (error) printf("\n");
      error = 0;
      isav = i;
      start = word;
      startp = iptr;
    }

    if (!error && *iptr != (((word / 11) << 16) | (word % 11))) {
      /*
       * show the entire cell
       */
      i = isav;
      word = start;
      iptr = startp;

      printf("msg %d, cell %d: ", rcvdMsgs, word / 11);
      error = 1;
    }

    if (error) {
      if (*iptr == (((word / 11) << 16) | (word % 11)))
	printf(" ok ");
      else
	printf(" %d,%d ", (*iptr >> 16) & 0xffff, *iptr & 0xffff); 
    }

  }
#endif TIME

  return TRUE;
}

static int bufs;
static int currentLen;

static xkern_return_t
sarDemux( self, lls, dg )
    XObj self, lls;
    Msg *dg;
{
  int i, len = msgLen(dg);

  if (len == currentLen) 
    rcvdMsgs++;
  else
    printf("sarDemux, message with unexpected len=%d, should be %d\n", 
	   len, currentLen);
  
  word = 0;
  msgForEach(dg, show, NULL);
  
  return XK_SUCCESS;
}


static int
sarRunTest( len )
    int len;
{
    Msg		savedMsg, msg;
    int 	test, trips, i;
    int         total = 0;
    char	*buf;
    XTime       startTime, endTime, netTime;
    unsigned    offset;
    int *ptr;

    myProtl->demux = sarDemux;
    currentLen = len;
    rcvdMsgs = 0;

    msgConstructAllocate(&savedMsg, len + 0x1000, &buf);
    /* page-align the buffer */
    offset = ((unsigned)(buf + 0x1000) & 0xfffff000) - (unsigned)buf;
    msgPopDiscard(&savedMsg, offset);
    msgTruncate(&savedMsg, len);

#ifndef TIME    
    /* fill the buffer */
    ptr = (int*)(buf + offset);
    for (i = 0; i < len / 4; i++)
      *ptr++ = ((i / 11) << 16) | (i % 11);
#endif

    for (test = 0; test < TIMES; test++) {
	msgConstructCopy(&msg, &savedMsg);
	printf("Sending (%d) ...\n", ++total);
	/*msgForEach(&msg, show, NULL);*/
	printf("msg length: %d\n", msgLen(&msg));
	count = 0;

	xGetTime(&startTime);

	for (trips = 0; trips < TRIPS; trips++) {
	  clientPushResult = xPush(clientDownSes, &msg);
	  if (clientPushResult == ERR_XK_MSG) 
	    printf("sarRunTest: xPush faild, len=%d, test=%d, trip=%d\n",
		   len, test, trips);

	  bufs += (len + 4092) / 4092;
	  if  ((bufs & 0x7) == 0)
	    Delay(0);
	}

	xGetTime(&endTime);
	subtime(&startTime, &endTime, &netTime);
	printf("\nlen = %4d, %d trips: %6d.%06d\n", 
	       len, trips, netTime.sec, netTime.usec);
	msgDestroy(&msg);

	/* wait until all msgs are received */
	Delay(1000);
	printf("received %d messages\n", rcvdMsgs);
    }
    msgDestroy(&savedMsg); 
    showStatistics();
    return 0;
}



static void
testInit()
{
  isClient = sarIsClient;
  isServer = sarIsServer;
  runTest = sarRunTest;
  bufs = 0;
}
