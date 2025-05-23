/*
 * rat.c    (Record Accessed TCP)
 *
 *
 *
 *
 */

#include "xkernel.h"
#include "romopt.h"
#include "rat_internal.h"
#include "tcp.h"
#include "ip.h"
#include "chan.h"

#ifdef __STDC__

XObj   ratOpen(XObj, XObj, XObj, Part *);
static xkern_return_t ratOpenEnable(XObj, XObj, XObj, Part *);
static xkern_return_t ratOpenDisable(XObj, XObj, XObj, Part *);
static xkern_return_t ratDemux(XObj, XObj, Msg *);
static xmsg_handle_t ratPush(XObj, Msg *);
static xkern_return_t ratPop(XObj, XObj, Msg  *,void *);
static int ratControl(XObj, int, char *, int );
static int ratControlSessn(XObj, int, char *, int );
static xkern_return_t ratCloseSessn(XObj);
XObj   ratCreateSessn( XObj, XObj, XObj, ActiveID  *, IPID);
void   rat_init(XObj);
static void rat_sessn_init(XObj);
long   ratHdrLoad(void *, char *, long, void *);
long   ratDemuxHdrLoad(void *, char *, long, void *);
void   ratHdrStore(void *,char *, long, void *);
XObj   ratOpenTcp(XObj, IPhost);
XObj   ratOpenIp(XObj, IPhost);
static xkern_return_t   readSPort( XObj, char **, int, int, VOID * );
static xkern_return_t   readDPort( XObj, char **, int, int, VOID * );
static xkern_return_t   readSndBuf( XObj, char **, int, int, VOID * );
static xkern_return_t   readRcvBuf( XObj, char **, int, int, VOID * );

#else

XObj   ratOpen();
static xkern_return_t ratOpenEnable();
static xkern_return_t ratOpenDisable();
static xkern_return_t ratDemux();
static xmsg_handle_t ratPush();
static xkern_return_t ratPop();
static xkern_return_t ratCloseSessn();
static int ratControl();
static int ratControlSessn();
XObj   ratCreateSessn();
void   rat_init();
static void rat_sessn_init();
long   ratHdrLoad();
long   ratDemuxHdrLoad();
void   ratHdrStore();
XObj   ratOpenTcp();
XObj   ratOpenIp();
static xkern_return_t   readSPort();
static xkern_return_t   readDPort();
static xkern_return_t   readSndBuf();
static xkern_return_t   readRcvBuf();


#endif


static XObjRomOpt ratOpt[] = {
    { "sourcePort", 3, readSPort },
    { "destPort", 3, readDPort },
    { "SndBuf", 3, readSndBuf },
    { "RcvBuf", 3, readRcvBuf }
};


int traceratp;


XObj ratOpen(self, hlpRcv, hlpType, part)
  XObj self, hlpRcv, hlpType;
  Part *part;
{
  PSTATE     *pstate;
  ActiveID    key;
  IPID        ip_id;
  XObj        rat_s;
  IPhost      dest;
  

  xTrace0(ratp, TR_MAJOR_EVENTS, "RAT open");
  
  if (partLen(part) < 1) {
    xTrace0(ratp, TR_ERRORS, "ratOpen:  bad participants");
    return ERR_XOBJ;
  }

  pstate = (PSTATE *)self->state;
  
  dest = *(IPhost *)partPop(part[0]);
  
  ip_id.prot_num = relProtNum(hlpType, self);
  ip_id.remoteAddr = dest; 
  if (mapResolve(pstate->ip_map,(char *)&ip_id, &rat_s) == XK_SUCCESS) {
    xTrace1(ratp, TR_MAJOR_EVENTS, "ratOpen:  exiting with session %x", rat_s);
    rat_s->rcnt++;
    return rat_s;
  }  

  key = 0;
  rat_s = ratCreateSessn(self, hlpRcv, hlpType, &key, ip_id);
  if ( rat_s == ERR_XOBJ ) {
    xTrace0(ratp, TR_ERRORS, "ratOpen:  couldn't create rat session");
    return ERR_XOBJ;
  }


  return rat_s;
}


