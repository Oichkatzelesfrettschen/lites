/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)clnp_debug.c	8.1 (Berkeley) 6/10/93
 */

/***********************************************************
		Copyright IBM Corporation 1987

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * ARGO Project, Computer Sciences Dept., University of Wisconsin - Madison
 */
/* $Header: /n/fast/usr/lsrc/mach/CVS/lites/server/netiso/clnp_debug.c,v 1.1.1.1 1995/03/02 21:49:57 mike Exp $ */
/* $Source: /n/fast/usr/lsrc/mach/CVS/lites/server/netiso/clnp_debug.c,v $ */

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/errno.h>

#include <net/if.h>
#include <net/route.h>

#include <netiso/iso.h>
#include <netiso/clnp.h>
#include <netiso/clnp_stat.h>
#include <netiso/argo_debug.h>
#include "simd.h"

#ifdef	ARGO_DEBUG

#ifdef	TESTDEBUG
#ifdef notdef
struct addr_37 u_37 = {
	{0x00, 0x02, 0x00, 0x10, 0x20, 0x30, 0x35}, 
	{0x01, 0x02, 0x03, 0x04, 0x50, 0x60, 0x70, 0x80, 0x90}
};
struct addr_osinet u_osinet = {
	{0x00, 0x04},
	{0x00, 0x02, 0x00, 0x01, 0x23, 0x42, 0x78, 0x20, 0x01, 0x05, 0x00}
};
#endif /* notdef */
struct addr_rfc986 u_rfc986 = {
	{0x00, 0x06},
	{0x01, 0xc0, 0x0c, 0x0c, 0xab, 0x11}
};
struct addr_rfc986 u_bad = {
	{0x00, 0x01},
	{0x01, 0xc0, 0x0c, 0x0c, 0xab, 0x11}
};
#include <stdio.h>
main()
{
	struct iso_addr	a;

	a.isoa_afi = AFI_37;
	a.isoa_u.addr_37 = u_37;
	a.isoa_len = 17;
	printf("type 37: %s\n", clnp_iso_addrp(&a));

	a.isoa_afi = AFI_OSINET;
	a.isoa_u.addr_osinet = u_osinet;
	a.isoa_len = 14;
	printf("type osinet: %s\n", clnp_iso_addrp(&a));

	a.isoa_afi = AFI_RFC986;
	a.isoa_u.addr_rfc986 = u_rfc986;
	a.isoa_len = 9;
	printf("type rfc986: %s\n", clnp_iso_addrp(&a));

	a.isoa_afi = 12;
	a.isoa_u.addr_rfc986 = u_rfc986;
	a.isoa_len = 9;
	printf("type bad afi: %s\n", clnp_iso_addrp(&a));

	a.isoa_afi = AFI_RFC986;
	a.isoa_u.addr_rfc986 = u_bad;
	a.isoa_len = 9;
	printf("type bad idi: %s\n", clnp_iso_addrp(&a));
}
#endif	/* TESTDEBUG */

unsigned int	clnp_debug;
static char letters[] = "0123456789abcdef";

/*
 *	Print buffer in hex, return addr of where we left off.
 *	Do not null terminate.
 */
char *
clnp_hexp(char *src, int len, char *where)
{
#if LITES_HAVE_SSE
    static const char lut[16] = "0123456789abcdef";
    while (len >= 16) {
        __m128i bytes = _mm_loadu_si128((const __m128i *)src);
        __m128i high = _mm_and_si128(_mm_srli_epi16(bytes, 4), _mm_set1_epi8(0x0f));
        __m128i low = _mm_and_si128(bytes, _mm_set1_epi8(0x0f));
        __m128i table = _mm_loadu_si128((const __m128i *)lut);
        __m128i high_chars = _mm_shuffle_epi8(table, high);
        __m128i low_chars = _mm_shuffle_epi8(table, low);
        __m128i lo = _mm_unpacklo_epi8(high_chars, low_chars);
        __m128i hi = _mm_unpackhi_epi8(high_chars, low_chars);
        _mm_storeu_si128((__m128i *)where, lo);
        _mm_storeu_si128((__m128i *)(where + 16), hi);
        src += 16;
        where += 32;
        len -= 16;
    }
#elif LITES_HAVE_NEON
    static const uint8_t lut[16] = "0123456789abcdef";
    while (len >= 16) {
        uint8x16_t bytes = vld1q_u8((const uint8_t *)src);
        uint8x16_t high = vandq_u8(vshrq_n_u8(bytes, 4), vdupq_n_u8(0x0f));
        uint8x16_t low = vandq_u8(bytes, vdupq_n_u8(0x0f));
        uint8x16_t table = vld1q_u8(lut);
        uint8x16_t high_chars = vqtbl1q_u8(table, high);
        uint8x16_t low_chars = vqtbl1q_u8(table, low);
        uint8x16x2_t z = vzipq_u8(high_chars, low_chars);
        vst1q_u8((uint8_t *)where, z.val[0]);
        vst1q_u8((uint8_t *)(where + 16), z.val[1]);
        src += 16;
        where += 32;
        len -= 16;
    }
#endif
    for (int i = 0; i < len; i++) {
        int j = ((u_char *)src)[i];
        *where++ = letters[j >> 4];
        *where++ = letters[j & 0x0f];
    }
    return where;
}


