/* inter-convert floating point numbers */



#define MAX_FLOAT_SIZE 16  /* largest number of bytes in any float */
#define MAX_FLOAT_SIZE_R8 (((MAX_FLOAT_SIZE+7)/8)*8)

typedef enum
  { IEEE1=1, IEEE1X, IEEE2, IEEE2X, IEEE4, VAXF, VAXD, VAXG, VAXH, MC68881_2X, PDP6, IBM360 }
  float_typename;

#define TWOS 1		/* negative numbers represented as 2s complement */
#define NAN_MZ 2	/* NANs represented as exponent 0 w/ minus sign  */
#define NAN_XMAX 4	/* NANs marked as maximum exponent               */
#define INF 8		/* representation has an infinity                */
#define MINF 0x10	/* representation has separate -infinity         */
#define Z_X0 0x20	/* exponent of 0 represents 0                    */
#define REVERSE 0x40	/* reverse all the bytes                         */
#define SWAPC 0x80	/* swap characters                               */
#define SWAPS 0x100	/* swap shorts                                   */
#define SWAPL 0x200	/* swap longs                                    */
#define SWAPD 0x400	/* swap doubles                                  */
#define HIDDEN 0x1000	/* there's a hidden bit                          */
#define DEN 0x2000	/* representation uses denormalized numbers      */
#define XM4 0x4000	/* exponent is a multiple of 4                   */
#define REORDER_FLAGS (REVERSE+SWAPC+SWAPS+SWAPL+SWAPD)
#define IEEE_FLAGS (NAN_XMAX+INF+MINF+HIDDEN+DEN)
#define VAX_FLAGS (NAN_MZ+Z_X0+SWAPC+HIDDEN)
/* $$$$$ worry about quiet & noisy NANs */
/* $$$$$ make sure my writeup always capitalizes NAN */
/* $$$$$ have a force -0 -> 0 flag? a force -inf -> inf flag? */

static struct float_conv_table
 { int flags, xsize, xbias, fsize, hidden,bits,bsize; long hmask,lmask,lbmask; int bzsize,bzbeg; int (*special)();
   struct { /*int any,c,s,l,d,n;*/ void (*special1)(), (*special2)();} reorder;
  }
  float_conv_table_array[] =
                { {0},
/* IEEE1      */  {IEEE_FLAGS,  8,    127, 23, 23, 32, 4,0x7FFFFF, 0},
/* IEEE1X     */  {0},
/* IEEE2      */  {IEEE_FLAGS, 11,   1023, 52, 20, 64, 8, 0xFFFFF,-1},
/* IEEE2X     */  {0},
/* VAXF       */  {VAX_FLAGS,   8,    128, 23, 23, 32, 4,0x7FFFFF, 0},
/* VAXD       */  {VAX_FLAGS,   8,    128, 55, 23, 64, 8,0x7FFFFF,-1},
/* VAXG       */  {VAX_FLAGS,  11,   1024, 52, 20, 64, 8, 0xFFFFF,-1},
/* VAXH       */  {VAX_FLAGS,  15,  16384,112, 16,128,16,  0xFFFF,-1},
/* MC68881_2X */  {IEEE_FLAGS-HIDDEN,
                               15,  16383, 64,  0, 96,12,       0,-1},  /* 16bits of 0 between exp & frac */
/* PDP6       */  {TWOS,        8,    128, 27,  0, 36, 5,0x7FFFFF,0xF0000000,0xF0},
/* IBM360     */  {XM4,         7,    256, 24,  0, 32, 4,0xFFFFFF, 0}
 };

/* for IEEE: maxX = infinity (0 frac) Nan (~0 frac) Nanhigh=1 quiet, 0 noisy*/
/* avoid creating VAX -0; translate Nans */
/* add mask for twos complement, 36bits? */
/* 80287 seems to not have -infinity */

static int local_arch_reorder = 0;  /* add up reordering flags for architecture */
/*_c=0, local_arch_reorder_s=0,
           local_arch_reorder_l=0, local_arch_reorder_d=0,
           local_arch_reorder_n=0; */
static void(*local_arch_reorder_f)()=0;

typedef struct
 { long s, x; unsigned long f1, f2, f3, f4; int nan, inf, zero, biased, bits; float_typename kind;}
  universal_float;
/* fields are sign (0 or -1); exponent; 4 words of 32 bits each for the fraction,
   with f1 being most significant; flags for Nan and infinity;
   and the type of the input number.
   The value of an ordinary number is sign * 2^x * fraction.
   sign is -1 if s<0, else 1.
   fraction is 128 bits; the high bit of f1 has value 1; the low bit of f4
   has value 2^-127.
   flags are NAN, infinity, force-to-zero-regradless-of-frac&exp,
   and no exponent bias subtracted.
   bits is the number of input bits; used in right-norming NAN data.
 */

