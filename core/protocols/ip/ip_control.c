/*     
 * ip_control.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.25 $
 * $Date: 1993/08/04 00:43:16 $
 */

#include "xkernel.h"
#include "ip.h"
#include "route.h"
#include "ip_i.h"


#define IPHOSTLEN	sizeof(IPhost)


/*
 * ip_controlsessn
 */
int
ipControlSessn(s, opcode, buf, len)
    XObj s;
    int opcode;
    char *buf;
    int len;
{
    SState        *sstate;
    PState        *pstate;
    IPheader      *hdr;
    
    xAssert(xIsSession(s));
    
    sstate = (SState *)s->state;
    pstate = (PState *)s->myprotl->state;
    
    hdr = &(sstate->hdr);
    switch (opcode) {
	
      case GETMYHOST :
	checkLen(len, IPHOSTLEN);
	*(IPhost *)buf = sstate->hdr.source;
	return IPHOSTLEN;
	
      case GETPEERHOST :
	checkLen(len, IPHOSTLEN);
	*(IPhost *)buf = sstate->hdr.dest;  
	return IPHOSTLEN;
	
      case GETMYHOSTCOUNT:
      case GETPEERHOSTCOUNT:
	checkLen(len, sizeof(int));
	*(int *)buf = 1;
	return sizeof(int);

      case GETMYPROTO :
      case GETPEERPROTO :
	checkLen(len, sizeof(long));
	*(long *)buf = sstate->hdr.prot;
	return sizeof(long);
	
      case GETMAXPACKET :
	checkLen(len, sizeof(int));
	*(int *)buf = IPMAXPACKET;
	return sizeof(int);
	
      case GETOPTPACKET :
	checkLen(len, sizeof(int));
	*(int *)buf = sstate->mtu - IPHLEN;
	return sizeof(int);
	
      case IP_REDIRECT:
	return ipControlProtl(s->myprotl, opcode, buf, len);

      case IP_PSEUDOHDR:
	return 0;
	
      case GETPARTICIPANTS:
	{
	    Part	p[2];
	    int		retLen;

	    retLen = xControl(xGetDown(s, 0), opcode, buf, len);
	    if ( retLen > 0 && partExtLen(buf) > 0 && partExtLen(buf) <= 2 ) {
		partInternalize(p, buf);
		/* 
		 * We may have rewritten the remote participant to be
		 * a gateway, so we'll replace it with the ultimate
		 * destination address.  The local participant (if
		 * it's there) should be OK.
		 */
		if ( partPop(p[0]) == 0 ) {
		    return -1;
		}
		partPush(p[0], &sstate->hdr.dest, sizeof(IPhost));
		return (partExternalize(p, buf, &len) == XK_FAILURE) ?
		  		-1 : len;
	    } else {
		return -1;
	    }
	}

      case IP_GETPSEUDOHDR:
	{
	    IPpseudoHdr	*phdr = (IPpseudoHdr *)buf;

	    checkLen(len, sizeof(IPpseudoHdr));
	    phdr->src = sstate->hdr.source;
	    phdr->dst = sstate->hdr.dest;
	    phdr->zero = 0;
	    phdr->len = 0;
	    phdr->prot = sstate->hdr.prot;
	    return sizeof(IPpseudoHdr);
	}

      default : 
	xTrace0(ipp,3,"Unhandled opcode -- forwarding");
	return xControl(xGetDown(s, 0), opcode, buf, len);
    }
}



/*
 * ip_controlprotl
 */
int
ipControlProtl(self, opcode, buf, len)
    XObj self;
    int opcode;
    char *buf;
    int len;
{
    PState	*pstate;
    IPhost 	net, mask, gw, dest;
    
    xAssert(xIsProtocol(self));
    pstate = (PState *) self->state;
    
    switch (opcode) {
	
      case IP_REDIRECT :
	{
	    checkLen(len, 2*IPHOSTLEN);
	    net = *(IPhost *)buf;
	    netMaskFind(&mask, &net);
	    gw = *(IPhost *)(buf + IPHOSTLEN);
	    xTrace3(ipp, 4, "IP_REDIRECT : net = %s, mask = %s, gw = %s",
		    ipHostStr(&net), ipHostStr(&mask), ipHostStr(&gw));
	    /*
	     * find which interface reaches the gateway
	     */
	    rt_add(pstate, &net, &mask, &gw, -1, RTDEFAULTTTL);
	    return 0;
	}

      case GETMAXPACKET :
	checkLen(len, sizeof(int));
	*(int *)buf = IPMAXPACKET;
	return sizeof(int);
	
      case GETOPTPACKET:
	/* 
	 * A somewhat meaningless question to be asking the protocol.
	 * It makes more sense to ask an individual session that knows
	 * about the MTU.
	 */
	checkLen(len, sizeof(int));
	*(int *)buf = IPOPTPACKET - IPHLEN;
	return sizeof(int);
	
	/* test control ops - remove later */
      case IP_GETRTINFO:
	/* get route info for a given dest address :
	   in : IP host address 
	   out : route structure for this address
	   */
	{
	    xkern_return_t	xkr;
	    
	    checkLen(len, sizeof(route));
	    dest = *(IPhost *)buf;
	    xkr = rt_get(&pstate->rtTbl, &dest, (route *)buf);
	    return (xkr == XK_SUCCESS) ? sizeof(route) : -1;
	}

      case IP_PSEUDOHDR:
	return 0;

      default:
	xTrace0(ipp,3,"Unrecognized opcode");
	return xControl(xGetDown(self, 0), opcode, buf, len);
    }
}


