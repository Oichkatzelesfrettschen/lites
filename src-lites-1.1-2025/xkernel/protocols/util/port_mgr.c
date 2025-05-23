/* 
 * port_mgr.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.20 $
 * $Date: 1993/09/28 20:44:36 $
 */

/* 
 * Management of ports
 *
 * This file is designed to be included by another source file which
 * defines these macros:
 *
 *	PORT -- the port type 
 *	MAX_PORT -- maximum allowed port
 *	FIRST_USER_PORT -- the first port which may be handed out through
 *		'getFreePort'
 *	NAME -- token to prepend to the routine names
 *	PROT_NAME -- string of protocol name (for debugging)
 *	TRACE_VAR -- trace variable to use in tracing
 * 
 * NOTE -- this code assumes a port is no larger than an int.
 * 
 */

#include "xkernel.h"

#define new(Type) (Type *)xMalloc(sizeof(Type))


int	traceportmgr;
#define DUMP(_ps)    xIfTrace(portmgr, TR_DETAILED) { displayMap(_ps); }



typedef struct {
    int		rcnt;
    long	port;
} PortDesc;


typedef struct {
    Map		portMap;
    unsigned long nextPort;
} port_state;


#ifdef __STDC__

static int	displayElem( void *, void *, void * );
static void	displayMap( port_state * );
static int	portBind( port_state *, long, int ) ;
static void	portUnbind( port_state *, PortDesc * );

#else

static int	displayElem();
static void	displayMap();
static int	portBind() ;
static void	portUnbind();

#endif /* __STDC__ */



void
#ifdef __STDC__
PASTE(NAME,PortMapInit)
#else
NAME/**/PortMapInit  
#endif
  (pst) VOID *pst;
{
  port_state **ps = (port_state **)pst;

  if (!(*ps))  {
    *ps = (port_state *)xMalloc(sizeof (port_state));
    bzero((char *)*ps, sizeof(port_state));
  }
  if ( ! (*ps)->portMap ) {
    (*ps)->portMap = mapCreate(PORT_MAP_SIZE, sizeof(long));
    (*ps)->nextPort = FIRST_USER_PORT;
  }
}


void
#ifdef __STDC__
PASTE(NAME,PortMapClose)
#else
NAME/**/PortMapClose  
#endif
  (pst) VOID *pst;
{
  port_state **ps = (port_state **)pst;

  if (!*ps)  return;
  if ( (*ps)->portMap ) {
     mapClose((*ps)->portMap);
  }
  xFree((char *)*ps);
}

struct dmargs {
    int	i;
    char		msgBuf[200];
  };

static int
displayElem( key, value, idv )
    VOID *key;
    VOID *value;
    VOID *idv;
{
    PortDesc	*pd = (PortDesc *)value;
    struct dmargs *idx = (struct dmargs *)idv;

    xAssert(pd);
    sprintf(idx->msgBuf, 
	    "Element %d:	  port = %d  rcnt = %d",
	   ++idx->i, pd->port, pd->rcnt);
    xError(idx->msgBuf);
    return MFE_CONTINUE;
}

static void
displayMap(ps)
    port_state *ps;
{
  struct dmargs args;

    args.i = 0;
    sprintf(args.msgBuf, "dump of %s port map:", PROT_NAME);
    xError(args.msgBuf);
    mapForEach(ps->portMap, displayElem, &args);
}



/* 
 * Binds 'port' into the map with the indicated reference count.
 * Returns 0 on a successful bind, 1 if the port could not be bound
 * (indicating that it was already bound.)
 */
static int
portBind( ps, port, rcnt )
    port_state *ps;
    long port;
    int rcnt;
{
    PortDesc	*pd;

    pd = new(PortDesc);
    pd->rcnt = rcnt;
    pd->port = port;
    if ( mapBind(ps->portMap, (char *)&port, (int)pd) == ERR_BIND ) {
	xFree((char *)pd);
	return 1;
    } 
    return 0;
}


static void
portUnbind( ps, pd )
    port_state *ps;
    PortDesc *pd;
{
    xAssert( pd && pd != (PortDesc *) -1 );
    mapUnbind(ps->portMap, &pd->port);
    xFree((char *)pd);
}


int
#ifdef __STDC__
PASTE(NAME,GetFreePort)
#else
NAME/**/GetFreePort
#endif
  ( pst, port )
    VOID *pst;
    long *port;
{
    port_state *ps = (port_state *)pst;
    unsigned long firstPort;

    xAssert(ps->portMap);
    firstPort = ps->nextPort;
    do {
	*port = ps->nextPort;
	if (ps->nextPort >= MAX_PORT) {
	    ps->nextPort = FIRST_USER_PORT;
	} else {
	    ps->nextPort++;
	} /* if */
	if ( portBind(ps, *port, 1) == 0 ) {
	    /* 
	     * Found a free port
	     */
	    DUMP(ps);
	    return 0;
	}
    } while ( ps->nextPort != firstPort );
    return 1;
}


int
#ifdef __STDC__
PASTE(NAME,DuplicatePort)
#else
NAME/**/DuplicatePort
#endif
  ( pst, port )
    VOID *pst;
    long port;
{
    PortDesc	*pd;
    int		res;
    port_state  *ps = (port_state *)pst;

    xAssert(ps->portMap);
    if ( port > MAX_PORT ) {
	res = 2;
    } else {
	if ( mapResolve(ps->portMap, &port, &pd) == XK_FAILURE ) {
	    /* 
	     * Port is not used, so we know portBind will succeed.
	     */
	    res = portBind(ps, port, 1);
	} else {
	    pd->rcnt++;
	    res = 0;
	}
    }
    DUMP(ps);
    return res;
}


void
#ifdef __STDC__
PASTE(NAME,ReleasePort)
#else
NAME/**/ReleasePort
#endif
  ( pst, port )
    VOID *pst;
    long port;
{
    PortDesc	*pd;
    port_state  *ps = (port_state *)pst;

    xAssert(ps->portMap);
    if ( mapResolve(ps->portMap, &port, &pd) == XK_SUCCESS ) {
	if ( pd->rcnt > 0 ) {
	    if ( --pd->rcnt == 0 ) {
		portUnbind(ps, pd);
	    }
	}
    }
    DUMP(ps);
}