XObj ratOpenIp(self, dest)
     XObj       self;
     IPhost     dest;
{
  Part p[5];
  XObj lls;

  xTrace0(ratp, TR_EVENTS, "ratOpenIp:  called");
  partInit(p,1);
  partPush(p[0], (void*)&dest, sizeof(IPhost));

  lls = xOpen(self, self, xGetDown(self, 1), p);
  if (lls == ERR_XOBJ)
    {
      xTrace0(ratp, TR_ERRORS, "rat open: could not open ip");
      return ERR_XOBJ;
    }

  return lls;

}


XObj ratOpenTcp(self, dest)
     XObj 	self;
     IPhost     dest;
{
  PSTATE     *pstate = (PSTATE *) self->state;
  Part p[5];
  XObj lls;

  xTrace2(ratp, TR_EVENTS, "ratOpenTcp:  called DPort %d SPort %d",
	pstate->DPort, pstate->SPort);
  partInit(p,2);
  partPush(p[0], (void*)&dest, sizeof(IPhost));
  partPush(p[0], (void*)&pstate->DPort, sizeof(long));
  partPush(p[1], (void*)&pstate->myAddr, sizeof(IPhost));
  partPush(p[1], (void*)&pstate->SPort, sizeof(long));
  
  lls = xOpen(self, self, xGetDown(self, 0), p);
  if (lls == ERR_XOBJ) {
      xTrace0(ratp, TR_ERRORS, "rat open: could not open tcp");
      return ERR_XOBJ;
  }

  xControl(lls, TCP_SETRCVBUFSIZE, (char*)&pstate->tcpRcvBufSize, sizeof(u_short));
  xControl(lls, TCP_SETSNDBUFSIZE, (char*)&pstate->tcpSndBufSize, sizeof(u_short));
  return lls;

}


static xkern_return_t
ratOpenEnable(self, hlpRcv, hlpType, part)
     XObj self, hlpRcv, hlpType;
    Part *part;
{
  PSTATE      *ps = (PSTATE *)self->state;
  PassiveID    key;
  long        protNum;

  if (partLen(part) < 1)
    {
      xTrace0(ratp, TR_ERRORS, "ratOpenEnable:  bad participants");
      return XK_FAILURE;
    }

  if ( (protNum = relProtNum(hlpType, self)) == -1 )
    return XK_FAILURE;
  
  xTrace2(ratp, TR_GROSS_EVENTS, "ratOpenEnable: hlp=%s, protlNum=0x%x",
	  hlpRcv->name, protNum);
  
  key = protNum;
/*
  key = htons(key);
*/
  return defaultOpenEnable(ps->passive_map, hlpRcv, hlpType, (VOID *)&key);
}


static xkern_return_t
ratOpenDisable(self, hlpRcv, hlpType, part)
    XObj self, hlpRcv, hlpType;
    Part *part;
{
    PSTATE      *ps = (PSTATE *)self->state;
    long        protNum;
    PassiveID   key;

    if ( (protNum = relProtNum(hlpType, self)) == -1 ) {
        return XK_FAILURE;
    }
    xTrace2(ratp, TR_GROSS_EVENTS, "ratOpenDisable: hlp=%s, protlNum=0x%x",
            hlpRcv->name, protNum);
    key = protNum;
    key = htons(key);
    return defaultOpenDisable(ps->passive_map, hlpRcv, hlpType, (VOID *)&key);
}



