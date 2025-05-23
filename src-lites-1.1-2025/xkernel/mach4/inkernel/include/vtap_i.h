/* 
 * vtap_i.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Date: 1993/03/11 21:03:50 $
 */

#ifndef vtap_i_h
#define vtap_i_h

#define	VTAP_ACT_MAP_SZ	11
#define	VTAP_PAS_MAP_SZ	11

typedef struct {
    Map		activeMap;
    Map 	passiveMap;
} PState;

typedef struct {
  int   tap_enabled;
  int   print_chars;
} SState;

#endif
