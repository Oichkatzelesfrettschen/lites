/*     
 * $RCSfile: xk_flags.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/05/12 23:38:02 $
 */

#include "platform.h"
#include "xk_debug.h"

static int	count;
static char	xkRomFlags[10][80];


/* 
 * Called for every command line parameter which assigns an x-kernel
 * romfile entry.
 * 
 * This routine is called very early.  Don't rely on any x-kernel
 * stuff (including trace routines) being properly initialized. 
 */
unsigned
xk_boot_flag_rom( addr, flag )
    unsigned char	*addr;
    char		*flag;
{
    int	n;

    if ( count + 1 < sizeof(xkRomFlags) ) {
	n = strlen(flag);
	if ( n < sizeof(xkRomFlags[0]) ) {
	    strcpy(xkRomFlags[count], flag);
	    count++;
	}
    }
    return 0;
}


#define isspace(_c) ((_c) == ' ' || (_c) == '\t')

/* 
 * Add rom entries from the boot parameters to the actual rom array. 
 * This actually writes into the xkRomFlags array.
 */
void
addRomFlags()
{
    int	i;	/* index into global rom array */
    int	j;	/* index into local xkRomFlags */
    int	k;	/* Tracks fields in a single rom option */
    char *p;

    for ( i=0; rom[i][0]; i++ )
      ;
    for ( j=0; j < count; i++, j++ ) {
	if ( i > ROM_MAX_LINES ) {
	    xError("too many rom lines");
	    return;
	}
	p = &xkRomFlags[j][0];
	xTrace1(init, TR_ALWAYS, "xkernel adding rom option: %s", p);

	for ( k=0; *p; k++ ) {
	    if ( k >= ROM_MAX_FIELDS ) {
		sprintf(errBuf,
			"Added ROM entry %d has too many fields (max %d)",
			j+1, ROM_MAX_FIELDS);
		xError(errBuf);
		break;
	    }
	    rom[i][k] = p;
	    /* 
	     * Find and mark the end of this field
	     */
	    while ( *p && ! isspace(*p) ) {
		p++;
	    }
	    if ( *p ) {
		*p++ = 0;
		/* 
		 * Find the start of the next field
		 */
		while ( *p && isspace(*p) ) {
		    p++;
		}
	    }
	}
    }
}
