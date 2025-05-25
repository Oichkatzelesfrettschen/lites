/* 
 * simsimeth.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.3 $
 * $Date: 1993/10/02 00:16:53 $
 */

#ifndef XKMACH4

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <netinet/in.h>
#include "x_stdio.h"
#include "xkernel.h"
#include "eth.h"
#include "eth_i.h"
#include "simsimeth_i.h"
#include "arp.h"

int	tracesimsimethp;


static ETHhost		ethBcastHost = ETH_BCAST_HOST;


#ifdef __STDC__

void			simsimeth_init( XObj );

static int		arpForEachFunc( ArpBinding *, VOID * );
static void		processRomFile( XObj );
static void		sendOnSocket( XObj, ETHhost *, Msg *);
static int		simsimethControl( XObj, int, char *, int );
static xkern_return_t	simsimethOpenEnable( XObj, XObj, XObj, Part * );
static xmsg_handle_t	simsimethPush( XObj, Msg * );
static xkern_return_t	simsimethDemux( XObj, XObj, Msg * );
static void		writeBcast( XObj, Msg *);
static void 		simEth2udp(ETHhost, IPhost *, long *);
static void 		udp2simEth(char *, IPhost  *, int);

#endif /* __STDC__ */


/* changes to support full internet addressing in rom files */

void simEth2udp(ethAddr,host, port)
    ETHhost ethAddr;
    IPhost *host;
    long   *port;
{
  u_short port2;

  /* 
   * IP address is in the first 4 bytes of the ethernet address
   * UDP port is in the 5th and 6th bytes of the ethernet address
   */
  *host = *(IPhost *)&ethAddr;
  
  port2 = ntohs((*(u_short *)((char *)&ethAddr + 4)));
  *port = port2;
}


void udp2simEth(ethAddr, inAddr, udpPort)
     char *ethAddr;
     IPhost *inAddr;
     int udpPort;
{
    char *cp1, *cp2;
    short tmpshrt;
    int i;
    
    cp1 = ethAddr;
    cp2 = (char *) inAddr;		
    for (i=0; i<4; i++) *cp1++ = *cp2++;
    tmpshrt = udpPort;
    tmpshrt = htons(tmpshrt);
    cp2 = (char *) &tmpshrt;
    for (i=0; i<2; i++) *cp1++ = *cp2++;
}


static int
arpForEachFunc( ab, arg )
    ArpBinding	*ab;
    VOID 	*arg;
{
    EthMsg	*m = (EthMsg *)arg;

    sendOnSocket(m->self, &ab->hw, m->msg);
    return TRUE;
}


/* 
 * We simulate broadcast by having ARP perform a callback
 * for every host it has in its table.  We then do a direct send for
 * each ARP entry.
 */
static void
writeBcast( self, msg)
XObj	self;
Msg	*msg;
{
    EthMsg	m;
    PState	*ps= (PState *)self->state;
    ArpForEach	afe;
    
    if ( ! ps->arp ) {
	xError("eth bcast write fails -- no arp protocol");
	return;
    }
    m.self = self;
    m.msg = msg;
    afe.f = arpForEachFunc;
    afe.v = &m;
    xControl(ps->arp, ARP_FOR_EACH, (char *)&afe, sizeof(ArpForEach));
}	


static void
sendOnSocket( self, dest, msg)
    XObj	self;
    ETHhost	*dest;
    Msg		*msg;
{  
    PState	*ps= (PState *) self->state;
    XObj  lls; 
    IPhost host;
    long port;
    Part  p[5];

    if ( mapResolve(ps->map, (char *)dest, &lls) == XK_FAILURE ) {
        xTrace0(simsimethp, TR_EVENTS, "sendOnSocket: New Eth Address");
	/* need to open new udp session */
  	ps = (PState *) self->state;
        simEth2udp(*dest, &host, &port);
        xTrace2(simsimethp, 5, "simsimeth: sendonsocket IP address (%s) port %d",
            ipHostStr((IPhost *)&host), port);
        partInit(p,1);
	partPush(p[0],&host,sizeof(IPhost));
	partPush(p[0],&port,sizeof(long));
        lls = xOpen(self, self, xGetDown(self, 0), p);
  	xDuplicate(lls);
	if (lls == ERR_XOBJ) {
           xTrace0(simsimethp, TR_ERRORS, "sendOnSocket: can't open socket");
	   return;
	}
        mapBind(ps->map, dest, lls);
    }

    xPush(lls,msg);
}


/* 
 * Sets the 'port' field of the protocol state
 */
static void
processRomFile( self )
    XObj	self;
{
    PState	*ps = (PState *)self->state;
    int 	i;

    ps->port = -1;
    for ( i=0; rom[i][0]; i++ ) {
	if ( ! strncmp(rom[i][0], self->name, strlen(self->name)) ) {
	    if ( ! strcmp(rom[i][0], self->fullName) ) {
		if ( rom[i][1] ) {
		    if ( ! strcmp(rom[i][1], "port") && rom[i][2] && 
			 sscanf(rom[i][2], "%d", &ps->port) >= 1 ) {
			continue;
		    }
		    /* 
		     * A lone number in the second field represents
		     * the port (backward compatibility)
		     */
		    if ( sscanf(rom[i][1], "%d", &ps->port) >= 1 ) {
			continue;
		    }
		}
	    } else {
		continue;
	    }
	    sprintf(errBuf, "%s format error in line %d of the rom file",
		    self->fullName, i + 1);
	    xError(errBuf);
	    break;
	}
    }
}


