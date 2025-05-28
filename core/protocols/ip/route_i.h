/*
 * route_i.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.3 $
 * $Date: 1993/03/26 02:55:11 $
 */

/*
 * Structures and constants internal to the routing subsystem
 */

#ifndef route_i_h
#define route_i_h

#define ROUTETABLESIZE  512
#define BPSIZE		100
#define OK		1
#define RTTABLEDELTA    1	  /* decrement route ttl by 1 every update */
#define RTTABLEUPDATE  	1000*60   /* update route table every minute */


#endif ! route_i_h