static xkern_return_t
ratDemux(self, lls, m)
     XObj self, lls;
     Msg  *m;
{
  XObj          s;
  PSTATE       *pstate;
  HDR           hdr;
  Enable       *e;
  Msg          *rec_m;
  IPID		ip_id;
  
  

  xTrace1(ratp, TR_MAJOR_EVENTS, "ratDemux: message received from lls = 0x%x",
	  lls);
  pstate = (PSTATE *)self->state;


  if ( mapResolve(pstate->active_map, &lls, &s) == XK_SUCCESS ) {
    xAssert(xIsSession(s));
    return xPop(s, lls, m,0);
  }

  /* we need to check to see if we've received a message */
  /* from this lls before.  The reason this may have happened */
  /* is if we received a message that didn't have enough */
  /* data to determine the hlp protocol number */
  if ( mapResolve(pstate->received_map, &lls, &rec_m) == XK_SUCCESS ) {
    xTrace1(ratp, TR_EVENTS, "found a previously saved message of size %d from this lls", msgLen(rec_m));
    msgJoin(m, rec_m, m);
    xFree( (void*)rec_m );
    mapUnbind(pstate->received_map, &lls);
  }

  if (msgLen(m) < HLEN) {
    xTrace0(ratp, TR_EVENTS, "received message is shorter than header length, saving...");

    rec_m = (Msg *) xMalloc(sizeof(Msg));
    msgConstructCopy(rec_m, m);
    mapBind(pstate->received_map, &lls, rec_m);
    return XK_SUCCESS;
  }
      
  msgPop(m, ratDemuxHdrLoad, (void*)&hdr, HLEN, (void*)m);

  ip_id.prot_num = hdr.prot_num;
  ip_id.remoteAddr = hdr.senderAddr;
  if ( mapResolve(pstate->ip_map, &ip_id, &s) == XK_SUCCESS ) {
    if (((SSTATE *)s->state)->lls_exists) { 
      xTrace0(ratp, TR_ERRORS, "ratDemux: lls exists dropping message");
      return XK_FAILURE;
    } else {
      xSetDown(s,0,lls);
      s->binding = mapBind(pstate->active_map, (char *)&lls, (int) s);
      if (s->binding== ERR_BIND) {
        xTrace0(ratp, TR_ERRORS, "ratDemux: could not bind lls ");
        xSetDown(s,0,0);
        return XK_FAILURE;
      }
      xDuplicate(lls);
      ((SSTATE *)s->state)->lls_exists = 1;
      xAssert(xIsSession(s));
      xAssert(xIsSession(lls));
      return xPop(s, lls, m,0);
    }
  }
        
  xTrace1(ratp, TR_DETAILED, "ratDemux: checking passive map for protocol %d", hdr.prot_num);

  if ( mapResolve(pstate->passive_map, &hdr.prot_num, &e) == XK_FAILURE ) {
    xTrace1(ratp, TR_ERRORS, "ratDemux: no passive map entry for protocol %d ... dropping message", hdr.prot_num);
    return XK_FAILURE;
  }

  s = ratCreateSessn(self, e->hlpRcv, e->hlpType, &lls, ip_id);
  if ( s != ERR_XOBJ ) {
    xOpenDone(e->hlpRcv, s, self);
    s->rcnt++;
    xAssert(xIsSession(s));
    return xPop(s, lls, m,0);
  }
  xControl(lls, TCP_SETRCVBUFSIZE, (char*)&pstate->tcpRcvBufSize, sizeof(u_short));
  xControl(lls, TCP_SETSNDBUFSIZE, (char*)&pstate->tcpSndBufSize, sizeof(u_short));
    
  xTrace0(ratp, TR_ERRORS, "ratDemux: dropping message");
  return XK_FAILURE;
}

static xmsg_handle_t
ratPush(s, msg)
    XObj s;
    Msg *msg;
{
  SSTATE         *ss = (SSTATE*)s->state;
  PSTATE         *pstate;
  HDR             hdr = ss->hdr;
  xmsg_handle_t   val;
  XObj		  lls;
  chan_info_t	  *info;

  if (!(ss->lls_exists)) {
    pstate = (PSTATE *)xMyProtl(s)->state;
    lls = ratOpenTcp(xMyProtl(s),ss->dest);
    if (lls == ERR_XOBJ) return XK_FAILURE; 
    xDuplicate(lls);
    xSetDown(s,0,lls);
    s->binding = mapBind(pstate->active_map, (char *)&lls, (int) s);
    if (s->binding== ERR_BIND) {
      xTrace0(ratp, TR_ERRORS, "ratPush: could not bind lls ");
      xSetDown(s,0,0);
      return XK_FAILURE;
    }
    ss->lls_exists = 1;
  }
   
  hdr.record_length = msgLen(msg);

  /* get chan info */
  info = (chan_info_t *) msgGetAttr(msg,0);


  xTrace2(ratp, TR_MAJOR_EVENTS, "ratPush: pushing record of length %d for hlp %d",
	  hdr.record_length, hdr.prot_num);


  msgPush(msg, ratHdrStore, &hdr, HLEN, 0);

  val = xPush(xGetDown(s,0), msg);
  xControl(xGetDown(s,0), TCP_PUSH, 0, 0);
  if (info) {
     info->reliable = 1;
     info->expensive = 1;
  }
  return val;
}

