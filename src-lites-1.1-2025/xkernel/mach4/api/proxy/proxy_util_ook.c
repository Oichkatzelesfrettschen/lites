/*     
 * $RCSfile: proxy_util_ook.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/07/16 00:28:13 $
 */

#include "xkernel.h"
#include "proxy_util.h"
#include "msg_internal.h"
#include "xk_mig_sizes.h"
#include <mach.h>
#include "xk_mach.h"


extern int	traceproxy;




/* 
 * Deallocate the virtual memory region containing 'p'.  
 */
void
oolFree( p, len )
    VOID	*p;
    int		len;
{
    kern_return_t	kr;

    xTrace2(proxy, TR_EVENTS, "proxy oobFree deallocates %d bytes starting at %x",
	    len, p);
    kr = vm_deallocate(mach_task_self(), (vm_address_t)p, len);
    if ( kr != KERN_SUCCESS ) {
	sprintf(errBuf,
		"vm_deallocate fails in  x-kernel proxy oobFree, error: %s",
		mach_error_string(kr));
	xError(errBuf);
	return;
    }
}


static void
call_xFree( p, len )
    VOID	*p;
    int		len;
{
    xTrace2(proxy, TR_FUNCTIONAL_TRACE, "calling xFree with region %x, length %d",
	    (u_int)p, len);
    xFree(p);
}


static void
call_msgDestroy( p, len )
    VOID	*p;
    int		len;
{
    xTrace2(proxy, TR_FUNCTIONAL_TRACE, "calling msgDestroy with msg %x (length %d)",
	    (u_int)p, len);
    msgDestroy((Msg *)p);
    xFree((VOID *)p);
}


#define msgTop(_m) ((_m)->stackHeadPtr)


/* 
 * Convert the data in the given message to an out-of-line region
 * suitable for passing directly as a MIG out-of-line parameter.
 */
xkern_return_t
msgToOol( m, oolPtr, func, arg )
    Msg		*m;
    char	**oolPtr;
    DeallocFunc	*func;
    VOID	**arg;
{
    /* 
     * In the case where we need to copy, it's unclear how best to
     * allocate the transmission buffer.  Either vm_allocate or
     * xMalloc will work.  vm_allocate is probably more expensive, but
     * is less likely to result in another copy due to a copy-on-write
     * fault. 
     */

    /* 
     * Avoid copying the data if it's contiguous
     */
    if ( msgIsContiguous(m) ) {
	Msg	*mCopy;

	xTrace2(proxy, TR_FUNCTIONAL_TRACE,
		"msgToOol: message is contiguous, avoiding copy",
		msgLen(m), (u_int)*oolPtr );
	mCopy = X_NEW(Msg);
	msgConstructCopy(mCopy, m);
	*arg = (VOID *)mCopy;
	*func = call_msgDestroy;
	*oolPtr = msgTop(m);
    } else {
	*oolPtr = xMalloc(msgLen(m));
	xAssert(*oolPtr);
	xTrace2(proxy, TR_FUNCTIONAL_TRACE,
		"msgToOol allocating %d bytes at %x",
		msgLen(m), (u_int)*oolPtr );
	msg2buf(m, *oolPtr);
	*func = call_xFree;
	*arg = *oolPtr;
    }
    return XK_SUCCESS;
}


/* 
 * Construct an x-kernel message from an incoming region of
 * out-of-line memory.  
 */
void
oolToMsg( oolPtr, len, msg )
    char	*oolPtr;
    int		len;
    Msg		*msg;
{
    xTrace1(proxy, TR_DETAILED, "xCall VM region starts at %x", (u_int)oolPtr);
    msgConstructInplace(msg, oolPtr, len, oolFree);
}


void
lingeringMsgSave( f, arg, len )
    DeallocFunc		f;
    VOID		*arg;
    int			len;
{
    LingeringMsg	*lMsg;

    lMsg = (LingeringMsg *)cthread_data(cthread_self());
    xAssert(lMsg);
    if ( lMsg->valid ) {
	lingeringMsgClear();
    }
    if ( f ) {
	lMsg->valid = TRUE;
	lMsg->func = f;
	lMsg->arg = arg;
	lMsg->len = len;
    }
}


void
lingeringMsgClear()
{
    LingeringMsg	*lMsg;

    lMsg = (LingeringMsg *)cthread_data(cthread_self());
    xAssert(lMsg);
    if ( lMsg->valid ) {
	xTrace0(proxy, TR_DETAILED, "clearing lingering message");
	if ( lMsg->func ) {
	    lMsg->func(lMsg->arg, lMsg->len);
	}
	lMsg->valid = FALSE;
    } else {
	xTrace0(proxy, TR_DETAILED, "no lingering message to clear");
    }
}


