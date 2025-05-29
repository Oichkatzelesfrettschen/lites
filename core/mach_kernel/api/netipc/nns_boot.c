/*     
 * $RCSfile: nns_boot.c,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993  Arizona Board of Regents
 *
 *
 * $Revision: 1.4 $
 * $Date: 1993/08/21 01:30:21 $
 */

#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <mach.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <netdb.h>

#include "xkernel.h"
#include "ip.h"
#include "nns.h"
#include "nns_i.h"
#include "gc.h"
#include "ssr.h"
#include "bidctl.h"

extern int tracennsp;

#define BOOTIDProtl xGetDown(pstate->self, BOOTID_I)

#define MAXHOSTNAMELEN 100
#define NNS_SSR_SERVICE 10

/* a full mach postcard */
struct mach_postcard_msg {
  mach_msg_header_t mmhdr;
  mach_msg_type_t   mmbody;
  enum ssr_operation_type op;
  int id;
};

/* a full mach postcard server request via server registry request: 
   mach hdr, ssr data, postcard server request  */
struct mach_server_postcard_msg {
  mach_msg_header_t mmhdr;
  mach_msg_type_t   mmbody;
  struct ssrdata ssrd;
  enum ssr_operation_type op;
  int id;
};

extern mach_port_t delayPort;
extern PSTATE *pstate;

mach_port_t nns_ssr_port;
extern mach_port_t service;
Map IdMap;


/* ssr server registry port - initialized during ssr_init */
extern mach_port_t server_registry_port;

static Semaphore registrationSem;
static Semaphore broadcastSem;
static mutex_t idMutex;
static mutex_t nameMutex;
static Map HostPortMap;
static int nns_register_bidctl();
int gethostname();
mach_msg_return_t mach_msg_send();
mach_msg_return_t mach_msg_receive();

#ifdef __STDC__

static int nns_register_bidctl(IPhost ipaddr);
void nns_null_function(void);
int nns_IsIpAddr(char *ipaddr);
int nns_get_host_ip_addr(char *name, char *ipaddr);
mach_port_t nns_look_up_host(IPhost host);
mach_port_t nns_register_host (IPhost host);
void nns_deregister_host(IPhost host);
int nns_init_bootstrap(void);
mach_port_t nns_execute_broadcast(char *key);
static void delayFunc(int interval);

#else

static int nns_register_bidctl();
void nns_null_function();
int nns_IsIpAddr();
int nns_get_host_ip_addr();
mach_port_t nns_look_up_host();
mach_port_t nns_register_host();
void nns_deregister_host();
int nns_init_bootstrap();
mach_port_t nns_execute_broadcast();
static void delayFunc();

#endif __STDC__


void nns_null_function()
{
}

/* delay interval ms */
static void delayFunc(interval)
int interval;
{
  mach_msg_header_t m;

  cthread_yield();
  (void)mach_msg(&m, MACH_RCV_MSG|MACH_RCV_TIMEOUT,
		 0, sizeof m, delayPort,
		 interval, MACH_PORT_NULL);
}

static int get_unique_id()
{
  static int id = 0;
  static int ret;

  mutex_lock(idMutex);
  ret = ++id;
  mutex_unlock(idMutex);

  return ret;
}


int nns_IsIpAddr(char *ipaddr)
{
  int a, b, c, d;

  return sscanf(ipaddr, "%d.%d.%d.%d", &a, &b, &c, &d) == 4;
}

int nns_get_host_ip_addr(char *name, char *ipaddr)
{
  struct hostent *ans;
  int i;

  mutex_lock(nameMutex);
  ans = gethostbyname(name);
  mutex_unlock(nameMutex);

  if (! ans) return -1;
  for (i=0;i<sizeof(IPhost);i++)
    ipaddr[i] = ans->h_addr[i];
  return 0;
}

static int get_my_ip_addr(char *ipaddr)
{
  char name[MAXHOSTNAMELEN];

  if (gethostname(name, MAXHOSTNAMELEN) < 0)
    return -1;
  return nns_get_host_ip_addr(name, ipaddr);
}

mach_port_t nns_look_up_host(host)
     IPhost host;
{
  mach_port_t port;

  /* assume host name is an IP address */
  xk_master_lock();
  if (mapResolve(HostPortMap, &host, &port) == XK_SUCCESS) {
    xk_master_unlock();
    return port;
  }
  xk_master_unlock();
  return MACH_PORT_NULL;
}


