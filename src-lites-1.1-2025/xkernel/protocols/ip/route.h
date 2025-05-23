/*
 * route.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.12 $
 * $Date: 1993/03/26 02:54:38 $
 */

/*
 * Interface to the routing subsystem
 */

#ifndef route_h
#define route_h

#define IPROUTE_TTL_INFINITE	0xffff
#define RTDEFAULTTTL    60*24	  /* default ttl for a route - 1 day */

typedef struct route {
	IPhost	net; 		/* net for this route */
	IPhost  mask;		/* mask for this net */
 	IPhost  gw;		/* gateway for hop */
	int 	metric; 	/* distance metric */
	Sessn 	netSessn; 	/* cached network session */
	u_short key;		/* sort key */
	u_short ttl;		/* time to live */
	struct route *next;	/* next for this hash */
} route;


typedef struct {
	u_char  valid;		/* is table valid */
	route	*defrt;		/* default route */
	u_short bpoolsize;	/* number of unallocated structures remaining */
	route	**arr;
} RouteTable;


#include "ip_i.h"

#ifdef __STDC__

/* 
 * Initialize the routing tables and set the default router to be the
 * given IP host.  If the IP host is all zeroes or if it is not
 * directly reachable, there will be no default router and rt_init
 * will return XK_FAILURE.
 */
xkern_return_t 	rt_init( PState *, IPhost * );
xkern_return_t 	rt_add( PState *, IPhost *, IPhost *, IPhost *, int, int );
xkern_return_t	rt_add_def( PState *, IPhost * );
xkern_return_t	rt_get( RouteTable *, IPhost *, route * );
void		rt_delete( RouteTable *, IPhost *, IPhost * );

#else

xkern_return_t 	rt_init();
xkern_return_t 	rt_add();
xkern_return_t	rt_add_def();
xkern_return_t	rt_get();
void		rt_delete();

#endif __STDC__

#endif  ! route_h
