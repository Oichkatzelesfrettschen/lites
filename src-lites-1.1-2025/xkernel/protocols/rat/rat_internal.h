/*
 * rat_internal.h     (Record Accessed TCP)
 *
 *
 *
 *
 */

#ifndef RAT_I_H
#define RAT_I_H

#include "xkernel.h"

#define HLEN sizeof(HDR)

#define         ACTIVE_MAP_SIZE 101
#define         PASSIVE_MAP_SIZE 23
#define         RECEIVED_MAP_SIZE 23
#define         CACHE_MAP_SIZE 101
#define 	RATMAXPACKET 0x7ffffff
#define 	RATOPTPACKET 0x7ffffff


typedef struct {
  int  prot_num;
  int  record_length;
  IPhost senderAddr;
} HDR;


typedef struct {
  Map   active_map;
  Map   passive_map;
  Map   received_map;
  Map   ip_map;
  IPhost myAddr;
  int	SPort;
  int   DPort;
  u_short tcpRcvBufSize;
  u_short tcpSndBufSize;
} PSTATE;

typedef struct {
  int    lls_exists;
  int    waiting_for_header;
  int    bytes_expected;
  Msg   *curr_msg;
  HDR    hdr;
  IPhost dest;
  Bind   ip_binding; 
} SSTATE;



typedef XObj ActiveID;
typedef int PassiveID;

typedef struct {
  IPhost remoteAddr;
  int    prot_num;
} IPID;

#endif