/*
 * nns_register
 *     register a machine
 */ 
mach_port_t nns_register_host (host)
     IPhost host;
{
  struct mach_server_postcard_msg reqmsg;
  kern_return_t ret;
  static mach_port_t remote_port = MACH_PORT_NULL;
  int i, unique_id;

  xTrace4(nnsp, TR_DETAILED, "NNS nns_register_host: Registering host %d.%d.%d.%d", host.a,host.b,
	  host.c,host.d);

  xk_master_lock();
  semWait(&registrationSem);
  xk_master_unlock();

  /* after waking up it is possible a previous thread
     could have already registered the host */
  remote_port = nns_look_up_host(host);
  if (remote_port != MACH_PORT_NULL) {
    xk_master_lock();
    semSignal(&registrationSem);
    xk_master_unlock();
    return remote_port;
  }

  reqmsg.mmhdr.msgh_remote_port = server_registry_port;
  reqmsg.mmhdr.msgh_local_port = nns_ssr_port;
  reqmsg.mmhdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND,
					 MACH_MSG_TYPE_MAKE_SEND);
  reqmsg.mmhdr.msgh_size = sizeof(struct mach_server_postcard_msg);
  reqmsg.mmbody.msgt_name = MACH_MSG_TYPE_UNSTRUCTURED;
  reqmsg.mmbody.msgt_size = MNBYTESIZE;
  reqmsg.mmbody.msgt_number = sizeof(struct mach_server_postcard_msg) -
    sizeof(mach_msg_header_t) - sizeof(mach_msg_type_t);
  reqmsg.mmbody.msgt_inline =     1;
  reqmsg.mmbody.msgt_longform =   0;
  reqmsg.mmbody.msgt_deallocate = 0;
  reqmsg.ssrd.service_id = NNS_SSR_SERVICE;
  reqmsg.ssrd.operation = REQUEST;
  reqmsg.ssrd.destination_host = host;
  reqmsg.op = htonl(REQUEST);
  unique_id = get_unique_id();
  reqmsg.id = htonl(unique_id);

  /* send the request to server */

  if ((ret = mach_msg_send(&reqmsg)) != MACH_MSG_SUCCESS) {
    xTrace1(nnsp, TR_ERRORS, "NNS nns_register_host: Error: Failed to send request message ret = x%x", ret);
    return MACH_PORT_NULL;
  }
  xTrace1(nnsp, TR_DETAILED, "NNS nns_register_host: Sent request to server for the name server port : ID = %d", ntohl(reqmsg.id));

  for (i=0; i < HOST_WAKEUPS; i++) {
    delayFunc(HOST_TIMEOUT);
    xk_master_lock();
    if (mapResolve(IdMap, &unique_id, &remote_port) == XK_SUCCESS) {
      mapUnbind(IdMap, &unique_id);
      xk_master_unlock();
      break;
    }
    remote_port = MACH_PORT_NULL;
    xk_master_unlock();
  }

  /* get the lock again - needed below */
  xk_master_lock();

  if (remote_port == MACH_PORT_NULL) {
    xTrace0(nnsp, TR_ERRORS, "NNS nns_register_host: Failed to get remote server's name port");
  }
  else {
    ret = mach_port_mod_refs(mach_task_self(), remote_port,
			     MACH_PORT_RIGHT_SEND, 1);
    if (ret != KERN_SUCCESS) {
      xTrace0(nnsp, TR_DETAILED, "NNS nns_register_host: Failed to modify references to the remote port");
    }
    else {
      mapBind(HostPortMap, &host, remote_port);
      nns_register_bidctl(host);

      xTrace0(nnsp, TR_EVENTS, "NNS nns_register_host: Returning from Host look up");
    }
  }

  semSignal(&registrationSem);
  xk_master_unlock();
  return remote_port;
}


void 
nns_deregister_host(host)
     IPhost host;
{
  if (mapUnbind(HostPortMap, &host) != XK_SUCCESS) {
    xTrace4(nnsp, TR_ERRORS, "nns_deregister_host: failed to deregister host %d.%d.%d.%d",
	    host.a, host.b, host.c, host.d);
  }
  xTrace4(nnsp, TR_EVENTS, "nns_deregister_host: deregistered host %d.%d.%d.%d",
	  host.a, host.b, host.c, host.d);
}


