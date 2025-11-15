/*
 * Copyright (c) 1982, 1986, 1988, 1993
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
 *	@(#)mbuf.h	8.5 (Berkeley) 2/19/95
 */

#ifndef _SYS_MBUF_H_
#define _SYS_MBUF_H_

#include <sys/types.h>

/*
 * Mbufs are used to store network data.
 *
 * An mbuf is a small buffer (typically 128 bytes) that can be chained
 * together to form larger buffers. The mbuf header contains management
 * information and a small data area. Large data can be stored in external
 * clusters referenced by the mbuf.
 */

/* Mbuf flags */
#define M_EXT       0x0001  /* has associated external storage */
#define M_PKTHDR    0x0002  /* start of record */
#define M_EOR       0x0004  /* end of record */
#define M_PROTO1    0x0008  /* protocol-specific */
#define M_PROTO2    0x0010  /* protocol-specific */
#define M_PROTO3    0x0020  /* protocol-specific */
#define M_PROTO4    0x0040  /* protocol-specific */
#define M_PROTO5    0x0080  /* protocol-specific */
#define M_BCAST     0x0100  /* send/received as link-level broadcast */
#define M_MCAST     0x0200  /* send/received as link-level multicast */

/* Mbuf types */
#define MT_FREE     0   /* should be on free list */
#define MT_DATA     1   /* dynamic (data) allocation */
#define MT_HEADER   2   /* packet header */
#define MT_SOCKET   3   /* socket structure */
#define MT_PCB      4   /* protocol control block */
#define MT_RTABLE   5   /* routing tables */
#define MT_HTABLE   6   /* IMP host tables */
#define MT_ATABLE   7   /* address resolution tables */
#define MT_SONAME   8   /* socket name */
#define MT_SOOPTS   10  /* socket options */
#define MT_FTABLE   11  /* fragment reassembly header */
#define MT_RIGHTS   12  /* access rights */
#define MT_IFADDR   13  /* interface address */
#define MT_CONTROL  14  /* extra-data protocol message */
#define MT_OOBDATA  15  /* expedited data  */

/* Size constants */
#define MSIZE       128                 /* size of an mbuf */
#define MLEN        (MSIZE - sizeof(struct m_hdr))  /* normal data len */
#define MHLEN       (MLEN - sizeof(struct pkthdr))  /* data len w/pkthdr */

#define MINCLSIZE   (MHLEN + MLEN)      /* smallest amount to put in cluster */
#define M_MAXCOMPRESS   (MHLEN / 2)     /* max amount to copy for compression */

/* Cluster sizes */
#define MCLBYTES    2048                /* size of an mbuf cluster */
#define MCLSHIFT    11                  /* log2(MCLBYTES) */
#define MCLOFSET    (MCLBYTES - 1)      /* offset within an mbuf cluster */

/*
 * Macros for type conversion
 */
#define MGET(m, how, type) \
    { (m) = m_get((how), (type)); }

#define MGETHDR(m, how, type) \
    { (m) = m_gethdr((how), (type)); }

#define MFREE(m, n) \
    { (n) = m_free(m); }

#define MCLGET(m, how) \
    { (void)m_clalloc(1, (how)); \
      if ((m)->m_flags & M_EXT) \
          ; \
      else \
          (m)->m_data = (m)->m_ext.ext_buf; \
    }

/*
 * Set the m_data pointer of a newly-allocated mbuf (m_get/MGET) to place
 * an object of the specified size at the end of the mbuf, longword aligned.
 */
#define M_ALIGN(m, len) \
    { (m)->m_data += (MLEN - (len)) & ~(sizeof(long) - 1); }

/*
 * As above, for mbufs allocated with m_gethdr/MGETHDR
 * or initialized by M_COPY_PKTHDR.
 */
#define MH_ALIGN(m, len) \
    { (m)->m_data += (MHLEN - (len)) & ~(sizeof(long) - 1); }

/*
 * Compute the amount of space available before the current start of data.
 * The m_ext.ext_buf case is taken care of in m_leadingspace().
 */
#define M_LEADINGSPACE(m) \
    ((m)->m_flags & M_EXT ? 0 : \
     (m)->m_flags & M_PKTHDR ? (m)->m_data - (m)->m_pktdat : \
     (m)->m_data - (m)->m_dat)

/*
 * Compute the amount of space available after the end of data in an mbuf.
 */
