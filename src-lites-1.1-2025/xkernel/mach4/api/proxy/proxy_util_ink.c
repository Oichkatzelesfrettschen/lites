/*     
 * $RCSfile: proxy_util_ink.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/09/21 00:20:25 $
 */

#include "xkernel.h"
#include "proxy_util.h"
#include "msg_internal.h"
#include "xk_mig_sizes.h"
#include <mach/mach_types.h>
#include <vm/vm_kern.h>

extern int	traceproxy;

/* 
 * Deallocate an out-of-line region that was received from another proxy.
 */
void
oolFreeIncoming( p, len )
    VOID	*p;
    int		len;
{
    kern_return_t	kr;

    xTrace2(proxy, TR_MORE_EVENTS,
	    "uproxy: oolFreeIncoming deallocates %d bytes at %x",
	    len, (u_int)p);
    kr = vm_deallocate(ipc_kernel_map, (vm_address_t)p, len);
    if ( kr != KERN_SUCCESS ) {
	sprintf(errBuf,
		"vm_deallocate fails in  x-kernel proxy oobFree, error: %d", kr);
	xError(errBuf);
    }
}


/* 
 * Deallocate an out-of-line region that was sent to another proxy.
 */
void
oolFreeOutgoing( p, len )
    VOID	*p;
    int		len;
{
    xTrace2(proxy, TR_EVENTS, "oolFreeOutgoing, addr %x, len %d",
	    (u_int)p, len);
    kmem_free(ipc_kernel_map, (vm_address_t)p, len);
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
    vm_offset_t		oolData;
    kern_return_t	kr;

    /* 
     * The object we were passed by MIG is a vm_map_copy_t object.  We
     * need to put it in the kernel VM map before we can use it.
     */
    kr = vm_map_copyout(ipc_kernel_map, &oolData, (vm_map_copy_t)oolPtr);
    xAssert( kr == KERN_SUCCESS );
    xTrace1(proxy, TR_EVENTS, "OOlToMsg, VM region starts at %x", (u_int)oolData);
    msgConstructInplace(msg, (char *)oolData, len, oolFreeIncoming);
}


/* 
 * Convert the data in the given message to an out-of-line region
 * suitable for passing directly as a MIG out-of-line parameter.  For
 * the in-kernel case, this should be an object of type vm_map_copy_t.
 */
xkern_return_t
msgToOol( m, oolPtr, func, arg )
    Msg		*m;
    char	**oolPtr;
    DeallocFunc	*func;
    VOID	**arg;
{

    /* 
     * XXX -- Avoid this copy when possible
     */

    char		*oolBuf; 
    kern_return_t	kr;

    /* 
     * We won't worry about avoiding the copy here.  If the message is
     * large enough that we're transferring it out-of-line, it's going
     * to be in several packets which we'd need to gather into a
     * single large buffer anyway.
     */
    xTrace1(proxy, TR_FUNCTIONAL_TRACE, "msgToOol allocating %d bytes", msgLen(m));
    kr = kmem_alloc_pageable(ipc_kernel_map, (vm_address_t *)&oolBuf, msgLen(m));
    if ( kr != XK_SUCCESS ) {
	xTrace1(proxy, TR_ERRORS, "allocate fails (%s) in msgToOol",
		 mach_error_string(kr));
	return XK_FAILURE;
    }
    xTrace1(proxy, TR_EVENTS, "msgToOOL allocates region %x", (int)oolBuf);
    msg2buf(m, oolBuf);
    /* 
     * Create the copy object, but leave it in the map
     */
    kr = vm_map_copyin(ipc_kernel_map, oolBuf, msgLen(m), TRUE, (vm_map_copy_t *)oolPtr);
    if ( kr != XK_SUCCESS ) {
	xTrace1(proxy, TR_ERRORS, "vm_copyin fails (%s) in msgToOol",
		 mach_error_string(kr));
	return XK_FAILURE;
    }
    *func = 0;
    *arg = 0;

    return XK_SUCCESS;
}


void
lingeringMsgSave( f, arg, len )
    DeallocFunc		f;
    VOID		*arg;
    int			len;
{
}


void
lingeringMsgClear()
{
}



/* 
 * MIG generates some in-kernel mig_allocate calls.  Not sure why it
 * feels it can do that, but they shouldn't get called anyway ...
 */
void
mig_allocate()
{
    Kabort("xkernel: mig_allocate called in-kernel!");
}







#if defined(XK_DEBUG)

#include <kern/zalloc.h>

extern zone_t	vm_map_copy_zone;

void
xkDumpVmStats()
{
    vm_statistics_data_t	stat;
    kern_return_t		kr;
    
    kr = vm_statistics(kernel_map, &stat);
    xAssert( kr == KERN_SUCCESS );
    xTrace0(proxy, TR_ALWAYS, "");
#if 1
    xTrace1(proxy, TR_ALWAYS, "pagesize: %d", stat.pagesize);
    xTrace1(proxy, TR_ALWAYS, "free_count: %d", stat.free_count);
    xTrace1(proxy, TR_ALWAYS, "active_count: %d", stat.active_count);
    xTrace1(proxy, TR_ALWAYS, "inactive_count: %d", stat.inactive_count);
#endif
    xTrace1(proxy, TR_ALWAYS, "wire_count: %d", stat.wire_count);
    xTrace1(proxy, TR_ALWAYS, "zero_fill_count: %d", stat.zero_fill_count);
#if 0
    xTrace1(proxy, TR_ALWAYS, "reactivations: %d", stat.reactivations);
    xTrace1(proxy, TR_ALWAYS, "pageins: %d", stat.pageins);
    xTrace1(proxy, TR_ALWAYS, "pageouts: %d", stat.pageouts);
    xTrace1(proxy, TR_ALWAYS, "faults: %d", stat.faults);
    xTrace1(proxy, TR_ALWAYS, "cow_faults: %d", stat.cow_faults);
    xTrace1(proxy, TR_ALWAYS, "lookups: %d", stat.lookups);
    xTrace1(proxy, TR_ALWAYS, "hits: %d", stat.hits);
#endif

    xTrace0(proxy, TR_ALWAYS, "");
    xTrace0(proxy, TR_ALWAYS, "vm_map_copy_zone:");
    xTrace2(proxy, TR_ALWAYS, "size: %d  max: %d",
	    vm_map_copy_zone->cur_size, vm_map_copy_zone->max_size);

#if TRACE_KERNEL_ZONES
    xIfTrace(proxy, TR_SOFT_ERRORS) {
	print_host_zone_info();
    }
#endif
}


#endif /* XK_DEBUG */
