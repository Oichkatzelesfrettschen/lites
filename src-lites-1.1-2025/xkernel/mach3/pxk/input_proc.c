/*
 * $RCSfile: input_proc.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.11 $
 * $Date: 1993/05/13 16:08:14 $
 */

/*
 * Mach3
 *
 *  This file is based on process.c as of Sun Jul  5 15:20:14 MST 1992
 *
 */

#include <cthreads.h>
#include <mach.h>

#include <mach/message.h>
/* xkernel.h includes xk_debug.h */
#include "xkernel.h"
#include "assert.h"
/* platform includes process.h */
#include "platform.h"
#include "eth.h"
#include "list.h"
#include "process_msg.h"
#include "event_i.h"
#include "romopt.h"
#include "config.h"


#ifdef __STDC__

static xkern_return_t	readBufsRom( char **, int, int, VOID * );
static xkern_return_t	readThreadsRom( char **, int, int, VOID * );

#else

static xkern_return_t	readBufsRom();
static xkern_return_t	readThreadsRom();

#endif


#define XK_THREAD_STATS	XK_DEBUG

#ifdef XK_THREAD_STATS
static int 		activeThreads;   
static int		activeHighWater;  /* keeps track of max concurrent threads used */
#endif
ProtectedQueue	xkFreeQueue;
ProtectedQueue	xkInQueue;


#define BUFFER_ALLOC_SIZE	(ROUND4(MAX_ETH_DATA_SZ + ROUND4(sizeof(struct mach_hdrs))))

static InputBuffer	*xkBuffers;
/* 
 * These are defaults and may be overridden by rom entries.
 * See config.h
 */
static int		numThreads = XK_SHEPHERD_THREADS;	
static int		numBuffers = XK_SHEPHERD_BUFFERS;


static RomOpt	threadOpts[] = {
    { "threads", 3, readThreadsRom },
    { "buffers", 3, readBufsRom }
};


static void
protectedQueueInit( pq )
    ProtectedQueue	*pq;
{
    pq->lock = mutex_alloc();
    pq->notEmpty = condition_alloc();
    pq->list = (list_t)xMalloc(sizeof(struct list_head));
    list_init(pq->list);
}


/*
 * xkBufferPoolInit()
 *
 *    Initial input handling structures
 *
 */
void
xkBufferPoolInit()
{
    int i;
    
    /*
     * create a pool of buffers for incoming data
     */
    xTrace1(processcreation, TR_GROSS_EVENTS,
	    "initializing %d buffers for incoming packets", numBuffers);
    xkBuffers = (InputBuffer *)xMalloc(sizeof(InputBuffer) * numBuffers);
    for ( i = 0; i < numBuffers; i++ ) {
	msgConstructAllocate (&xkBuffers[i].upmsg, BUFFER_ALLOC_SIZE, &xkBuffers[i].data);
	xkBuffers[i].q.next = 0;
	enlist( xkFreeQueue.list, &xkBuffers[i].q);
    }
}


/*
 * Entry point for a pool thread
 *
 * A pool thread is activated by the driver signalling on the input queue's
 * condition variable; it processes packets from the queue until the
 * input queue is empty.
 *
 */
static void
xkInputPool( evSelf, arg )
    Event	evSelf;
    VOID	*arg;
{
    InputBuffer	*buffer;
    Msg         *msg;
    int		threadId;
    
    threadId = (int)arg;
    /* 
     * As an event we were started under the master lock.
     */
#ifdef XK_THREAD_TRACE
    evMarkBlocked(0);
#endif
    xk_master_unlock();
    while( TRUE ) {
	/* 
	 * Pull a buffer off the input queue
	 */
	pqRemove( xkInQueue, buffer );
#ifdef XK_THREAD_STATS
	if ( ++activeThreads > activeHighWater ) {
	    activeHighWater = activeThreads;
	}
#endif
	xTrace1(processcreation, TR_EVENTS,
		"%d shepherd threads active", activeThreads);
	/* 
	 * Shepherd the message up
	 */
	msg = &buffer->upmsg;
	xTrace1(processcreation, TR_EVENTS,
		"input_process: thread %d acquires master lock", threadId);
	xk_master_lock();
#ifdef XK_THREAD_TRACE
	evMarkRunning();
#endif
	xTrace2(processcreation, TR_EVENTS,
		"input_process: thread %d demux with buffer %d",
		threadId, ((int)buffer - (int)xkBuffers)/sizeof(InputBuffer));
	xDemux(buffer->driverProtl, msg);
	/*
	 * refresh the buffer -- this must run within the lock
	 */
	msgConstructAllocate(msg, BUFFER_ALLOC_SIZE, &buffer->data);
#ifdef XK_THREAD_TRACE
	evMarkBlocked(0);
#endif
	xk_master_unlock();
	/* 
	 * Return the input buffer to the free queue
	 */
	pqAppend( xkFreeQueue, buffer );
#ifdef XK_THREAD_STATS
	activeThreads--;
#endif
    }
}


static xkern_return_t
readThreadsRom( str, nFields, line, arg )
    char	**str;
    int		nFields, line;
    VOID	*arg;
{
    return sscanf(str[2], "%d", &numThreads) >= 1 ?
      		XK_SUCCESS : XK_FAILURE;
}


static xkern_return_t
readBufsRom( str, nFields, line, arg )
    char	**str;
    int		nFields, line;
    VOID	*arg;
{
    return sscanf(str[2], "%d", &numBuffers) >= 1 ?
      		XK_SUCCESS : XK_FAILURE;
}



/*
 * xkFillThreadPool -- create initial threads 
 */
void
xkFillThreadPool()
{
    int	i;

    findRomOpts("shepherd", threadOpts,
		    sizeof(threadOpts)/sizeof(RomOpt), 0);
    /*
     * Initialize queues for synchronizing between the driver threads
     * and the shepherd threads.
     */
    protectedQueueInit( &xkInQueue );      
    protectedQueueInit( &xkFreeQueue );      

    xTrace1(processcreation, TR_GROSS_EVENTS,
	    "initializing %d threads for incoming packets", numThreads);
    for ( i=0; i < numThreads; i++ ) {
	evDetach(evSchedule(xkInputPool, (VOID *)i, 0));
    }
}


void
xkThreadDumpStats()
{
    xTrace0(processcreation, TR_ALWAYS, "xkernel input thread statistics:");
    xTrace3(processcreation, TR_ALWAYS,
	    "\tthreads: %d\tactive: %d\thigh-water: %d",
	    numThreads, activeThreads, activeHighWater);
}