static universal_float uf_static, uf_static2;

static union float_temporary
{ unsigned char c[MAX_FLOAT_SIZE_R8];
  unsigned short s[MAX_FLOAT_SIZE_R8/2];
  unsigned long l[MAX_FLOAT_SIZE_R8/4];}
 ftmp;

void twos_complement(a,n) unsigned char a[]; int n;
{ int c=1;  for (n--;n>=0;n--) { a[n] = c + ~a[n]; if (a[n]) c=0; }; }

/* f is unchanged */
float2universal( f, fkind, uf)
unsigned char *f; float_typename fkind; universal_float *uf;
{ struct float_conv_table *fcv = &float_conv_table_array[fkind];int i,reorder;unsigned long tmp;
printf("entered f2u %d %d %d\n",f,fkind,uf);
printf("fcv: fkind %d  fcv %d  xsize %d  fsize %d  hd %d  bsize %d  bzsize %d  bzbeg %d\n",
  fkind, fcv, fcv->xsize, fcv->fsize, fcv->hidden, fcv->bsize, fcv->bzsize, fcv->bzbeg);
printf("flags: %x\n", fcv->flags);
if (fcv->bsize==4) printf("input %.8f  ",*(float *)f);
else printf("input %.17f  ",*(double *)f);
for (i=0;i<fcv->bsize;i++) printf(" %02x",f[i]); printf("\n");
  if (!uf) uf = &uf_static;
  uf->s = uf->x = uf->f1 = uf->f2 = uf->f3 = uf->f4 = 0;
  uf->nan = uf->inf = uf->zero = uf->biased = 0; uf->bits = fcv->bits; uf->kind = fkind;
  if (fcv->special) return fcv->special(f,fkind,uf,0);
  if (fcv->bzsize) bzero(&ftmp.c[fcv->bzbeg],fcv->bzsize);/*usually don't need this */
  bcopy(f,&ftmp,fcv->bsize);
printf("ftmp:"); for (i=0;i<MAX_FLOAT_SIZE_R8;i++) printf(" %02x",ftmp.c[i]); printf("\n");
/*$$$$ conflict between reorder.any & localarchreorder; */
/* distinguishing reordering due to the local architecture, and due to the number */
/*  if (fcv->reorder.any) */
  { /* check for special case reorder */
    if (local_arch_reorder_f) local_arch_reorder_f(ftmp,fkind,fcv,0);
    if (fcv->reorder.special1) fcv->reorder.special1(ftmp,fkind,fcv,0);
    reorder = local_arch_reorder ^ (fcv->flags & REORDER_FLAGS);
    if (reorder & REVERSE)
  for (i=0;2*i+1<fcv->bsize;i++)
  { tmp = ftmp.c[i]; ftmp.c[i] = ftmp.c[fcv->bsize-1-i]; ftmp.c[fcv->bsize-1-i] = tmp; };
    if (reorder & SWAPC)
  for (i=0;i<fcv->bsize;i+=2)
  { tmp = ftmp.c[i]; ftmp.c[i] = ftmp.c[i+1]; ftmp.c[i+1] = tmp; };
    if (reorder & SWAPS)
  for (i=0;2*i<fcv->bsize;i+=2)
  { tmp = ftmp.s[i]; ftmp.s[i] = ftmp.s[i+1]; ftmp.s[i+1] = tmp; };
    if (reorder & SWAPL)
  for (i=0;4*i<fcv->bsize;i+=2)
  { tmp = ftmp.l[i]; ftmp.l[i] = ftmp.l[i+1]; ftmp.l[i+1] = tmp; };
    if (reorder & SWAPD)
  for (i=0;4*i<fcv->bsize;i+=4)
  { tmp = ftmp.l[i]; ftmp.l[i] = ftmp.l[i+2]; ftmp.l[i+2] = tmp;
    tmp = ftmp.l[i+1]; ftmp.l[i+1] = ftmp.l[i+3]; ftmp.l[i+3] = tmp; };
    if (fcv->reorder.special2) fcv->reorder.special2(ftmp,fkind,fcv,0);
  };
printf("ftmp:"); for (i=0;i<MAX_FLOAT_SIZE_R8;i++) printf(" %02x",ftmp.c[i]); printf("\n");
  if (ftmp.l[0] & 0x80000000)
  { uf->s = -1;
    if (fcv->flags & TWOS) { if (fcv->lbmask) ftmp.c[fcv->bsize-1] &= fcv->lbmask;
			     twos_complement(ftmp.c,fcv->bsize); }; };
  uf->x = (ftmp.l[0] & 0x7fffffff) >> (31 - fcv->xsize);
  uf->f1 = ftmp.l[0] & fcv->hmask;  /* was (0x7fffffff >> fcv->xsize) */
  if (fcv->bsize>4) { uf->f2 = ftmp.l[1];
		    if (fcv->bsize>8) { uf->f3 = ftmp.l[2];
				      if (fcv->bsize>12) uf->f4 = ftmp.l[3] & fcv->lmask;
				      else uf->f3 &= fcv->lmask; }
		    else uf->f2 &= fcv->lmask;};
 /*  else uf->f1 &= fcv->lmask; don't need this, it's subsumed in hmask */
 /* trouble in river city: my intended universal format is int*2^exp
  unfortunately, int may occupy four words; I have to decide if single
  precision is 0,0,0,data or data,0,0,0.  This interacts with
  information about normalization of the input; setting the hidden bit;
  extending the precision; and rounding when decreasing the precision.
  and two's complementing.  Check spelling of complement. *i*?
  maybe record "number of valid words"? or, "in-format"?
  denormalized single will fit in double; den. double will fit in extended;
  Where is the binary point?  How does it interact with the exponent,
  and with using emin/emax to make denorms & infinities.
  Think about this a bit. */
/* leaning toward "number of valid bytes" & leaving everything Ljust */
/* then, would shift when converting Nanstuff. */
/* correct strategy: if we find a number with unmeaningful mantissa,
   use shift to right position it.  then shift it back for out-conversion $$$$*/
  if (fcv->flags & (INF|NAN_XMAX) && uf->x == (1<<fcv->xsize)-1) /* max expt */
  { /* if both flags on, must discriminate on basis of 0 fraction */
    if (!(fcv->flags & INF)) uf->nan = 1;
    else if (!(fcv->flags & NAN_XMAX)) uf->inf = 1;
    else if (uf->f1 || uf->f2 || uf->f3 || uf->f4) uf->nan = 1; else uf->inf = 1;
    uf->biased = 1;
    goto bias; };
/*$$$$$ projective infinity will lose bogus -inf on output
    if inf & minus & not minf don't convert in, but convert out */
/*$$$$$ should NAN or 0 or INF mean that exponent is not debiased? */
/* would like NAN to propagate "useless" fraction bits RIGHT normalized;
   but want to preserve quiet/noisy bit.
   If a NAN is in danger of having a 0 fraction, set it to all 1s.
   ? preserve the useless fraction bits of VAX 0s?
   $$$$$*/
  if (fcv->flags & (NAN_MZ|Z_X0) && !uf->x) 
  { if (fcv->flags & NAN_MZ && uf->s < 0) uf->nan = 1;
    else if (fcv->flags & Z_X0) uf->zero = 1;
    uf->biased = 1;
    goto bias; };
/* $$$$$
   Nan flag, noisy/quiet; zero; denormalization flag?; infinity
   did 8087 chip really do projective infinity?
   ? use rounding mode?
   resolve round-to-infinity question with hugenum+epsilon? I think
    this behaves as if infinity had an even bit in the round-to-nearest case,
    and as if infinity were hugenum+2epsilon.  Otherwise, round to nearest
    would always round down, since all finite numbers are closer to hugenum
    than to infinity.
*/
  if (fcv->flags & XM4) uf->x *= 4;
  uf->x -= fcv->xbias; /*$$$$$ may be different for denorms */
  uf->x += fcv->xsize;
  if (fcv->flags & HIDDEN)  uf->f1 |= 1<<fcv->hidden;
/*getting very slow!
*/
bias: ;
}