static xkern_return_t
ratPop(self, lls, msg, h)
     XObj self, lls;
     Msg  *msg;
     void *h;
{
  HDR  hdr;
  SSTATE  *ss = (SSTATE *)self->state;
  PSTATE  *pstate = (PSTATE *)self->myprotl->state;
  long    length;
  Msg     new_msg;
  xkern_return_t val;

  xControl(lls, TCP_SETRCVBUFSPACE, (char*)&pstate->tcpRcvBufSize, sizeof(u_short));
  

  xTrace1(ratp, TR_MAJOR_EVENTS, "ratPop: received message of length %d",
	  msgLen(msg));
  xTrace3(ratp, TR_DETAILED, "ratPop: waiting_for_header=%d   bytes_expected=%d    curr_msg=0x%x", ss->waiting_for_header, ss->bytes_expected, ss->curr_msg);

  if (ss->waiting_for_header)
    {
      msgJoin(ss->curr_msg, ss->curr_msg, msg);
      ss->waiting_for_header = 0;
      msg = ss->curr_msg;
      ss->curr_msg = 0;
    }

  while (1)
    {
      if (ss->bytes_expected == 0)
	{
	  xTrace0(ratp, TR_DETAILED, "ratPop: Start of a new Record");
	  if (msgLen(msg) >= HLEN)
	    {
	      msgPop(msg, ratHdrLoad, (void *)&hdr, HLEN, (void*)msg);
	      ss->bytes_expected = hdr.record_length;
	      xTrace1(ratp, TR_DETAILED, "ratPop: size of next record is %d",
		      ss->bytes_expected);
	    }
	  else
	    {
	      xTrace0(ratp, TR_DETAILED, "ratPop: Message not long enough to hold the header... waiting for next message");
	      ss->waiting_for_header = 1;
	      ss->curr_msg = (Msg *)xMalloc(sizeof(Msg));
	      msgConstructCopy(ss->curr_msg, msg);
	      return XK_SUCCESS;
	    }
	}
      
      length = msgLen(msg);
      if ( ss->bytes_expected == length )
	{
	  xTrace0(ratp, TR_DETAILED, "ratPop: Message length equals bytes expected");
	  if (ss->curr_msg != 0)
	    {
	      msgJoin(ss->curr_msg, ss->curr_msg, msg);
	      
	      xTrace1(ratp, TR_EVENTS, "ratPop: handing message of length %d to hlp", msgLen(ss->curr_msg));
	      val = xDemux(self, ss->curr_msg);
	      msgDestroy(ss->curr_msg);
	      xFree((void*)ss->curr_msg);
	      ss->curr_msg = 0;
	      ss->bytes_expected = 0;
	      return val;
	    }
	  else
	    {
	      ss->bytes_expected = 0;
	      
	      xTrace1(ratp, TR_EVENTS, "ratPop: handing message of length %d to hlp", msgLen(msg));
	      
	      return xDemux(self, msg);
	    }
	}
      else if ( ss->bytes_expected > length )
	{
	  xTrace0(ratp, TR_DETAILED, "ratPop: bytes expected greater than the length of the message");

	  ss->bytes_expected -= length;
	  if (ss->curr_msg == 0)
	    {
	      ss->curr_msg = (Msg*)xMalloc(sizeof(Msg));
	      msgConstructCopy(ss->curr_msg, msg);
	    }
	  else
	    msgJoin(ss->curr_msg, ss->curr_msg, msg);

	  return XK_SUCCESS;
	}
      else
	{
	  /* there is at least one complete record */
	  xTrace0(ratp, TR_EVENTS, "ratPop: message length greater than bytes expected");
	  if (ss->curr_msg == 0)
	    {
	      ss->curr_msg = (Msg *)xMalloc(sizeof(Msg));
	      msgConstructEmpty(ss->curr_msg);
	    }
	  msgConstructEmpty(&new_msg);
	  msgChopOff(msg, &new_msg, ss->bytes_expected);
	  msgJoin(ss->curr_msg, ss->curr_msg, &new_msg);
	  msgDestroy(&new_msg);

	  xTrace1(ratp, TR_EVENTS, "ratPop: handing message of length %d to hlp", msgLen(ss->curr_msg));

	  xDemux(self, ss->curr_msg);
	  msgDestroy(ss->curr_msg);
	  xFree((void*)ss->curr_msg);
	  ss->curr_msg = 0;
	  ss->bytes_expected = 0;
	}
    }
}