/*
 * receive_requests
 *
 *
 */ 

static int receive_requests_and_reply()
{
  struct local_ssr_msg msg;
  struct mach_postcard_msg *repmsg;
  kern_return_t ret;

  repmsg = (struct mach_postcard_msg *)&msg;

  while (1) {
    repmsg->mmhdr.msgh_size = sizeof(struct local_ssr_msg);
    repmsg->mmhdr.msgh_local_port = nns_ssr_port;
    if ((ret=mach_msg_receive(repmsg)) == KERN_SUCCESS) {
      repmsg->op = ntohl(repmsg->op);
      if (repmsg->op == REQUEST) {
	xTrace0(nnsp, TR_DETAILED, "NNS receive_requests_and_reply: Got a host registration request");
	/* reply with our name server port */
	repmsg->mmhdr.msgh_local_port = service;
	repmsg->mmhdr.msgh_kind = MACH_MSGH_KIND_NORMAL;
	repmsg->mmhdr.msgh_bits = 
	  MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND);
	repmsg->mmhdr.msgh_size = sizeof(struct mach_postcard_msg);
	repmsg->mmbody.msgt_name = MACH_MSG_TYPE_UNSTRUCTURED;
	repmsg->mmbody.msgt_size = MNBYTESIZE;
	repmsg->mmbody.msgt_number = sizeof(struct mach_postcard_msg) - sizeof(mach_msg_header_t) - sizeof(mach_msg_type_t);
	repmsg->mmbody.msgt_inline = 1;
	repmsg->mmbody.msgt_longform = 0;
	repmsg->mmbody.msgt_deallocate = 0;
	repmsg->mmbody.msgt_unused = 0;
	repmsg->op = htonl(REPLY);

	if ((ret = mach_msg_send(repmsg)) != MACH_MSG_SUCCESS) {
	  xTrace0(nnsp, TR_ERRORS, "NNS receive_requests_and_reply: Error: Failed to send reply message");
	}
	xTrace0(nnsp, TR_DETAILED, "NNS receive_requests_and_reply: Server Replied with name server port");
      }
      else if (repmsg->op == REPLY) {
	repmsg->id = ntohl(repmsg->id);
	xTrace1(nnsp, TR_DETAILED, "NNS receive_requests_and_reply: Got a reply from the server: ID = %d", repmsg->id);

	xk_master_lock();
	mapBind(IdMap, &repmsg->id, repmsg->mmhdr.msgh_remote_port);
	xk_master_unlock();
      }
      else {
	xTrace1(nnsp, TR_ERRORS, "NNS receive_requests_and_reply: Error: got a bad operation: %d!!", repmsg->op);
      }
    }
  }
}

/*
 * nns_init_bootstrap 
 *
 *
 */

int 
nns_init_bootstrap()
{
  struct local_ssr_msg regmsg;
  kern_return_t ret;

  idMutex = mutex_alloc();
  nameMutex = mutex_alloc();
  semInit(&registrationSem, 1);
  semInit(&broadcastSem, 1);

  if (get_my_ip_addr((char *)&pstate->myaddr) < 0) {
    xTrace0(nnsp, TR_ERRORS, "NNS nns_init_bootstrap: Error: Failed to get my ip addr");
    return -1;
  }

  HostPortMap = mapCreate(100, sizeof(IPhost));
  IdMap = mapCreate(100, sizeof(int));

  if ((ret = mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE,
				&nns_ssr_port)) != KERN_SUCCESS ) {
    xTrace0(nnsp, TR_ERRORS, "NNS nns_init_bootstrap: Error: Failed to allocate nns port");
    return -1;
  }

  xTrace0(nnsp, TR_DETAILED, "NNS nns_init_bootstrap: Registering nns with ssr");
  
  /* register nns with ssr */
  regmsg.mmhdr.msgh_remote_port = server_registry_port;
  regmsg.mmhdr.msgh_local_port = nns_ssr_port;
  regmsg.mmhdr.msgh_bits = 
    MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND);
  regmsg.mmhdr.msgh_size = sizeof(regmsg);
  regmsg.mmbody.msgt_name = MACH_MSG_TYPE_UNSTRUCTURED;
  regmsg.mmbody.msgt_size = 32;  /* 32 bits */
  regmsg.mmbody.msgt_number = 2; /* 2 items in ssrd relevant to server */
  regmsg.mmbody.msgt_inline = 1;
  regmsg.mmbody.msgt_longform = 0;
  regmsg.mmbody.msgt_deallocate = 0;
  regmsg.ssrd.operation = REGISTER;
  regmsg.ssrd.service_id = NNS_SSR_SERVICE;
  if ((ret = mach_msg_send(&regmsg)) != MACH_MSG_SUCCESS) {
    xTrace0(nnsp, TR_ERRORS, "NNS nns_init_bootstrap: Error: Failed to send register message");
    return -1;
  }

  xTrace0(nnsp, TR_DETAILED, "NNS nns_init_bootstrap: starting thread for requests");

  /* start a listening thread to look for requests on the
     nns_ssr_port */
  cthread_detach(cthread_fork((cthread_fn_t)receive_requests_and_reply, 0));

  return 0;
}