/*
 *	Return a ptr to a human readable form of an iso addr 
 */
static char iso_addr_b[50];
#define	DELIM	'.';

char *
clnp_iso_addrp(isoa)
struct iso_addr *isoa;
{
	char	*cp;

	/* print length */
	sprintf(iso_addr_b, "[%d] ", isoa->isoa_len);

	/* set cp to end of what we have */
	cp = iso_addr_b;
	while (*cp)
		cp++;

	/* print afi */
	cp = clnp_hexp(isoa->isoa_genaddr, (int)isoa->isoa_len, cp);
#ifdef notdef
	*cp++ = DELIM;

	/* print type specific part */
	switch(isoa->isoa_afi) {
		case AFI_37:
			cp = clnp_hexp(isoa->t37_idi, ADDR37_IDI_LEN, cp);
			*cp++ = DELIM;
			cp = clnp_hexp(isoa->t37_dsp, ADDR37_DSP_LEN, cp);
			break;
		
/* 		case AFI_OSINET:*/
		case AFI_RFC986: {
			u_short	idi;

			/* osinet and rfc986 have idi in the same place */
			/* print idi */
			cp = clnp_hexp(isoa->rfc986_idi, ADDROSINET_IDI_LEN, cp);
			*cp++ = DELIM;
			CTOH(isoa->rfc986_idi[0], isoa->rfc986_idi[1], idi);

			if (idi == IDI_OSINET) {
				struct ovl_osinet *oosi = (struct ovl_osinet *)isoa;
				cp = clnp_hexp(oosi->oosi_orgid, OVLOSINET_ORGID_LEN, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(oosi->oosi_snetid, OVLOSINET_SNETID_LEN, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(oosi->oosi_snpa, OVLOSINET_SNPA_LEN, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(oosi->oosi_nsap, OVLOSINET_NSAP_LEN, cp);
			} else if (idi == IDI_RFC986) {
				struct ovl_rfc986 *o986 = (struct ovl_rfc986 *)isoa;
				cp = clnp_hexp(&o986->o986_vers, 1, cp);
				*cp++ = DELIM;
#ifdef  vax
				sprintf(cp, "%d.%d.%d.%d.%d", 
				o986->o986_inetaddr[0] & 0xff,
				o986->o986_inetaddr[1] & 0xff,
				o986->o986_inetaddr[2] & 0xff,
				o986->o986_inetaddr[3] & 0xff,
				o986->o986_upid & 0xff);
				return(iso_addr_b);
#else
				cp = clnp_hexp(&o986->o986_inetaddr[0], 1, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(&o986->o986_inetaddr[1], 1, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(&o986->o986_inetaddr[2], 1, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(&o986->o986_inetaddr[3], 1, cp);
				*cp++ = DELIM;
				cp = clnp_hexp(&o986->o986_upid, 1, cp);
#endif /* vax */
			}
			
		} break;

		default:
			*cp++ = '?';
			break;
	}
#endif /* notdef */
	*cp = (char)0;
	
	return(iso_addr_b);
}

char *
clnp_saddr_isop(s)
register struct sockaddr_iso *s;
{
	register char	*cp = clnp_iso_addrp(&s->siso_addr);

	while (*cp) cp++;
	*cp++ = '(';
	cp = clnp_hexp(TSEL(s), (int)s->siso_tlen, cp);
	*cp++ = ')';
	*cp++ = 0;
	return (iso_addr_b);
}

#endif	/* ARGO_DEBUG */
