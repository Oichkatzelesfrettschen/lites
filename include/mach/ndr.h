/*
 * Mach NDR (Network Data Representation) header
 * Data marshaling for cross-architecture IPC
 */

#ifndef _MACH_NDR_H_
#define _MACH_NDR_H_

typedef struct {
    unsigned char       mig_vers;
    unsigned char       if_vers;
    unsigned char       reserved1;
    unsigned char       mig_encoding;
    unsigned char       int_rep;
    unsigned char       char_rep;
    unsigned char       float_rep;
    unsigned char       reserved2;
} NDR_record_t;

extern NDR_record_t NDR_record;

#define NDR_PROTOCOL_2_0        0
#define NDR_INT_BIG_ENDIAN      0
#define NDR_INT_LITTLE_ENDIAN   1
#define NDR_FLOAT_IEEE          0
#define NDR_CHAR_ASCII          0

#endif /* _MACH_NDR_H_ */
