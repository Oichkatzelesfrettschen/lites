/*     
 * ip_frag.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.6 $
 * $Date: 1993/05/05 22:41:13 $
 */

/*
 * REASSEMBLY ROUTINES
 */



#include "xkernel.h"
#include "ip.h"
#include "ip_i.h"


#ifdef __STDC__

static void		displayFragList( FragList * );
static void 		hole_create(FragInfo *, FragInfo *, u_int, u_int);

#else

static void		displayFragList();
static void 		hole_create();

#endif __STDC__


xkern_return_t
ipReassemble(s, down_s, dg, hdr)
    XObj s;
    XObj down_s;
    Msg *dg;
    IPheader *hdr;
{
    PState 	*ps = xMyProtl(s)->state;
    FragId 	fragid;
    FragList	*list;
    FragInfo 	*fi, *prev;
    Hole 	*hole;
    u_short 	offset, len;
    xkern_return_t xkr;
    
    offset = FRAGOFFSET(hdr->frag)*8;
    len = hdr->dlen - GET_HLEN(hdr) * 4;
    xTraceP3(s, TR_EVENTS, "IP reassemble, seq=%d, off=%d, len=%d",
	    hdr->ident, offset, len);
    
    fragid.source = hdr->source;
    fragid.dest = hdr->dest;	/* might be multiple IP addresses for me! */
    fragid.prot = hdr->prot;
    fragid.pad  = 0;
    fragid.seqid = hdr->ident;
    
    if ( mapResolve(ps->fragMap, &fragid, &list) == XK_FAILURE ) {
	xTraceP0(s, TR_MORE_EVENTS, "reassemble, allocating new Fraglist");
	list = X_NEW(FragList);
	list->binding = mapBind(ps->fragMap, (char *)&fragid, list );
	/* 
	 * Initialize the list with a single hole spanning the whole datagram 
	 */
	list->nholes = 1;
	list->head.next = fi = X_NEW(FragInfo);
	fi->next = 0;
	fi->type = RHOLE;
	fi->u.hole.first = 0;
	fi->u.hole.last = INFINITE_OFFSET;
    } else {
	xTraceP1(s, TR_MORE_EVENTS,"reassemble - found fraglist == %x", list);
    }
    list->gcMark = FALSE;
    
    xIfTrace(ipp, TR_DETAILED) {
	xTraceP0(s, TR_DETAILED, "frag table before adding");
	displayFragList(list);
    }
    prev = &list->head;
    for ( fi = prev->next; fi != 0; prev = fi, fi = fi->next ) {
	if ( fi->type == RFRAG ) {
	    continue;
	}
	hole = &fi->u.hole;
	if ( (offset < hole->last) && ((offset + len) > hole->first) ) {
	    xTraceP0(s, TR_MORE_EVENTS, "reassemble, found hole for datagram");
	    xTraceP2(s, TR_DETAILED, "hole->first: %d  hole->last: %d",
		    hole->first, hole->last);
	    /*
	     * check to see if frag overlaps previously received
	     * frags.  If it does, we discard parts of the new message.
	     */
	    if ( offset < hole->first ) {
		xTraceP0(s, TR_MORE_EVENTS, "Truncating message from left");
		msgPopDiscard(dg, hole->first - offset);
		offset = hole->first;
	    }
	    if ( (offset + len) > hole->last ) {
		xTraceP0(s, TR_MORE_EVENTS, "Truncating message from right");
		msgTruncate(dg, hole->last - offset); 
		len = hole->last - offset;
	    }
	    /* now check to see if new hole(s) need to be made */
	    if ( ((offset + len) < hole->last) &&
		 (hdr->frag & MOREFRAGMENTS) ) {
		/* This hole is not created if this is the last fragment */
		xTraceP0(s, TR_DETAILED, "Creating new hole above");
		hole_create(prev, fi, (offset+len), hole->last);
		list->nholes++;
	    }
	    if ( offset > hole->first ) {
		xTraceP0(s, TR_DETAILED, "Creating new hole below");
		hole_create(fi, fi->next, hole->first, (offset));
		list->nholes++;
	    }
	    /*
	     * change this FragInfo structure to be an RFRAG
	     */
	    list->nholes--;
	    fi->type = RFRAG;
	    msgConstructCopy(&fi->u.frag, dg); 
	    break;
	} /* if found a hole */
    } /* for loop */
    xIfTrace(ipp, TR_DETAILED) {
	xTraceP0(s, TR_DETAILED, "frag table after adding");
	displayFragList(list);
    }
    /*
     * check to see if we're done
     */
    if ( list->nholes == 0 ) {
	Msg fullMsg;
	
	xTraceP0(s, TR_MORE_EVENTS, "reassemble: now have a full datagram");
	msgConstructEmpty(&fullMsg);
	for( fi = list->head.next; fi != 0; fi = fi->next ) {
	    xAssert( fi->type == RFRAG );
	    msgJoin(&fullMsg, &fi->u.frag, &fullMsg);
	}
	xkr = mapRemoveBinding(ps->fragMap, list->binding);
	xAssert( xkr == XK_SUCCESS );
	ipFreeFragList(list);
	xTraceP1(s, TR_EVENTS,
		 "IP reassemble popping up message of length %d",
		 msgLen(&fullMsg));
	xkr = ipMsgComplete(s, down_s, &fullMsg, hdr);
	msgDestroy(&fullMsg);
    } else {
	xkr = XK_SUCCESS;
    }
    return xkr;
}


/* hole_create :
 *   insert a new hole frag after the given list with the given 
 *   first and last hole values
 */
static void
hole_create(prev, next, first, last)
    FragInfo *prev, *next;
    u_int first;
    u_int last;
{
    FragInfo 	*fi;
    
    xTrace2(ipp,5,"IP hole_create : creating new hole from %d to %d",
	    first,last);
    fi = X_NEW(FragInfo);
    fi->type = RHOLE;
    fi->u.hole.first = first;
    fi->u.hole.last = last;
    fi->next = next;
    prev->next = fi;
}

void
ipFreeFragList( l )
    FragList *l;
{
    FragInfo *fi, *next;
    
    for( fi = l->head.next; fi != 0; fi = next ) {
	next = fi->next;
	if (fi->type == RFRAG) {
	    msgDestroy(&fi->u.frag);
	}
	xFree((char *)fi);
    }
    xFree((char *)l);
}



static void
displayFragList(l)
    FragList *l;
{
    FragInfo 	*fi;

    xTrace2(ipp, TR_ALWAYS, "Table has %d hole%c", l->nholes,
	    l->nholes == 1 ? ' ' : 's');
    for ( fi = l->head.next; fi != 0; fi = fi->next ) {
	if ( fi->type == RHOLE ) {
	    xTrace2(ipp, TR_ALWAYS, "hole  first == %d  last == %d",
		    fi->u.hole.first, fi->u.hole.last);
	} else {
	    xTrace1(ipp, TR_ALWAYS, "frag, len %d", msgLen(&fi->u.frag));
	}
    } 
}