mach_port_t nns_execute_broadcast(key)
    char *key;
{
  Msg msg;
  BCAST_MSG bcast;
  int id, i;
  BCAST_REPLY reply[MAX_BCAST_REPLIES+1];
  mach_port_t remote_port = MACH_PORT_NULL;

  /* create and send a broadcast message */
  bcast.id = id = get_unique_id();
  strcpy(bcast.key, key);
  bcast.client = pstate->myaddr;
  bcast.mode = REQUEST_MODE;

  xk_master_lock();
  semWait(&broadcastSem);

  /* initialize dummy list header */
  bzero((char *)reply, (MAX_BCAST_REPLIES+1)*sizeof(BCAST_REPLY));

  /* add id to map */
  mapBind(IdMap, &id, &reply);

  /* send bcast request */
  msgConstructInplace(&msg, (char *)&bcast, sizeof(BCAST_MSG),
		      nns_null_function);
  xPush(pstate->udp_sessn, &msg);
  msgDestroy(&msg);
  xk_master_unlock();

  /* wait for replies */
  for (i=0; i < BCAST_WAKEUPS; i++) {
    delayFunc(BCAST_TIMEOUT);
    if (reply[0].server.a != 0)
      break;
  }

  xTrace0(nnsp, TR_DETAILED, "NNS nns_execute_broadcast: Back from waiting for broadcast replies");

  /* free this id */
  xk_master_lock();
  xAssert(mapUnbind(IdMap, &id) == XK_SUCCESS);
  xk_master_unlock();

  /* no - replies received... */
  if (reply[0].server.a == 0) {
    xTrace0(nnsp, TR_DETAILED, "NNS nns_execute_broadcast: Found no repiles for the broadcast");
    remote_port = MACH_PORT_NULL;
  }
  else {
    /* check which host is already registered with us -
       if so return it's ns port */
    for (i = 0;
	 reply[i].server.a != 0;
	 i++)
      if ((remote_port = nns_look_up_host(reply[i].server))
	  != MACH_PORT_NULL)
	break;

    if (reply[i].server.a == 0) {
      /* if none registered pick first host that can be registered */
      for (i=0;
	   reply[i].server.a != 0;
	   i++) {
	remote_port = nns_register_host(reply[i].server);
	if (remote_port != MACH_PORT_NULL)
	  break;
      }
    }
  }

  xk_master_lock();
  semSignal(&broadcastSem);
  xk_master_unlock();

  return remote_port;
}


static int
nns_register_bidctl(ipaddr)
     IPhost ipaddr;
{
  Part part[1];
  xkern_return_t ret;

  partInit(part, 1);
  partPush(part[0], &ipaddr, sizeof(IPhost));
  if ((ret = xOpenEnable(pstate->self,pstate->self,BOOTIDProtl,part)) != XK_SUCCESS) {
    xTrace4(nnsp, TR_ERRORS, "nns_register_bidctl: open enable on bidclt failed for addr %d.%d.%d.%d", ipaddr.a, ipaddr.b, ipaddr.c, ipaddr.d);
    return -1;
  }
  xTrace4(nnsp, TR_FULL_TRACE, "nns_register_bidctl: registered host %d.%d.%d.%d with bidctl", ipaddr.a, ipaddr.b, ipaddr.c, ipaddr.d);
  return(0);
}
