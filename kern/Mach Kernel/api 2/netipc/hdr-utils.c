/*
 *
 * hdr-utils.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.4 $
 * $Date: 1993/08/12 19:30:03 $
 */

/*
 *
 *  Support functions for converting between different byte orders
 *
 *    This file represents work in progress
 *
 */

#include "xk_debug.h"
#include <mach.h>
#include <mach/message.h>

#define HDRUTILS

#ifdef hp700
#define BE
#else if define mips
#define LE
#endif

void bcopy();

#include "hdr-utils.h"

#ifdef __STDC__

unsigned int nop_1(char *n);
char  nop_8(char *n);
char  twist_8(char *n);
int nop_32(char *n);
int swap_32(char *n);
char unscramble_8(char *n);
short unscramble_16(char *n);
int unscramble_32(char *n);
long unscramble_64(char *n);
short twist_16(char *n);
int twist_32(char *n);
long  twist_64(char *n);
float fnop_32(char *n);
float fswap_32(char *n);
float funscramble_32(char *n);
float ftwist_32(char *n);
mach_msg_type_t nop_msg_type(char *n);
mach_msg_type_t twist_msg_type(char *n);
char  swap_8(char *n);
short swap_16(char *n);
int  swap_32(char *n);
long  swap_64(char *n);
short nop_16(char *n);
long  nop_64(char *n);
float real_error(int n);
int arch_unpermute_index(enum SOURCE_BYTE_ARCH arch_type);
void set_convert_vector(enum SOURCE_BYTE_ARCH arch, Pfv *vec);

#else

unsigned int nop_1();
char  nop_8();
char  twist_8();
int nop_32();
int swap();
char unscramble_8();
short unscramble_16();
int unscramble_32();
long unscramble_64();
short twist_16();
int twist_32();
long  twist_64();
float fnop_32();
float fswap_32();
float funscramble_32();
float ftwist_32();
mach_msg_type_t nop_msg_type();
mach_msg_type_t twist_msg_type();
char swap_8();
short swap_16();
int  swap_32();
short nop_16();
long  nop_64();
float real_error();
int arch_unpermute_index();
void set_convert_vector();

#endif __STDC__


extern int tracemachripcp;

/* general conversions: i.e. irrespective of the endian of
   this machine */
unsigned int   nop_1(n) char *n; { return *n; }
char  nop_8(n)  char *n;  { return *n; }
char  twist_8(n)  char *n;  { return *n & 0xff; }
float fnop_32(n) char *n; { int some = nop_32(n);return *(float *)&some;}
float fswap_32(n) char *n; { int some = swap_32(n);return *(float *)&some;}
float funscramble_32(n) char *n; { int some = unscramble_32(n);return *(float *)&some;}
float ftwist_32(n) char *n; { int some = twist_32(n);return *(float *)&some;}
mach_msg_type_t nop_msg_type (n) char *n; {return *(mach_msg_type_t *)n; }

long  swap_64(n) char *n;  { 
  return 
#ifdef HASLONGLONGS
/* make this have two 32-bit quantities */
    swap_32(n + 8) | swap_32(n)
#else
  0
#endif HASLONGLONGS
    ;
 }

/* if this is a LE machine */
#ifdef LE
int   nop_32(n) char *n; { return ((((((*(n+3) & 0xff)<<8) | (*(n+2) & 0xff)) <<8) | (*(n+1) & 0xff)) << 8) | (*n & 0xff); }
short nop_16(n) char *n; { return ((*(n+1) & 0xff) <<8) | (*n & 0xff); }
long  nop_64(n) char *n;  { return 
#ifdef HASLONGLONGS
     (nop_32(n) | nop_32(n+4) << 32)
#else
     0
#endif
     ;
}

short twist_16(n) char *n; { return (((*n & 0xff) << 8) | (*(n+1) & 0xff)); }

int   twist_32(n) char *n; { return ((((((*n & 0xff) << 8) | (*(n+1) & 0xff))  << 8) | (*(n+2) & 0xff)) << 8) | (*(n+3)& 0xff); }
long  twist_64(n) char *n;  {  return 
#ifdef HASLONGLONGS
   twist_32(n)<<32 | twist_32(n+8)
#else
   0
#endif HASLONGLONGS
   ; 
}

mach_msg_type_t twist_msg_type (n)
char *n;
{
  mach_msg_type_t short_type;
  unsigned int oldbits, newbits, i;
  char *c = (char *)&short_type;

  /* assump: data is coming from a BE machine */

  /* first 2 bytes are fine */
  bcopy (n, c, 2);
  n += 2;
  c += 2;

  /* process number field */
  c[0] = ((n[0] << 4) & 0xf0) | ((n[1] >> 4) & 0xf);
  c[1] = (n[0] >> 4) & 0xf;

  /* process bit fields - they are reversed */
  oldbits = n[1] & 0xf;
  newbits = 0;
  for (i=0; i < 4; i++) {
    newbits <<= 1;
    if (oldbits & 0x1) newbits |= 0x1;
    oldbits >>= 1;
  }
  c[1] |= ((newbits << 4) & 0xf0);
  return short_type;
}
#endif