#define M_TRAILINGSPACE(m) \
    ((m)->m_flags & M_EXT ? (m)->m_ext.ext_buf + (m)->m_ext.ext_size - \
     ((m)->m_data + (m)->m_len) : \
     &(m)->m_dat[MLEN] - ((m)->m_data + (m)->m_len))

/*
 * Arrange to prepend space of size plen to mbuf m.
 */
#define M_PREPEND(m, plen, how) \
    { if (M_LEADINGSPACE(m) >= (plen)) { \
          (m)->m_data -= (plen); \
          (m)->m_len += (plen); \
      } else \
          (m) = m_prepend((m), (plen), (how)); \
      if ((m) && (m)->m_flags & M_PKTHDR) \
          (m)->m_pkthdr.len += (plen); }

/* Length to m_copy to copy all */
#define M_COPYALL   1000000000

/* Packet header structure */
struct pkthdr {
    struct ifnet *rcvif;        /* rcv interface */
    int     len;                /* total packet length */
};

/* External storage structure */
struct m_ext {
    caddr_t ext_buf;            /* start of buffer */
    void    (*ext_free)(caddr_t, u_int);  /* free routine if not NULL */
    u_int   ext_size;           /* size of buffer, for ext_free */
};

/* Mbuf header */
struct m_hdr {
    struct mbuf *mh_next;       /* next buffer in chain */
    struct mbuf *mh_nextpkt;    /* next chain in queue/record */
    int     mh_len;             /* amount of data in this mbuf */
    caddr_t mh_data;            /* location of data */
    short   mh_type;            /* type of data */
    short   mh_flags;           /* flags; see below */
};

/* Mbuf structure */
struct mbuf {
    struct m_hdr m_hdr;
    union {
        struct {
            struct pkthdr MH_pkthdr;    /* M_PKTHDR set */
            union {
                struct m_ext MH_ext;    /* M_EXT set */
                char    MH_databuf[MHLEN];
            } MH_dat;
        } MH;
        char    M_databuf[MLEN];        /* !M_PKTHDR, !M_EXT */
    } M_dat;
};

/* Mbuf field access macros */
#define m_next      m_hdr.mh_next
#define m_len       m_hdr.mh_len
#define m_data      m_hdr.mh_data
#define m_type      m_hdr.mh_type
#define m_flags     m_hdr.mh_flags
#define m_nextpkt   m_hdr.mh_nextpkt
#define m_act       m_nextpkt
#define m_pkthdr    M_dat.MH.MH_pkthdr
#define m_ext       M_dat.MH.MH_dat.MH_ext
#define m_pktdat    M_dat.MH.MH_dat.MH_databuf
#define m_dat       M_dat.M_databuf

/* Mbuf statistics */
struct mbstat {
    u_long  m_mbufs;    /* mbufs obtained from page pool */
    u_long  m_clusters; /* clusters obtained from page pool */
    u_long  m_spare;    /* spare field */
    u_long  m_clfree;   /* free clusters */
    u_long  m_drops;    /* times failed to find space */
    u_long  m_wait;     /* times waited for space */
    u_long  m_drain;    /* times drained protocols for space */
    u_short m_mtypes[256]; /* type specific mbuf allocations */
};

#ifdef KERNEL
/* Flags to m_get/MGET */
#define M_DONTWAIT  1
#define M_WAIT      0

/* Function prototypes */
struct mbuf *m_get(int how, int type);
struct mbuf *m_gethdr(int how, int type);
struct mbuf *m_free(struct mbuf *m);
void        m_freem(struct mbuf *m);
struct mbuf *m_copy(struct mbuf *m, int off, int len);
struct mbuf *m_copym(struct mbuf *m, int off, int len, int wait);
struct mbuf *m_pullup(struct mbuf *m, int len);
struct mbuf *m_prepend(struct mbuf *m, int len, int how);
void        m_copydata(struct mbuf *m, int off, int len, caddr_t cp);
void        m_cat(struct mbuf *m, struct mbuf *n);
void        m_adj(struct mbuf *mp, int req_len);
int         m_clalloc(int ncl, int how);
struct mbuf *m_split(struct mbuf *m, int len, int wait);

/* Global variables */
extern struct mbuf *mbutl;      /* virtual address of mclusters */
extern char *mclrefcnt;         /* cluster reference counts */
extern struct mbstat mbstat;    /* mbuf statistics */
extern int nmbclusters;         /* number of mapped clusters */
extern int nmbufs;              /* number of mbufs */
extern int mbtypes[];           /* type specific mbuf allocations */

#endif /* KERNEL */

#endif /* !_SYS_MBUF_H_ */
