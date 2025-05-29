/*     
 * $RCSfile: proxy_util.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.4 $
 * $Date: 1993/07/16 00:45:14 $
 */

#include "xkernel.h"
#include "proxy_util.h"
#include "msg_internal.h"
#include "xk_mig_sizes.h"


int	traceproxy = 0;


#define	isContiguous(_m) ( (_m)->state.numNodes == 1 && (_m)->stack == (_m)->tree )

bool
msgIsContiguous( m )
    Msg	*m;
{
    return isContiguous(m);
}


bool
msgIsOkToMangle( m, machMsg, offset )
    Msg		*m;
    char	**machMsg;
    int		offset;
{
#ifdef XK_PROXY_MSG_HACK
    /* 
     * Is the message contiguous
     */
    if ( ! isContiguous(m) ) {
	xTrace0(proxy, TR_DETAILED, "message is non-contiguous, must be copied");
	return FALSE;
    }
    /* 
     * Does this message have the only reference to the stack
     */
    if ( m->stack->refCnt != 1 ) {
	xTrace0(proxy, TR_DETAILED, "msg stack is shared, must be copied");
	return FALSE;
    }
    /* 
     * Is there enough space at the front of the stack to write a mach message header? 
     */
    if ( m->stackHeadPtr <= m->stackTailPtr &&
	 m->stackHeadPtr - m->stack->b.leaf.data >= offset ) {
	*machMsg = m->stackHeadPtr - offset;
    } else {
	xTrace2(proxy, TR_DETAILED, "front of stack is too short (%d < %d)",
		m->stackHeadPtr - m->stack->b.leaf.data, offset);
	xTrace0(proxy, TR_DETAILED, "Msg must be copied");
	return FALSE;
    }
    /* 
     * Is the stack going to be large enough?
     */
    if ( m->stack->b.leaf.data + m->stack->b.leaf.size - *machMsg < XK_MAX_MIG_MSG_LEN ) {
	xTrace1(proxy, TR_DETAILED,
		"Usable stack size (%d) is too small, msg must be copied",
		m->stack->b.leaf.data + m->stack->b.leaf.size - *machMsg);
	return FALSE;
    }
    /* 
     * Is the mach message going to be aligned properly?
     */
    if ( ! LONG_ALIGNED(*machMsg) ) {
	xTrace0(proxy, TR_DETAILED, "Msg has bad alignment, must be copied");
	return FALSE;
    }
    xTrace0(proxy, TR_DETAILED, "Can build mach msg around xkernel msg");
    xTrace2(proxy, TR_DETAILED, "mach msg start: %x, xk msg start: %x",
	    (int)*machMsg, (int)m->stackHeadPtr);
    return TRUE;
#else /* ! XK_PROXY_MSG_HACK */
    return FALSE;
#endif /* XK_PROXY_MSG_HACK */

}