/* if this is a BE machine */
#ifdef BE
int   nop_32(n) char *n; { return ((((((*n & 0xff)<<8) | (*(n+1)&0xff)) <<8) | (*(n+2)&0xff)) << 8) | (*(n+3) & 0xff); }
short nop_16(n) char *n; { return ((*n & 0xff)<<8) | (*(n+1)&0xff); }
long  nop_64(n) char *n;  { return 
#ifdef HASLONGLONGS
   (nop_32(n) << 32) | nop_32(n+4) 
#else
   0
#endif HASLONGLONGS
   ;
}

short twist_16(n) char *n; { return (((*(n+1) & 0xff) << 8) | (*n & 0xff)); }

int   twist_32(n) char *n; { return ((((((*(n+3) & 0xff) << 8) | (*(n+2) & 0xff))  << 8) | (*(n+1) & 0xff)) << 8) | (*n & 0xff); }
long  twist_64(n) char *n;  {  return 
#ifdef HASLONGLONGS
   (twist_32(n) | twist_32(n+8)<<32)
#else
   0
#endif HASLONGLONGS
   ;
}

mach_msg_type_t twist_msg_type (n)
char *n;
{
  mach_msg_type_t short_type;
  unsigned int oldbits, newbits, i;
  char *c = (char *)&short_type;

  /* assump: data is coming from a LE machine */

  /* first 2 bytes are fine */
  bcopy (n, c, 2);
  n += 2;
  c += 2;

  /* process number field */
  c[0] = ((n[0] >> 4) & 0xf) | ((n[1] << 4) & 0xf0);
  c[1] = (n[0] << 4) & 0xf0;

  /* process bit fields - they are reversed */
  oldbits = n[1] >> 4;
  newbits = 0;
  for (i=0; i < 4; i++) {
    newbits <<= 1;
    if (oldbits & 0x1) newbits |= 0x1;
    oldbits >>= 1;
  }
  c[1] |= (newbits & 0xf);
  return short_type;
}
#endif

int   swap_32(n) char *n; { return ((*n & 0xffff0000) << 16) | (*(n+2) & 0xffff); }
short swap_16(n) char *n; { return ((*n & 0xff) << 8) | (*(n+1) & 0xff); }
char  swap_8(n)  char *n;  { return *n & 0xff; }

long  unscramble_64(n) char *n; { return 
#ifdef HASLONGLONGS
   unscramble_32(n)<<32 | unscramble_32(n+8)
#else
   0
#endif HASLONGLONGS
   ;
}

int   unscramble_32(n) char *n;  { return ((((((*n & 0xff) << 8) | (*(n+1) & 0xff)) << 8) | (*(n+2) & 0xff)) << 8) | (*(n+3)& 0xff); }
short unscramble_16(n) char *n; { return *n; }
char  unscramble_8(n)  char *n;  { return *n & 0xff; }

float real_error(n) int n;
{ 
  xTrace0(machripcp,
	  TR_ALWAYS, "machnetipc: data conversion: cannot convert reals");
  return 0.0;
}

long  (*unpermute_int64[4])() = { nop_64, swap_64, twist_64, unscramble_64 };
int   (*unpermute_int32[4])() = { nop_32, swap_32, twist_32, unscramble_32 };
short (*unpermute_int16[4])() = { nop_16, swap_16, twist_16, unscramble_16 };
float (*unpermute_real32[4])()= { fnop_32, fswap_32, ftwist_32,funscramble_32};
mach_msg_type_t (*unpermute_msg_type[4])() = { nop_msg_type, nop_msg_type,
						 twist_msg_type, nop_msg_type};

int
arch_unpermute_index(arch_type) enum SOURCE_BYTE_ARCH arch_type;
{
  switch (arch_type) {
   case MN_ARCH_MARKER:   return(0);
   case MN_OTHER_ENDIAN:  return(1);
   case MN_WORD_SWAP:     return(2);
   case MN_SCRAMBLE:      return(3);
  }
  xTrace1(machripcp, TR_ERRORS, "machnetipc: arch_unpermute_index: bad arch_type 0x%x", arch_type);
  return 0;
}

/*
 *  set_convert_vector (arch, vec)
 *
 *   Sets up a vector of functions for converting from a given
 *   source architecture to the local architecture.
 *   The resulting vector is indexed by the data type ---
 *      
 *     vec[MACH_MSG_TYPE_INTEGER_16] is a function to convert
 *       from an incoming representation of a 16-bit integer
 *       to a local integer.
 */
void
set_convert_vector( arch, vec )
     enum SOURCE_BYTE_ARCH arch;
     Pfv *vec;
{
  int archin = arch_unpermute_index(arch);

  vec[0] = (Pfv)nop_8;
  vec[1] = (Pfv)unpermute_int16[archin];
  vec[2] = (Pfv)unpermute_int32[archin];
  vec[3] = (Pfv)nop_8;
  vec[4] = (Pfv)nop_8;
  vec[5] = (Pfv)nop_8;
  vec[8] = (Pfv)unpermute_real32[archin];
  vec[9] = (Pfv)nop_8;
  vec[10] = (Pfv)nop_8;
  vec[11] = (Pfv)nop_8;
  vec[12] = (Pfv)nop_8;
}