void print_universal_float(uf)
universal_float *uf;
{ if (!uf) uf = &uf_static;
  printf("sign %d  exponent %ld  fraction %08lx %08lx %08lx %08lx  kind %d",
	 uf->s, uf->x, uf->f1, uf->f2, uf->f3, uf->f4, uf->kind); }

copy_universal_float(ufrom,uto)
universal_float *ufrom, *uto;
{ if (!ufrom) ufrom = &uf_static; if (!uto) uto = &uf_static2;
   bcopy(ufrom,uto,sizeof(universal_float)); }

/* find number of leading zero bits in x.
   return -1 for 0, and 0 if sign bit is on */
int find_first_one(x) long x;
{ int n; if (!x) return -1; for(n=0;x>0;n++,x<<=1); return n; }

/* return -1 if uf contains 0 fraction */
int find_first_one_uf(uf) universal_float *uf;
{ if (uf->f1) return find_first_one(uf->f1);
  else if (uf->f2) return 32+find_first_one(uf->f2);
  else if (uf->f3) return 64+find_first_one(uf->f3);
  else if (uf->f4) return 96+find_first_one(uf->f4);
  return -1; }

/* return (4 if least kept bit is 1) + (2 if first dropped bit is 1)
   + (1 if any other dropped bit is 1)
   n is position of kept-bit, 0 for high bit of f1, 127 for low bit of f4 */
