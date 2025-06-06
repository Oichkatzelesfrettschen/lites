/*
 *
 * hdr-utils.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/06/24 01:04:59 $
 */


/*
 *
 *  Support functions for converting between different byte orders
 *
 */


#define MAX_MACH_DATA_TYPE   MACH_MSG_TYPE_STRING_C+1

#define MNBYTESIZE 8

#define MN_ARCH_MARKER    (1<<(3*MNBYTESIZE)|((2<<(2*MNBYTESIZE) | 3 <<MNBYTESIZE) | 4))
#define MN_OTHER_ENDIAN   (2<<(3*MNBYTESIZE)|((1<<(2*MNBYTESIZE) | 4 <<MNBYTESIZE) | 3))
#define MN_WORD_SWAP      (4<<(3*MNBYTESIZE)|((3<<(2*MNBYTESIZE) | 2 <<MNBYTESIZE) | 1))
#define MN_SCRAMBLE       (3<<(3*MNBYTESIZE)|((4<<(2*MNBYTESIZE) | 1 <<MNBYTESIZE) | 2))
enum SOURCE_BYTE_ARCH { PERM1=MN_ARCH_MARKER, PERM2=MN_OTHER_ENDIAN, PERM3=MN_WORD_SWAP, PERM4=MN_SCRAMBLE };

enum REAL_NUMBER_TYPE { FLOAT, DOUBLE, DOUBLELONG };
enum MACHINE_TYPE { VAX, SUN3, MIPS, SUN4, HPRISC, M68020, INTELn86, ALPHA };

#ifndef HDRUTILS
extern long  (*unpermute_int64[4])();
extern int   (*unpermute_int32[4])();
extern short (*unpermute_int16[4])();
extern float (*unpermute_real32[4])();
extern mach_msg_type_t (*unpermute_msg_type[4])();

extern int arch_unpermute_index( enum SOURCE_BYTE_ARCH );
extern long  nop_64(char *);
extern int   nop_32(char *);
extern short nop_16(char *);
extern char  nop_8(char *);
#endif HDRUTILS