static int 
ratControl(s, op, buf, len)
     XObj s;
     int op;
     char *buf;
     int len;
{
  int   val;

  xAssert(xIsProtocol(s));

  xTrace0(ratp, TR_GROSS_EVENTS, "ratControl Entered");

  switch (op) {
    case GETMAXPACKET :
      checkLen(len, sizeof(int));
      *(int *)buf = RATMAXPACKET;
      return sizeof(int);
    case GETOPTPACKET :
      checkLen(len, sizeof(int));
      *(int *)buf = RATOPTPACKET;
      return sizeof(int);

    default:  
    val = xControl(xGetDown(s,1), op, buf, len);
    return val; 
  }

}

static int 
ratControlSessn(s, op, buf, len)
     XObj s;
     int op;
     char *buf;
     int len;
{
  int   val;
  


  xTrace0(ratp, TR_GROSS_EVENTS, "ratControlSessn Entered");
  xAssert(xIsSession(s));

  switch (op) {
    case GETMAXPACKET :
      checkLen(len, sizeof(int));
      *(int *)buf = RATMAXPACKET;
      return sizeof(int);
    case GETOPTPACKET :
      checkLen(len, sizeof(int));
      *(int *)buf = RATOPTPACKET;
      return sizeof(int);

    default:  
    val = xControl(xGetDown(s,1), op, buf, len);
    return val; 
  }

}



XObj ratCreateSessn(self, hlpRcv, hlpType, activeid,ip_id)
     XObj self, hlpRcv, hlpType;
     ActiveID  *activeid;
     IPID     ip_id;
{
  XObj      s;
  SSTATE   *ss;
  PSTATE   *ps;
  XObj     lls = *(XObj *)activeid;
  XObj     lls_vec[2];
 

  ps = (PSTATE *)self->state;
  lls_vec[0] = lls;
  lls_vec[1] = ratOpenIp(self,ip_id.remoteAddr);
  if (lls_vec[1] == ERR_XOBJ) {
      xTrace0(ratp, TR_ERRORS, "ratCreateSessn: could not open ip");
      return ERR_XOBJ;
  }

  s = xCreateSessn(rat_sessn_init, hlpRcv, hlpType, self, 2, lls_vec);
  if (lls) {
    xDuplicate(lls); 
    s->binding = mapBind(ps->active_map, (char *)&lls, (int) s);
    if (s->binding == ERR_BIND) {
      xTrace0(ratp, TR_ERRORS, "ratCreateSessn: could not bind active map");
      return ERR_XOBJ;
    }
  }
  ss = (SSTATE *)xMalloc(sizeof(SSTATE));
  ss->ip_binding = mapBind(ps->ip_map, (char *)&ip_id, (int) s);
  if (ss->ip_binding == ERR_BIND) {
      xTrace0(ratp, TR_ERRORS, "ratCreateSessn: could not bind ip map");
      return ERR_XOBJ;
  }
  ss->waiting_for_header=0;
  ss->bytes_expected = 0;
  ss->curr_msg = 0;
  ss->hdr.prot_num = ip_id.prot_num; 
  ss->hdr.senderAddr = ps->myAddr;
  ss->dest = ip_id.remoteAddr;
  if (lls) { 
    ss->lls_exists = 1;
  } else {
    ss->lls_exists = 0;
  }

  xTrace1(ratp, TR_EVENTS, "ratCreateSessn:  hlp_protNum is %d", ss->hdr.prot_num);
  s->state = (VOID *)ss;
  return s;
}