int rounding_data(uf,n) universal_float *uf; int n;
{  unsigned long sticky=0,hi,lo;

  if (n>=128) return 0;
  else if (n>=96) { n-=96; hi=uf->f4; lo=0; }
  else if (n>=64) { n-=64; hi=uf->f3; lo=uf->f4; }
  else { sticky = uf->f4;
	 if (n>=32) { n-=32; hi=uf->f2; lo=uf->f3; }
	 else { sticky |= uf->f3;
		if (n>=0) { hi=uf->f1; lo=uf->f2; }
		else { sticky |= uf->f2; n+=32; hi=0; lo=uf->f1; }; }; };
  if (n=31) return (hi&1)<<2 + (lo&0x80000000)?2:0 + (sticky || lo<<1);
  sticky |= lo;
  if (n<0) return sticky?1:0;  /* check for n<0; if so, only sticky matters. */
  sticky |= hi & (0x3fffffff>>n);
  return (6 & (hi>>(30-n))<<1) + sticky?1:0;
 }

/* Rounding modes:
   The IEEE standard modes are RTNEVEN (the default case), RTZERO, RTPINF, & RTMINF.
   In all modes, if the Round & Sticky bits are 0, the value is unchanged.
   RTZERO  discard low order bits, rounding values toward 0.  Also called "truncate".
   RTINF   round toward infinity (away from 0).
   RTPINF  round toward +infinity
   RTMINF  round toward -infinity
   All RTN modes are Round-To-Nearest:  If the Sticky bit is 1, then the Round bit
   controls whether to increment.  The RTN modes differ in their handling of the exact
   midpoint case, when Round=1, Sticky=0.
   RTNEVEN default IEEE case.  Increment iff LSB is 1.  (Forces LSB=0.)
   RTNODD  Increment if LSB is 0.
   RTNZERO discard exact half
   RTNINF  increment.  This is the way we learned in school, where 3.50 -> 4.
   RTNPINF round half toward +infinity
   RTNMINF round half toward -infinity
The nonRTN modes are safe under double rounding (when the second rounding drops
more bits), in the sense that the pair of roundings produces the same answer as
the second rounding alone.
All modes except RTZERO might increment the exponent, conceivably forcing a
second rounding in some algorithms, or affecting the decision on whether a
number should be made infinite, denormalized, or forced to 0.
*/
typedef enum
  { RTZERO=1, RTINF, RTPINF, RTMINF, RTNEVEN, RTNODD, RTNZERO, RTNINF, RTNPINF, RTNMINF }
  rounding_mode;
#define RTN RTNEVEN  /* separates RTN modes from nonRTN modes */
static rounding_mode Network_Conversion_Rounding_Mode = RTNEVEN;

/*
round?
  rt0: truncate bits
  rt+inf: if sign<0, truncate bits
	 else, inspect shifted area for 1s, & increment after shifting if found
  rt-inf: same, with condition backward
  rt-n: if 1st shifted bit is 0, truncate
	else, if 1 & last unshifted bit is 1, increment
        else, inspect shifted area for 1s, & increment if found
There are two other rounding modes like round-to-nearest:
  they differ in treatment of 1st shifted bit=1, rest 0:  up, and down.
  might also add rtinf ( = round away from 0), rtodd (where the halfway point
  is rounded to the odd value).  and rt+,rt- where the midpoint value is
  rounded toward + or - infinity.
So overall, ten rounding modes.
*/

/* return 0 for do-nothing, 1 to increment the LSB */
int decide_rounding(mode,sign,data)
  rounding_mode mode; int sign,data;
{ if (data & 3 == 0) return 0;  /* exact number, don't need to round */
  if (mode < RTN) return ( mode == RTINF || (mode == RTPINF && sign>=0) || (mode == RTMINF && sign<=0));
  if (data & 1) return (data & 2)>>1;  /* all RTN modes */
  /* only exact midpoint case remains */
  return (mode == RTNINF || (mode == RTNPINF && sign>=0) || (mode == RTNMINF && sign<=0)
 || (mode == RTNEVEN && (data & 4)) || (mode == RTNODD && (data & 4 == 0)));
}  /* move default case RTNEVEN closer to front to return faster $$$$$ */


