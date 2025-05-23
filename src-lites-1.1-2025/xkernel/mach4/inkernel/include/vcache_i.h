
/* 
 * vcache_i.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.8 $
 * $Date: 1993/03/16 22:13:44 $
 */

#define VCACHE_ALL_SESSN_MAP_SZ	511	/* sessn keyed (all vcache sessns) */
#define	VCACHE_HLP_MAP_SZ	11	/* hlp keyed */
#define VCACHE_COLLECT_INTERVAL	15	/* seconds */


typedef struct {
    Map 	passiveMap;

    /* 
     * idleMap:  { lls -> sessn }
     */
    Map 	idleMap;

    /* 
     * activeLlsMap:  { lls -> sessn }
     */
    Map		activeMap;

    /* 
     * 'symmetric' indicates whether the sessions of the lower
     * protocol are symmetric.
     */
    bool	symmetric;
} PState;

typedef struct SState {
    bool		cachable;
    IPhost		remote_host;
} SState;



extern int	tracevcachep;