static xkern_return_t ratCloseSessn(self )
     XObj self;
{
  PSTATE *ps = (PSTATE *)self->myprotl->state;
  SSTATE *ss = (SSTATE *)self->state;

  xAssert(xIsSession(self));
  xTrace1(ratp, TR_MAJOR_EVENTS, "RAT closing session %x", self);
  self->rcnt--;
  if (self->rcnt >  0) {
    return XK_SUCCESS;
  }

  xTrace1(ratp, TR_MAJOR_EVENTS, "RAT removing session %x", self);
  if (self->binding) mapRemoveBinding(ps->active_map, self->binding);
  if (ss->ip_binding) mapRemoveBinding(ps->ip_map, ss->ip_binding);
  if (xGetDown(self,0)) xClose(xGetDown(self, 0));
  if (xGetDown(self,1)) xClose(xGetDown(self, 1));
  return xDestroy(self);
}

void rat_init(self)
     XObj self;
{
  PSTATE *pstate;
  Part p[2];

  xTrace0(ratp, TR_MAJOR_EVENTS, "In rat init!");
  xAssert(xIsProtocol(self));

  pstate = X_NEW(PSTATE);
  self->state = (VOID *)pstate;
  pstate->active_map = mapCreate(ACTIVE_MAP_SIZE, sizeof(ActiveID));
  pstate->passive_map = mapCreate(PASSIVE_MAP_SIZE, sizeof(PassiveID));
  pstate->received_map = mapCreate(RECEIVED_MAP_SIZE, sizeof(ActiveID));
  pstate->ip_map = mapCreate(CACHE_MAP_SIZE, sizeof(IPID));
  pstate->SPort = 0;
  pstate->DPort = 5005;
  pstate->tcpRcvBufSize = 32*1024;
  pstate->tcpSndBufSize = 32*1024;
  findXObjRomOpts(self, ratOpt, sizeof(ratOpt)/sizeof(XObjRomOpt), 0);

  xControl(xGetDown(self, 0), GETMYHOST,(char*)&pstate->myAddr, sizeof(IPhost));

  xTrace4(ratp, TR_DETAILED, "My IP address is %d.%d.%d.%d",
	  pstate->myAddr.a,pstate->myAddr.b,pstate->myAddr.c,pstate->myAddr.d);

  self->open = ratOpen;
  self->openenable = ratOpenEnable;
  self->control = ratControl;
  self->opendisable = ratOpenDisable;
  self->demux = ratDemux;

  partInit(p,1);

  partPush(p[0], (void*)&pstate->myAddr, sizeof(IPhost));
  partPush(p[0], (void*)&pstate->DPort, sizeof(long));
  if ( xOpenEnable(self, self, xGetDown(self, 0), p) == XK_FAILURE  ) {
    xTrace0(ratp, TR_ERRORS, "ratOpenEnable:  could not openenble llp");
  }

}


static void
rat_sessn_init(s)
     XObj  s;
{
  xAssert(xIsSession(s));
  s->push = ratPush;
  s->pop = ratPop;
  s->close = ratCloseSessn;
  s->control = ratControlSessn;
}