/* $$$$$ check that I don't actually do negative << or >> */
/* n may be negative; return true if any 1 bits shifted off left end */
int left_shift_universal_float(uf,n) universal_float *uf; int n;
{ int val=0;
  if (n>0)
  { if (n>=128) { if ( uf->f1 || uf->f2 || uf->f3 || uf->f4 ) val = 1;
		  uf->f1 = uf->f2 = uf->f3 = uf->f4 = 0; return val; };
  l:if (n>=32) { if (uf->f1) val = 1;
		 uf->f1 = uf->f2; uf->f2 = uf->f3; uf->f3 = uf->f4; uf->f4 = 0;
		 n-=32; goto l; }
    if (!n) return val;
    if (n>0) { if ( uf->f1 & (0xffffffff << (32-n))) val = 1;
	       uf->f1 = (uf->f1 << n) | (uf->f2 >> (32-n));
	       uf->f2 = (uf->f2 << n) | (uf->f3 >> (32-n));
	       uf->f3 = (uf->f3 << n) | (uf->f4 >> (32-n));
	       uf->f4 = (uf->f4 << n); return val; }; }
  if (n<=-128) { uf->f1 = uf->f2 = uf->f3 = uf->f4 = 0; return 0; };
r:if (n<=-32) { uf->f4 = uf->f3; uf->f3 = uf->f2; uf->f2 = uf->f1; uf->f1 = 0;
		n+=32; goto r; };
  /*$$$$ check that >> shifts in 0s; Cbook says "yes, on unsigned" */
  if (!n) return 0;
  uf->f4 = (uf->f4 >> (-n)) | (uf->f3 << (32+n));
  uf->f3 = (uf->f3 >> (-n)) | (uf->f2 << (32+n));
  uf->f2 = (uf->f2 >> (-n)) | (uf->f1 << (32+n));
  uf->f1 = (uf->f1 >> (-n));
  return 0; }

int universal_float_bit(uf,n) universal_float *uf; int n;
{ if (n<0 || n>127) return 0;
  if (n<32) return (uf->f1>>(31-n)) & 1;
  if (n<64) return (uf->f2>>(63-n)) & 1;
  if (n<96) return (uf->f3>>(95-n)) & 1;
  return (uf->f4>>(127-n)) & 1; }

/* n=0 for most significant bit, the "sign" bit of f1; n=127 for the low order bit of f4
   return 1 if carry out of f1 */
int increment_universal_float(uf,n) universal_float *uf; int n;
{ if (n<0) printf("deep kimche");
  if (n>127) return 0; /* don't bother */
  if (n>=96) { uf->f4 += (1<<(127-n)); goto check4; }
  else if (n>=64) { uf->f3 += (1<<(95-n)); goto check3; }
  else if (n>=32) { uf->f2 += (1<<(63-n)); goto check2; }
  else { uf->f1 += (1<<(31-n)); goto check1; };

 check4:  if (uf->f4) return 0; uf->f3++;
 check3:  if (uf->f3) return 0; uf->f2++;
 check2:  if (uf->f2) return 0; uf->f1++;
 check1:  if (uf->f1) return 0; return 1;
 }

/*
conversion flags:
rounding mode.  anything else?
zero->? etc.
?set zero flags, etc? debatable.  inexact result.
*/

int u2fincremented, u2frounded, u2froundingdata, u2foverflow, u2funderflow, u2finexact;

