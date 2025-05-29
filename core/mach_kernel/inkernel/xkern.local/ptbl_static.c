/*     
 * $RCSfile: ptbl_static.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/11/11 22:30:10 $
 */


#include "prottbl.h"

typedef struct {
    char	*name;
    long	relNum;
} MapEntry;


typedef struct {
    char 	*name;
    long	id;
    MapEntry	*map;
} Entry;


#include "ptblData.c"


int
protTblParse( s )
    char *s;
{
    static int	done = 0;
    Entry 	*e = entries;
    MapEntry	*mapEnt;

    printf("protTblParse called\n");
    if ( ! done ) {
	done = 1;
	for ( e = entries; e->name; e++ ) {
	    protTblAddProt(e->name, e->id);
	    if ( e->map ) {
		for ( mapEnt = e->map; mapEnt->name; mapEnt ++ ) {
		    protTblAddBinding(e->name, mapEnt->name, mapEnt->relNum);
		}
	    }
	}
    }
    return 0;
}


void
prottbl_init()
{
    protTblInitMaps();
    /* 
     * Use a fake filename ... this will just call back to
     * protTblParse where we load in the static table.
     */
    if ( protTblBuild("in-kernel") ) {
	xError("\nERROR building x-kernel protocol table\n");
    }
}