long ratHdrLoad(dest, src, len, ignore)
     void *dest;
     char *src;
     long len;
     void *ignore;
{
  HDR *hdr = (HDR *)dest;
  long host;

  xTrace4(ratp,TR_FULL_TRACE,"ratcopy args: %x %x %x %x",
	  dest,src,len,ignore);
  
  bcopy(src, (char*)hdr, len);
  
  hdr->prot_num = ntohl(hdr->prot_num);
  hdr->record_length = ntohl(hdr->record_length);
  bcopy((char *)&hdr->senderAddr,(char *)&host, sizeof(host));
  host = ntohl(host);
  bcopy((char *)&host, (char *)&hdr->senderAddr,sizeof(host));

  return len;
}

long ratDemuxHdrLoad(dest, src, len, ignore)
     void *dest;
     char *src;
     long len;
     void *ignore;
{
  HDR *hdr = (HDR *)dest;
  long host;

  xTrace4(ratp,TR_FULL_TRACE,"ratcopy args: %x %x %x %x",
          dest,src,len,ignore);

  bcopy(src, (char*)hdr, len);

  hdr->prot_num = ntohl(hdr->prot_num);
  hdr->record_length = ntohl(hdr->record_length);
  bcopy((char *)&hdr->senderAddr,(char *)&host, sizeof(host));
  host = ntohl(host);
  bcopy((char *)&host, (char *)&hdr->senderAddr,sizeof(host));

  return 0;
}

void ratHdrStore(src, dest, len, ignore)
     void *src;
     char *dest;
     long len;
     void *ignore;
{
  HDR *hdr = (HDR *)src;
  long host;

  xTrace4(ratp,TR_FULL_TRACE,"ratcopy args: %x %x %x %x",
          src,dest,len,ignore);

  hdr->prot_num = htonl(hdr->prot_num);
  hdr->record_length = htonl(hdr->record_length);
  bcopy((char *)&hdr->senderAddr,(char *)&host, sizeof(host));
  host = htonl(host);
  bcopy((char *)&host, (char *)&hdr->senderAddr,sizeof(host));
  bcopy((char*)hdr, dest, len);
}


static xkern_return_t
readDPort( self, str, nFields, line, arg )
    XObj        self;
    int         line, nFields;
    char        **str;
    VOID        *arg;
{
    PSTATE      *ps = (PSTATE *)self->state;

    return
#ifdef XKMACHKERNEL
                                    sscanf1
#else
                                    sscanf
#endif
                                      (str[2], "%d", &ps->DPort) < 1  ?
                                                XK_FAILURE : XK_SUCCESS;
}

static xkern_return_t
readSPort( self, str, nFields, line, arg )
    XObj        self;
    int         line, nFields;
    char        **str;
    VOID        *arg;
{
    PSTATE      *ps = (PSTATE *)self->state;

    return
#ifdef XKMACHKERNEL
                                    sscanf1
#else
                                    sscanf
#endif
                                      (str[2], "%d", &ps->SPort) < 1  ?
                                                XK_FAILURE : XK_SUCCESS;
}

static xkern_return_t
readSndBuf( self, str, nFields, line, arg )
    XObj        self;
    int         line, nFields;
    char        **str;
    VOID        *arg;
{
    PSTATE      *ps = (PSTATE *)self->state;
    int         size;


#ifdef XKMACHKERNEL
    if ( sscanf1 (str[2], "%d", &size) < 1 ) {
        return XK_FAILURE; 
    }   
#else 
    if ( sscanf (str[2], "%d", &size) < 1 ) {
        return XK_FAILURE; 
    }   
#endif

    ps->tcpSndBufSize = size;
    return XK_SUCCESS;
}

static xkern_return_t
readRcvBuf( self, str, nFields, line, arg )
    XObj        self;
    int         line, nFields;
    char        **str;
    VOID        *arg;
{
    PSTATE      *ps = (PSTATE *)self->state;
    int		size;


#ifdef XKMACHKERNEL

    if ( sscanf1 (str[2], "%d", &size) < 1 ) {
	return XK_FAILURE; 
    }

#else
    if ( sscanf (str[2], "%d", &size) < 1 ) {
        return XK_FAILURE;
    }

#endif

    ps->tcpRcvBufSize = size;
    return XK_SUCCESS;
}