/* may spoil uf; copy it if you need it */
universal2float(uf,f,fkind,flags)
universal_float *uf; char *f; float_typename fkind; int flags;
{ struct float_conv_table *fcv = &float_conv_table_array[fkind];
  int i,ff1,ff1b,ovfl,sh,reorder;unsigned long tmp; long xtrial; int xtrm;
/* $$$$ maybe sh should be long? */
  u2fincremented = u2frounded = u2froundingdata = u2foverflow = u2funderflow = u2finexact = 0;
if (!uf) uf = &uf_static;
printf("entered u2f %d %d %d %d\n",uf,f,fkind,flags);
printf("fcv: fkind %d  fcv %d  xsize %d  fsize %d  hd %d  bsize %d  bzsize %d  bzbeg %d\n",
  fkind, fcv, fcv->xsize, fcv->fsize, fcv->hidden, fcv->bsize, fcv->bzsize, fcv->bzbeg);
printf("flags: %x\n", fcv->flags);
print_universal_float(uf); printf("\n");
/* $$$$ branch out here for Nans, infinitiesm, & zeros */
if (uf->zero) goto makezero;
if (uf->inf) goto makeinf;
if (uf->nan) goto makenan;
/* $$$$ algorithm change: precompute exponent */
ff1 = find_first_one_uf(uf);
if (ff1>=0)
{ sh = ff1 - 31 + fcv->hidden;  /* second meaning for hidden $$$$$ */
  /* if exp+bias isn't multiple of M, then extra shifting is required $$$$ */
  if (fcv->flags & XM4) { xtrial = uf->x -sh +fcv->xbias - fcv->xsize;
			  xtrm = xtrial%4;
			  if (xtrm) { if (xtrm<0) xtrm+=4; sh -= (4-xtrm); }; };
  /* figure out inexact result flag $$$$$ */
  /* round before shifting */
  u2frounded = 1;
  u2froundingdata = rounding_data(uf,fcv->bits-sh);
  u2fincremented = decide_rounding(Network_Conversion_Rounding_Mode,
				   uf->s,u2froundingdata);
  if (u2fincremented)
  { ovfl = increment_universal_float(uf,fcv->bits-sh);
    if (ovfl) {left_shift_universal_float(uf,-1); sh--; uf->f1 = 0x80000000; uf->x--;};
    ff1b = find_first_one_uf(uf);
    if (ff1b!=ff1) sh--; /* $$$$ is re-rounding ever required? */ };
/* $$$$ change above code to sample the ff1 bit;
   also, if XM4, need to subtract 4 from sh */
  if (sh) { uf->x-=sh; left_shift_universal_float(uf,sh); }; };
/* Having calculated the final exponent, it's time to check for
   newly created infinities, zeros, denorms. $$$$$ */
/*  if (uf->x >= limit) goto inf code
 <lolimit goto 0 code
 <denlimit goto den code */
printf("ff1 %d  sh %d\n", ff1, sh);
print_universal_float(uf); printf("\n");

/*$$$$$
check-if-its-normalized-correctly
if (uf->f1 & (1<<magicpos) == 1<<mp;
  if so, skip shifting
otherwise will need to shift by ffo-mp.
need to check for shifted bits ...
may not need ovfl check in left-shift function.
  after shifting, adjust exponent
  if increment, may have overflowed to next power of 2; check.
  if truncate, may have reached 0.
 <remember rounding direction, in case of second roundings,
  such as norm->denorm when shortening; might accidentally make epsilon instead
  of zero when rounding epsilon/2-smidgen>
 <alternative: copy number, and be prepared to reshift/round in cases
  where exponent goes screwy>
max-expt, after which -> infinity (or max-float, if rounding down? check)
min-expt: convert to 0 or denorm.
$$$$$inexact result flag
$$$$$accrued flags
$$$$$underflow exception, overflow
  is underflow when we have to denorm, or when denorm -> 0?
  for overflow -> infinity: pretend there's a power of 2 just past max-finite
zero-check
infinity-check
nan-check, quiet/noisy
  converting nans to shorter precision: what to do with mantissa?
  keep mantissa sign (noisy/quiet?), and low bits if non0.
normalize, if appropriate; adjusting exponent
denormalize, if required to get exponent to be a multiple */

  if (fcv->flags & HIDDEN)  uf->f1 &= ~(1<<fcv->hidden);  /* hide leading bit */
  uf->x += fcv->xbias; /*$$$$$ may be different for denorms */ /*bias exponent*/
  uf->x -= fcv->xsize;

/* $$$$$ */
/* some of these have meaningless bits, which should be copied in from the
   mantissa, and checked not to be zero sometimes.
   if -0 is illegal, force sign +; ditto -inf.
   for makenan, need to force ~0 fraction, perhaps all 1s.
   do I need makeden?
$$$$ */
 makezero:
 makeinf:
 makenan:

  ftmp.l[0] = uf->f1;  /*store fraction*/
  if (fcv->bsize>4) { ftmp.l[1] = uf->f2;
		    if (fcv->bsize>8) { ftmp.l[2] = uf->f3;
				      if (fcv->bsize>12) ftmp.l[3] = uf->f4; }}
/* do any required low-word masking here */

/* $$$$$ divide exponent */
  ftmp.l[0] |= (uf->x << (31 - fcv->xsize));  /* store exponent */
  if (uf->s < 0)
    { if (fcv->flags & TWOS) twos_complement(ftmp.c,fcv->bsize);
/*$$$$$ make sure two's complementing operates with low byte masked! */
      else ftmp.l[0] |= 0x80000000;  /* store sign */ };
/*unbyte-mung */
/*$$$$$ add special out-conversion calls */
/*  if (fcv->reorder.any) */
    reorder = local_arch_reorder ^ (fcv->flags & REORDER_FLAGS);
  { /* check for special case reorder */
    if (fcv->reorder.special2) fcv->reorder.special2(ftmp,fkind,fcv,1);
    if (reorder & SWAPD)
  for (i=0;4*i<fcv->bsize;i+=4)
  { tmp = ftmp.l[i]; ftmp.l[i] = ftmp.l[i+2]; ftmp.l[i+2] = tmp;
    tmp = ftmp.l[i+1]; ftmp.l[i+1] = ftmp.l[i+3]; ftmp.l[i+3] = tmp; };
    if (reorder & SWAPL)
  for (i=0;4*i<fcv->bsize;i+=2)
  { tmp = ftmp.l[i]; ftmp.l[i] = ftmp.l[i+1]; ftmp.l[i+1] = tmp; };
    if (reorder & SWAPS)
  for (i=0;2*i<fcv->bsize;i+=2)
  { tmp = ftmp.s[i]; ftmp.s[i] = ftmp.s[i+1]; ftmp.s[i+1] = tmp; };
    if (reorder & SWAPC)
  for (i=0;i<fcv->bsize;i+=2)
  { tmp = ftmp.c[i]; ftmp.c[i] = ftmp.c[i+1]; ftmp.c[i+1] = tmp; };
    if (reorder & REVERSE)
  for (i=0;2*i+1<fcv->bsize;i++)
  { tmp = ftmp.c[i]; ftmp.c[i] = ftmp.c[fcv->bsize-1-i]; ftmp.c[fcv->bsize-1-i] = tmp; };
    if (fcv->reorder.special1) fcv->reorder.special1(ftmp,fkind,fcv,1);
    if (local_arch_reorder_f) local_arch_reorder_f(ftmp,fkind,fcv,1);
  };

  bcopy(&ftmp,f,fcv->bsize);
}

