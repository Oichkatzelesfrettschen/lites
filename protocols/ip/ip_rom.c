/*     
 * ip_rom.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.4 $
 * $Date: 1993/02/09 03:29:01 $
 */

/* 
 * ROM file processing
 */


#include "xkernel.h"
#include "ip_i.h"

#define ERROR { sprintf(errBuf,	 "IP ROM file format error in line %d", i+1); \
		xError(errBuf); }

void
ipProcessRomFile()
{
    int		i;
    IPhost	iph;

    for ( i=0; rom[i][0]; i++ ) {
	if ( ! strcmp(rom[i][0], "ip") ) {
	    if ( ! rom[i][1] || ! rom[i][2] ) {
		ERROR;
		continue;
	    }
	    if ( ! strcmp(rom[i][1], "gateway") ) {
		if ( str2ipHost(&iph, rom[i][2]) == XK_FAILURE ) {
		    ERROR;
		    continue;
		}
		ipSiteGateway = iph;
		xTrace1(ipp, TR_EVENTS, "loaded default GW %s from rom file",
			ipHostStr(&ipSiteGateway));
	    }
	}
    }
}
