/* 
 * vsize_i.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.7 $
 * $Date: 1993/07/26 22:13:27 $
 */

#define VSIZEMAXDOWN 8

typedef  struct {
    Map		activeMap;
    Map 	passiveMap;
    /* 
     * If a hard cutoff is specified in configuration, it is stored in
     * protocol state and copied to each session state.  If the cutoff
     * is to be determined dynamically for each session (the preferred
     * method), protocol state cutoff is < 0.
     */
    int		numdown; 
    int		cutoff[VSIZEMAXDOWN];
} PSTATE;


typedef struct {
    int		numdown; 
    int		cutoff[VSIZEMAXDOWN];
} SSTATE;

typedef	XObj	PassiveId;