/*$$$$$ avoid writable static storage for ftmp */
/*
specials should accept direction flag?!

$$$$$ byte-mung may be a function of both remote & local architecture! $$$$$
$$$$$ right shift may need to know if bits shifted away, and last+sticky
*/

#define Local_Architecture_Single IEEE1
#define Local_Architecture_Double IEEE2

#define F2U float2universal
#define U2F universal2float
#define LA1 Local_Architecture_Single
#define LA1X Local_Architecture_Single_Extended
#define LA2 Local_Architecture_Double
#define LA2X Local_Architecture_Double_Extended
#define LA4 Local_Architecture_Quad
#define PUF(x) {print_universal_float(x); printf("\n");}

void test_float_conversions()
{  float a,d; double b,c; universal_float uf;
   a=1.0; F2U(&a,LA1,0); PUF(0); /* expectu(1,1,1,0,0,7,3); */
   copy_universal_float(0,&uf);
   U2F(0,&d,LA1,0); printf("%.8f\n",d);
   U2F(&uf,&c,LA2,0); printf("%.20f\n",c);
   a=0.3; F2U(&a,LA1,0); PUF(0);
   copy_universal_float(0,&uf);
   U2F(0,&d,LA1,0); printf("%.8f\n",d);
   U2F(&uf,&c,LA2,0); printf("%.20f\n",c);
   b=1.0; F2U(&b,LA2,0); PUF(0);
   U2F(0,&c,LA2,0); printf("%.20f\n",c);
   b=0.3; F2U(&b,LA2,0); PUF(0);
   U2F(0,&c,LA2,0); printf("%.20f\n",c);
/*$$$$$
  setu(....); U2F(...,LA1); expect(...);
*/
}

main() { test_float_conversions(); }