void
simsimeth_init( self )
    XObj	self;
{
    PState		*ps;
    Part		p[5];
    IPhost		host;
    
    
    xTrace0(simsimethp, TR_MAJOR_EVENTS, "init_ether");

    ps = X_NEW(PState);
    bzero((char *)ps, sizeof(PState));
    self->state = (VOID *)ps;
    processRomFile(self);
    if ( ps->port == -1 ) {
	Kabort("simsimeth -- port specification error");
    }
    /* create eth address to udp session map */
    ps->map = mapCreate(simsimethMapSize,sizeof(ETHhost));

    /* open udp session */ 
    partInit(p, 1);
    partPush(*p, ANY_HOST, 0);
    partPush(*p, &ps->port, sizeof(long));

    xTrace1(simsimethp, TR_MAJOR_EVENTS,
	    "init_ether: listening on port %d", ps->port);

    if ( xOpenEnable(self, self, xGetDown(self,0), p) == XK_FAILURE ) {
	Kabort("simsimeth_init: OpenEnable fails");
    }

    /* 
     * Determine my host address
     */
    if ( xControl(xGetDown(self,0), GETMYHOST, (char *)&host, sizeof(IPhost)) == -1 ) {
	Kabort("simsimeth_init: GETMYHOST fails");
    }
    xTrace2(simsimethp, 5, "simsimeth: IP address (%s) port %d",
            ipHostStr((IPhost *)&host), ps->port);

    udp2simEth((char *)&ps->myHost, &host, ps->port);

    xTrace1(simsimethp, TR_GROSS_EVENTS,
	    "init_ether: ethernet started with address %s",
	    ethHostStr(&ps->myHost));

    self->push = simsimethPush;
    self->demux = simsimethDemux;
    self->control = simsimethControl;
    self->openenable  = simsimethOpenEnable;
    self->up = 0;
}


static void
ethMsgStore( void *hdr, char *netHdr, long len, void *arg )
{
    xAssert(len == sizeof(ETHhdr));
    ((ETHhdr *)hdr)->type = ((ETHhdr *)hdr)->type;
    bcopy(hdr, netHdr, sizeof(ETHhdr));
}


static long
ethMsgLoad( void *hdr, char *netHdr, long len, void *arg )
{
    xAssert(len == sizeof(ETHhdr));
    bcopy(netHdr, (char *)hdr, sizeof(ETHhdr));
    ((ETHhdr *)hdr)->type = ((ETHhdr *)netHdr)->type;
    return sizeof(ETHhdr);
}


static xmsg_handle_t
simsimethPush( self, msg )
    XObj	self;
    Msg 	*msg;
{
    ETHhdr	*hdr = msgGetAttr(msg, 0);
    int 	len;

    xTrace0(simsimethp, TR_EVENTS, "simsimethPush");
    xAssert(hdr);
    msgPush(msg, ethMsgStore, hdr, sizeof(ETHhdr), NULL);
    if ( (len = msgLen(msg)) > EMAXPAK ) {
	xTrace2(simsimethp, TR_SOFT_ERRORS,
		"sim ether driver: msgLen (%d) is larger than max (%d)",
		len, EMAXPAK);
	return XMSG_ERR_HANDLE;
    }

    if ( ETH_ADS_EQUAL(hdr->dst, ethBcastHost) ) {
	writeBcast(self, msg);
    } else {
	sendOnSocket(self, &hdr->dst, msg);
    }
    return XMSG_NULL_HANDLE;
}    


static xkern_return_t
simsimethDemux( self, lls, msg )
XObj	self, lls;
Msg	*msg;
{
    ETHhdr	hdr;
    Msg	new_msg;
    
    xTrace0(simsimethp, TR_EVENTS, "in simsimethDemux");
    
    if (! msgPop(msg, ethMsgLoad, (void *)&hdr, sizeof(ETHhdr), NULL)) {
	xTrace0(simsimethp, TR_SOFT_ERRORS,
		"eth_demux: incoming message too small!");
	return XK_SUCCESS;
    }
    if ( ! self->up ) {
	xTrace0(simsimethp, TR_ERRORS, "simsimethDemux: no upper protocol!");
	return XK_SUCCESS;
    }
    xDuplicate(lls);
    
    msgConstructEmpty(&new_msg);
    msgAssign(&new_msg,msg);
    msgSetAttr(&new_msg, 0, &hdr, sizeof(ETHhdr));
    xDemux(self, &new_msg);
    return XK_SUCCESS;
}


static xkern_return_t
simsimethOpenEnable( self, hlpRcv, hlpType, p )
    XObj	self, hlpRcv, hlpType;
    Part	*p;
{
    if ( self->up ) {
	xError("simsimethOpenEnable called multiple times!");
	return XK_FAILURE;
    }
    self->up = hlpRcv;
    return XK_SUCCESS;
}


static int
simsimethControl( s, op, buf, len )
    XObj	s;
    int 	op, len;
    char 	*buf;
{
    PState	*ps = (PState *)s->state;

    switch (op) {

      case ETH_REGISTER_ARP:
	/* 
	 * ARP registers itself with us so we can ask it to perform
	 * callbacks in order to simulate hardware broadcast.  
	 */
	checkLen(len, sizeof(XObj));
	ps->arp = *(XObj *)buf;
	return 0;

      case GETMYHOST:
	checkLen(len, sizeof(ETHhost));
	bcopy((char *) &ps->myHost, buf, sizeof(ETHhost));
	return (sizeof(ETHhost));

      default:
	return -1;

    }
}
    
#endif /* !XKMACH4 */
