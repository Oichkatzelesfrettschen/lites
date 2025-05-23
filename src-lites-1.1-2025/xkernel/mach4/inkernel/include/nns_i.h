/*
 * nns_internal.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993  Arizona Board of Regents
 *
 *
 * $Revision: 1.4 $
 * $Date: 1993/11/08 16:34:21 $
 */

/* number of max/min threads to process requests */
#define NETNAME_THREADS_MIN 5
#define NETNAME_THREADS_MAX 20

/* max size of service buffer */
#define MAX_MSG_BUF 256

/* maximum bcast replies honored */
#define MAX_BCAST_REPLIES 20

/* the indices of the lower level protocols */
#define UDP_I    0
#define BOOTID_I 1

/* message types */
#define REQUEST_MODE 1
#define REPLY_MODE   2

/* network name service name */
#define NETWORK_NAME_SERVICE "NetworkNameServer"

/* broadcast timout in ms */
#define BCAST_TIMEOUT 100
#define BCAST_WAKEUPS 200

/* specific host timeout in ms */
#define HOST_TIMEOUT  100
#define HOST_WAKEUPS  200

typedef struct nns_bcast
{
  char mode;
  int id;
  IPhost client;
  IPhost server;
  char key[80];
} BCAST_MSG;

typedef struct nns_bcast_replies
{
  IPhost server;
} BCAST_REPLY;

typedef struct pstate
{
  XObj self;
  XObj udp;
  XObj udp_sessn;
  IPhost myaddr;
} PSTATE;