/* Issues for Conversion of Floating Point Numbers

Understand as many formats as possible, importance weighted by
number of machines connected with network & amount of data.
Known formats:
IEEE 32, 64, 80  includes Nan, -0, +-infinity, ?other infinity,
		 denormalized
Cray 64		 includes integers; software "double"
Vax, at least two formats, single & double
PDP11 single & double
PDP6, PDP10 single & double
IBM 360 single & double
Lispm short floats
other supercomputers
various FP chips & cards: M68881, etc.
other computers

non-binary reps: decimal, ascii, ebcdic, ternary
complexes
software float formats?

exponent underflow & overflow:
  losing a bit of range at the ends; grossly out of range.
Mantissa accuracy; rounding; avoidance of double rounding
older computers often had unnormalized numbers available;
 usually these don't exist in the newer architectures
traps vs. Nans vs. plow-ahead vs. error returns

"numbers" not representable in second architecture:
  infinity, maxpos, minpos, -0, cx-infinity, proj-infinity
unnormalized numbers
Nans

fail options: convert to larger format; nearest representible;
  zero; maxpos; maxsign;

sometimes it's enough to swap bytes
report if converting a number would cause trouble.

Goals:  a->b should produce nearest matching number.
if number is representible in both systems, should convert
 exactly.
a->b->c->d->... should avoid multiple roundings & loss of
accuracy.

a->b->a should be exact if formatB>=formatA.

a->b->a->b should = a->b

need opaque bits option
need names for formats
convert array, perhaps skipping

for storage on a different architecture, would like to preserve bits.

a->b->c should make sense.

options to convert everything to some universal, all encompassing format.
option to "convert everything to double"
? should any machine A be able to convert from format B to format C?
even if A has no floating format defined?

no-growth option
no information loss -- probably impossible

Processors/Architectures Formats
Byte-order questions: pairs, quads, octs, 16s.
?complex numbers?
typed numbers: Cray includes "48bit" integers.
signed magnitude versus complement notation.
reserved, non-numeric values

Cray 1
Vax - 1981  FDEG
Motorola MC68881 - Sun 3
Sparc
HP
IBM 360
80287 coprocessor
Decstations, Snakes

remember to keep references for format material.

so far, everything fits in ~16bit exponent & ~64bit fraction
HP defines a quad-precision 128bit thingie;
Cray provides software (at least) for this
Fortran might have Real*16?

imagine "floatcap" table!

unpack operation:
byte-reordering
twos-complementing
into size, sign, exponent, fraction
un-normalization info
special values: -0, +-infinity, Nan(quiet/noisy), reserved, trapping,
  impossible
unused bits, must be 0, must be 1

rounding methods
set flags on overflow/underflow/infinity conversions
inexact result

table needs escapes for special cases

Testing will be hard.

ieee params:
name   width sign frwid	prec. hid unrm?	ewid emax  emin	  ebias	inf? nan?
ieee1	32   bit   23	24    usu tiny 	 8   127   -126   127	yes  qn
ieee2	64   bit   52	53    usu tiny   11  1023  -1022  1023	yes  qn
ieee4	128  bit   112  113   usu tiny   15  16383 -16382 16383 yes  qn
(HPs definition of ieee4.  No mention of requiring leading 1 in double-
  extended precision.
 HP Precision Architecture.  Get year.)

sun3 matches ieee1 & ieee2
sun3.2x 96/80 bit  63   63    no  tiny   15  etc.
sun4/sparc

i287 math chip (& 8087?)


ieee is 2^e (1+f) or 2^e (0+f) for 0/denrm

cray1	64   bit   48   48    no  any    15  8191  -8192  16384 ?    ?
cray1d  128  etc.  112 (software, in my reading)
  Not clear what interpretation to assign to the extreme 50% of the
  exponent range here.  I didn't see any mention of underflow -> 0,
  or trap-on-overflow.
  Normalization may require a +48 in the exponent.
  Don't do anything special for the E=0 case.

ibm360  32   bit   24/56      no  any;4wide    7   e*16  64*4  no no
7094
pdp6/10/20
vax f 32 sign magn exp8 bias128 weird byte order hiddenbit zero=+e0,any frac
         trap=-e0,any frac
vax d 64 format F + 32trailing fraction bits
vax g 64 like D, but with 3 more bits of exponent
vax h 128 like D, with 15bit exp, 112+h fraction
vax: p10, p33ff, p176ff of architecture manual, 1981.
byte numbers seem to be low-order to high; this leads to some funny business
with reordering the double-bytes.
i8087 seems to have projective infinity, from the early Ieee draft

I may need a special flag for representations of 0.  Special formats,
and the -0 question, and unnormalized zero.
Having derived fields like prec & emax/emin is bad; confuses the code
unless primary/secondary is clear.

float-to-universal(&f,fkind)
fct = fconvtable[fkind];
g = f or bytemung(f);
sign = ...
un-complement?
exponent * multiplier;
exponent = ... + bias;
fraction = ... + hidden;  this must be done to the correct word.
done.

*/


/*
lots of tests
convert->u, <-u, and round trips starting at c, c->d
preserve c; preserve u of limited precision
check rounding & flags, in borderline cases.
defining borderline cases is tricky.
?have a file of expected results?
limit testing to handy architectures for now: sparc, decstation, snake.sun3/60?
 PCs & Macs are available, and Vaxen; probably IBM360.  caslon=sequent=80287?

Hmmm.  Propagate Nan data?
Would like convert+unconvert to be idempotent for same format;
what about bad-format numbers?  The programmer might know more
than I do.  Situation is different cross-architecture; here I'm
more secure in my knowledge: the programmer can't expect things
to behave well in conversions.
Would also like -0 and amount of unnormalness preserved.
Fallback position is to only preserve IEEE sanctioned properties;
this provides an excuse to lose normalization.

conversions when necessary
-0 -> 0
silent Nan -> noisy Nan
Nan -> 0 (or maybe infinity if available, or any trapping value)
+-infinity -> +-huge
overflow -> infinity (option: huge)
+-underflow -> +-0 (or denormalized?)
 Q: -epsilon -> +0 or -0 if the architecture permits?

flags for the conversions.
Trap option?  Message option?

Testing?  Top level command line interface?
Internal compiler generated numbers?
Need several architectures, and a way to send UDP packets between them.
Maybe use my RPC hacks.
The list of special numbers differs in each architecture.
Maybe a text file of test cases, and the results?

byte-swap options: byte, pair, long should do for now.
special stuff flag: how to handle?
  flags, hash-table, where-to-resume-value ??
*/

