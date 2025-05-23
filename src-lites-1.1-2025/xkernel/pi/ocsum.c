/*
 * ocsum.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/03/31 17:51:33 $
 */

#include "xk_debug.h"
#include "platform.h"	
#include "assert.h"
#include "msg.h"

/* 
 * This code is not really platform-independent.  It assumes an
 * architecture with 32-bit longs and 16-bit shorts.  A platform for
 * which this is not true can add its own version of the checksum code
 * to the pxk library and override these functions.
 */

int tracecksum = 0;

/*
 * ocsum -- return the 1's complement sum of the 'count' 16-bit words
 * pointed to by 'hdr'.  Move and add by 32 bits at a time.
 *
 * assumes first address is on an even address, aligned for a short
 * assumes next even address is aligned for u_long
 * assumes len is > 0
 *
 *  To test this, please be sure to use 1, 2, 3, 4, and higher buffer
 *  sizes, with alignment at all possible byte positions!
 *  See the cksum_helper routine, especially.
 */
u_short
ocsum( hdr, count )
    u_short *hdr;
    register int count;
{
    register u_long sum = 0;
    register u_long *lp;
    register u_long overflow = 0;
    
    if ((u_int)hdr & 0x2) {   /* get up to a mult of 4 address */
      sum += *hdr++;
      --count;
    }

    lp = (u_long*) hdr;
    while ( count >= 16 ) {
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	sum += *lp;
	if (sum < *lp++) overflow++;
	count -= 16;
    } /* while */
    while ( count >= 2 ) {
	sum += *lp;
	if (sum < *lp++) overflow++;
	count -= 2;
    }
    if (count) {
      hdr = (u_short*) lp;
      sum += *hdr;
      if (sum < *hdr++) overflow++;
    }
    sum += overflow;
    if (sum < overflow) sum++;
    sum = (sum & 0xffff) + ((sum>>16) & 0xffff);
    sum = (sum & 0xffff) + ((sum>>16) & 0xffff);
    return sum;
}

/*
 * ocsum_simple -- return the 1's complement sum of the 'count' 16-bit words
 * pointed to by 'hdr'.  
 */
u_short
ocsum_simple( hdr, count )
    u_short	*hdr;
    int		count;
{
    register u_long acc = 0;
    
    while ( count-- ) {
	acc += *hdr++;
	if ( acc & 0xFFFF0000 ) {
	    /*
	     * Carry occurred -- wrap around
	     */
	    acc &= 0xFFFF;
	    acc++;
	}
    }
    return acc & 0xFFFF;
}


typedef struct {
    u_long 	sum;
    int		odd;
} ckSum_t;

/*
 * cksum_helper -- perform the checksum over the data.  's' should point to
 * a ckSum_t with the current state of the checksum.  The data pointer does
 * not have to be aligned.
 */
static bool
cksum_helper( data, len, s ) 
    char	*data;
    long	len;
    VOID	*s;
{
    bool oddAddr;
    register unsigned long sum = 0;
    int	save_len;

    xTrace2(cksum, TR_DETAILED, "cksum_helper:  data at 0x%x len %d", data, len);
#ifndef ENDIAN
      xError("Machine byte order unknown; cannot compute checksum");
#endif

    save_len = len;
    /*
     * Add first byte if necessary to put 'data' on an even address
     */
    if (oddAddr = ((int)data % 2)) {
#if ENDIAN == LITTLE
	sum = *(u_char *)data++ << 8;
#else
	sum = *(u_char *)data++;
#endif
	len--;
    }
    
    if (len>1) sum += ocsum((u_short *)data, len / 2);
    data += len & ~1;

    /*
     * Add in last byte if there is one
     */
    if (len & 1) {
#if ENDIAN == LITTLE
	sum += *(u_char *)data;
#else
	sum += *(u_char *)data << 8;
#endif
    }
    sum = (sum & 0xffff) + ((sum >> 16) & 0xffff);
    sum = (sum & 0xffff) + ((sum >> 16) & 0xffff);
    /*
     * Swap bytes in the sum if necessary
     */
    if (oddAddr ^ ((ckSum_t *)s)->odd) {
	/*
	 * Wrap possible overflow
	 */
	xAssert(!(sum & 0xffff0000));
	sum = ((sum >> 8) & 0xff) | ((sum & 0xff) << 8);
    }
    /*
     * Add sum to old sum and indicate whether an odd or even total number
     * of bytes have been processed
     */
    ((ckSum_t *)s)->odd = ((ckSum_t *)s)->odd ^ (save_len & 1);
    ((ckSum_t *)s)->sum += sum;
    xTrace2(cksum, TR_DETAILED, "cksum_helper:  sum %x, running sum %x", sum,
	    ((ckSum_t *)s)->sum);
    return TRUE;
  }

u_short
inCkSum( m, buf, len )
    Msg		*m;
    u_short	*buf;
    int		len;
{
    ckSum_t s;
    
    s.odd = 0;
    xTrace1(cksum, TR_DETAILED, "in_cksum:  msg_len = %d", msgLen(m));
    /*  xAssert(len == msg_len(m)); */
    xAssert(! (len % 2));
    s.sum = ocsum(buf, len / 2);
    xTrace1(cksum, TR_EVENTS, "Buf Checksum: %x\n", s.sum);
    msgForEach(m, cksum_helper, &s);
    s.sum = (s.sum & 0xffff) + ((s.sum >> 16) & 0xffff);
    s.sum = (s.sum & 0xffff) + ((s.sum >> 16) & 0xffff);
    xTrace1(cksum, TR_EVENTS, "Total checksum: %x\n", s.sum);
    xAssert(!(s.sum >> 16));
    return ~s.sum & 0xffff;
}
