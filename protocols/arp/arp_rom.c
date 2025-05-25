/* 
 * $RCSfile: arp_rom.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/11/15 20:51:01 $
 */

/*
 * initialize table from ROM entries
 */

#include "xkernel.h"
#include "arp.h"
#include "arp_i.h"
#include "arp_table.h"
#include "romopt.h"
#include "sim.h"

#ifdef __STDC__

static xkern_return_t loadEntry( XObj, char **, int, int, VOID * );

#else

static xkern_return_t loadEntry();

#endif



static XObjRomOpt	arpOpts[] = {
    { "", 3, loadEntry }
};
 

static xkern_return_t
loadEntry( self, str, nFields, line, arg )
    XObj	self;
    char	**str;
    int		nFields, line;
    VOID	*arg;
{
    ETHhost	ethHost;
    IPhost	ipHost;
    PSTATE	*ps = (PSTATE *)self->state;
    int		nextField;

    if ( str2ipHost(&ipHost, str[1]) == XK_FAILURE )
    	return XK_FAILURE;
    if ( str2ethHost(&ethHost, str[2]) == XK_SUCCESS ) {
	nextField = 3;
    } else {
	/* 
	 * Second field isn't an ethernet address.  See if it's
	 * one of the alternate ways of specifying a hardware address
	 * (there is currently only one alternate)
	 */
	{
	    /* 
	     * Look for SIM address, an IP-host/UDP-port pair
	     */
	    SimAddrBuf	buf;

	    if ( nFields < 4 ) return XK_FAILURE;
	    if ( str2ipHost(&buf.ipHost, str[2]) == XK_FAILURE ) {
		return XK_FAILURE;
	    }
	    if ( sscanf(str[3], "%d", &buf.udpPort) < 1 ) {
		return XK_FAILURE;
	    }
	    if ( xControl(xGetDown(self, 0), SIM_SOCK2ADDR,
			  (char *)&buf, sizeof(buf)) < 0 ) {
		xTraceP0(self, TR_ERRORS, "llp couldn't translate rom entry");
		return XK_FAILURE;
	    }
	    bcopy((char *)&buf.ethHost, (char *)&ethHost, sizeof(ETHhost));
	    nextField = 4;
	}
    }
    arpSaveBinding( ps->tbl, &ipHost, &ethHost );
    xTraceP1(self, TR_MAJOR_EVENTS, "loaded (%s) from rom file",
	    ipHostStr(&ipHost));
    xTraceP1(self, TR_MAJOR_EVENTS, "corresponding eth address: %s",
	    ethHostStr(&ethHost));

    if ( nFields > nextField ) {
	if ( ! strcmp(str[nextField], "lock") ) {
	    arpLock(ps->tbl, &ipHost);
	} else {
	    return XK_FAILURE;
	}
    }
    return XK_SUCCESS;
}


void
arpPlatformInit( self )
    XObj self;
{
    /*
     * Check the rom file for arp initialization 
     */
    findXObjRomOpts(self, arpOpts, sizeof(arpOpts)/sizeof(XObjRomOpt), 0);
}
