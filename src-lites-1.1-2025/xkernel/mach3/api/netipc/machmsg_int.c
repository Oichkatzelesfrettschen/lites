/*
 *
 * mach-full-convert.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.11 $
 * $Date: 1993/08/12 19:31:39 $
 */

/*
 *
 *  Interpreter for the Mach msg structure
 *   This is called only for messages that have the 'complex' bit set
 *
 *  Support for heterogeneous architectures is not included in this
 *  version.
 *
 */

#include <stdio.h>
#include <mach.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <cthreads.h>
#include "xkernel.h"
#include "ip.h"
#include "rwlock.h"
#include "machripc_internal.h"

extern int tracemachripcp;
extern Map xkMsgIdMap;  /* port transfer completion indicator */

#define VMADDR_SIZE  4

/* Null_Netport  definition shared with port_maint */
extern mnetport Null_Netport;  /* error return value */

typedef	long (*Pfl) ();

static long
ool_copy(char *from, char *to, long len, void *arg)
{
  bcopy(from, to, len);
  return len;
}


/* external  */
mnetport convert_to_netport(mach_port_right_t port, mach_port_type_t right, IPhost dest, msg_id_t msgid, mnportid_t pnn);

#define is_netrpc(msg, rright) ((msg.msgt_inline == 1) && (msg.msgt_number == 1) && (rright == MACH_PORT_TYPE_SEND_ONCE))

#define is_machrpc(msg, rright) ((msg->mmbody.msgt_inline == 1) && (msg->mmbody.msgt_number == 1) && (rright == MACH_PORT_TYPE_SEND_ONCE))


/* ***** end temporary decl's ***** */

#define PORT_CHUNK_SIZE (20 * sizeof(mn_netport_t))


/* int item_size_table[MAX_MACH_DATA_TYPE] = {
  1, 2, 4, 0, 0, 0, 0, 0, 1, 4, 0, 1 
  };*/
int item_size_table[MAX_MACH_DATA_TYPE] = {
  4, 2, 4, 0, 0, 0, 0, 0, 1, 1, 4, 0, 1
  };

/*
#define MACH_MSG_TYPE_UNSTRUCTURED	0
#define MACH_MSG_TYPE_BIT		0
#define MACH_MSG_TYPE_BOOLEAN		0
#define MACH_MSG_TYPE_INTEGER_16	1
#define MACH_MSG_TYPE_INTEGER_32	2
#define MACH_MSG_TYPE_CHAR		8
#define MACH_MSG_TYPE_BYTE		9
#define MACH_MSG_TYPE_INTEGER_8		9
#define MACH_MSG_TYPE_REAL		10
#define MACH_MSG_TYPE_STRING		12
#define MACH_MSG_TYPE_STRING_C		12
*/

/*
 *  Message pop and push utilities
 *
 *   Forward declarations
 *
 */

struct datargs {
  int           arch_index;
  int		data_type;
  int	  	real_number_arch;
};

#ifdef LOCALMSGAPPEND
/*
 *  local message append function (if msgtool does not support it)
 *
 */
struct msgappendargs {
  Msg   msg;
  Msg  *cmsg;
  Msg   rcmsg;
  char *buffer;
  char *buffertail;
  long *blen;
};
#endif LOCALMSGAPPEND

#ifdef PORTLOCKS
/*
 *  push a port net number onto a message; entirely local operation;
 *  this will not go out on the net.
 *
 */
static long
pnumcopy ( from, to, len, arg)
     char *from, *to;
     long len;
     VOID *arg;
{
  bcopy(from, to, sizeof(mnportid_t *));
  return sizeof(mnetport *);
}
#endif PORTLOCKS

#ifdef __STDC__
void xk_mach_msg_convert (
			  struct mach_msg_big_t  *msg,
			  Msg			 *fulloutmsg,
			  machnetipc_hdr	 *nethdr,
			  IPhost		  dest);

mach_msg_header_t *
xk_ports_and_ool_convert(Msg			*netmsg,
			 int			mach_netport_count,
			 int			mach_notinline_count,
			 msg_id_t		msgid,
			 IPhost			srchost,
			 int                    xfercompletion);

mach_msg_header_t *
xk_netmach_msg_to_mach(Msg			*netmsg,
			 int			mach_netport_count,
			 int			mach_notinline_count,
			 msg_id_t		msgid,
			 IPhost			srchost,
			 int                    xfercompletion,
                         mn_arch_tag_t		arch_type);

mach_port_t convert_netport_to_mach_port(mn_netport_t *netport,
					 XObj self, XObj lower_session,
					 Msg *reply_msg,
					 msg_id_t msgid, IPhost sender);

void set_convert_vector(enum SOURCE_BYTE_ARCH arch, Pfv *vec);

long port_hdr_pop_and_swap(
			      mn_netport_t *output,
			      char *input,
			      long len,
		              void *(*argf[])()
			      );

int need_to_swap(
		    int data_type
		    );

long data_pop_and_swap(
			      char *input,
			      char *output,
			      long  len,
			      struct datargs *arg
			      );

static long port_append_func(
     mnetport *input,
     char     *output,
     int       len,
     struct    datargs *arg
			     );

static long
longhdr_pop (
     mach_msg_type_long_t  *output,
     char *input,
     long len,
     void *(*arg[])()
  );

static long
justcopy (
     char *input,
     char *output,
     long len,
     void *direction
	  );

static long
justcopy_andlie (
     char *input,
     char  *output,
     long len,		 
     void *direction
	  );

#ifdef LOCALMSGAPPEND
static void
msgAppendDone(
     struct msgappendargs *msgarg);
#endif LOCALMSGAPPPEND

static mach_port_right_t
set_resend_right_type(unsigned int segtype);

static mach_port_type_t
set_resend_outward_port_type(unsigned int segtype);

extern xkern_return_t quick_netport_lookup( unsigned int portid, mnetport **n);

extern void rrxTransferComplete ( IPhost , msg_id_t);

extern void srxTransferComplete ( IPhost , msg_id_t);
#else
void xk_mach_msg_convert ();
mach_msg_header_t *xk_ports_and_ool_convert();
mach_msg_header_t *xk_netmach_msg_to_mach();
mach_port_t convert_netport_to_mach_port();
void set_convert_vector();
long port_hdr_pop_and_swap();
int need_to_swap();
long data_pop_and_swap();
static long port_append_func();
static long longhdr_pop();
static long justcopy();
static long justcopy_andlie();
#ifdef LOCALMSGAPPEND
static void msgAppendDone();
#endif LOCALMSGAPPEND
static mach_port_type_t set_resend_outward_port_type();
static int set_resend_right_type();
extern xkern_return_t quick_netport_lookup();
extern void rrxTransferComplete ( );
extern void srxTransferComplete ( );
#endif __STDC__


/*
 *   no_free --- a nop free function for ool buffers
 */
static void 
no_free_func(void *ptr)
{
}

/*
 * xk_mach_msg_convert (msg, netmsg, nethdr, destination)
 *
 *    creates outgoing message body 
 *
 *    convert all transferred ports, out-of-line data, etc.
 * 
 * 
 */
void
xk_mach_msg_convert (
  struct mach_msg_big_t  *msg,
  Msg			 *fulloutmsg,
  machnetipc_hdr	 *nethdr,
  IPhost		  dest)   
{
  Msg		 	 outdata_dict_msg;
  long		 	 msg_size;
  mach_msg_type_t	*msg_ptr;
  int			*data_item_number;
  int			 item_count;
  int			 port_item_number;
  mn_netport_t		*port_buffer;
  Msg			 port_dict_msg;
  int			 notinline = 0;
  bool			 made_portd = FALSE, made_datad = FALSE;
  msg_id_t		 msgid = nethdr->content.machmsg.message_id;
  int		         ports; /* port completion indicator */
  struct msg_attribute  *msg_attr = (struct msg_attribute  *)xMalloc(sizeof(struct msg_attribute));

#define MAX_OOL_INDEX 32
  struct ool_list        ools[MAX_OOL_INDEX];
  Msg                    ool_list_msg;
  int                    ool_list_index = 0;

#ifdef PORTLOCKS
  Msg                    locked_ports;

  msgConstructEmpty(&locked_ports);
#endif PORTLOCKS

  xTrace0(machripcp, TR_FULL_TRACE, "machnetipc: xk_mach_msg_convert");

  msgConstructEmpty(&ool_list_msg);
  bzero((char *)ools, MAX_OOL_INDEX * sizeof(struct ool_list));

  bzero((char *)msg_attr, sizeof(struct msg_attribute));

  msg_size = msg->mmhdr.msgh_size - sizeof(mach_msg_header_t);
  msg_ptr = (mach_msg_type_t *)((char *)msg + sizeof(mach_msg_header_t));
  data_item_number = 0;
  port_item_number = 0;

  while (msg_size > 0) {
    int		     hdr_size;
    int		     item_size;
    int		     item_byte_size;
    int		     item_name;
    mach_port_t     *port_ptr;
    int		     i;
    int		     port_count;
    char	    *item_ptr;
    int		     datainline;
    bool	     receive_right;

    hdr_size = sizeof(mach_msg_type_t);
    item_count = msg_ptr->msgt_number;
    item_size = msg_ptr->msgt_size;
    item_name = msg_ptr->msgt_name;
    datainline = msg_ptr->msgt_inline;
    receive_right = FALSE;

    if (msg_ptr->msgt_longform) {
      hdr_size = sizeof(mach_msg_type_long_t);
      item_count = ((mach_msg_type_long_t *)msg_ptr)->msgtl_number;
      item_size = ((mach_msg_type_long_t *)msg_ptr)->msgtl_size;
      item_name = ((mach_msg_type_long_t *)msg_ptr)->msgtl_name;
    }
    /* this is to check for older code that had bug */
/*    if (!item_size || item_size != MNBYTESIZE)
      {
	if (item_size != 32) {
	  item_size *= MNBYTESIZE;
	  xTrace1(machripcp, TR_ERRORS, "machnetipc: msg_msg_convert: item size bad value %d", item_size);
	}
      }
*/
    if (datainline)
      item_byte_size = ROUND4((item_size*item_count)/MNBYTESIZE);
    else
      item_byte_size = ROUND4(sizeof(vm_address_t));

    item_ptr = ((char *)msg_ptr) + hdr_size;
    if (!item_byte_size) continue;  /* zero size msg */
    
    if (item_byte_size > msg_size) {
      xTrace4(machripcp, TR_ERRORS, "machnetipc: mach_msg_convert: item size %d (%d %d) exceeds remaining msg length %d",
	      item_byte_size, item_count, item_size, msg_size);
      return;
    }
    
    xTrace5(machripcp, TR_DETAILED, "machnetipc: mach_msg_convert item_count %d item_size %d hdr_size %d msg_size %d msg_name 0x%x",
	    item_count, item_size, hdr_size, msg_size, item_name);
    xTrace1(machripcp, TR_DETAILED, "machnetipc: mach_msg_convert data inline %d", datainline);
    
    switch(item_name) {
      
    case MACH_MSG_TYPE_PORT_RECEIVE:
      goto process_port;
    case MACH_MSG_TYPE_COPY_SEND:
    case MACH_MSG_TYPE_MAKE_SEND:
    case MACH_MSG_TYPE_MAKE_SEND_ONCE:
      xTrace1(machripcp, TR_DETAILED, "machnetipc: data convert: should not have this name %x", item_name);
      goto process_port;
    case MACH_MSG_TYPE_PORT_SEND:
      /* might have triggered no_more_senders; might be new */
      goto process_port;
    case MACH_MSG_TYPE_PORT_SEND_ONCE:
      /* might be time for no_more_senders; might be new */
process_port:
      xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_mach_to_netmach port item");
      if (datainline) {
	port_ptr = (mach_port_t *)(((char *)msg_ptr) + hdr_size);
	port_count = 1;
      }
      else {
	port_ptr = *((mach_port_t **)(((char *)msg_ptr) + hdr_size));
	xTrace1(machripcp, TR_DETAILED, "machnetipc: xk_mach_to_netmach: out of line port(s) at %x",
		port_ptr);
	port_count = item_count;
      }
      for (i=0; i<port_count; i++) {
	mnetport	port_desc;

	/* this will do any port transfer operations */
	{
#ifdef XK_DEBUG
	  extern int traceportmaintp;
	  int savetr = traceportmaintp;
	  int savemtr = tracemachripcp;
/*
	  traceportmaintp = TR_FULL_TRACE;
	  tracemachripcp  = TR_FULL_TRACE;
*/
#endif XK_DEBUG
	  xTrace5(machripcp, TR_DETAILED, "machnetipc: xk_mach_to_netmach: will convert port for msgid %d dest %d.%d.%d.%d",
		  msgid,
		  dest.a, dest.b, dest.c, dest.d);
	  xTrace2(machripcp, TR_DETAILED, "machnetipc: xk_mach_to_netmach: original port type %x resend port type %x",
		  item_name, set_resend_outward_port_type(item_name));
	  port_desc = convert_to_netport(*port_ptr,
					 set_resend_right_type(item_name),
					 dest, msgid, 0);

	  /* save the descriptor address for later unlocking */
	  if (! bcmp((char *)&port_desc, (char *)&Null_Netport,
		     sizeof(port_desc))) {
	    xTrace0(machripcp, TR_SOFT_ERRORS, "machnetipc: xk_mach_convert: Got a null netport from convert_to_netport");
	  }
	  else {
#ifdef PORTLOCKS
	    xTrace1(machripcp, TR_EVENTS, "machnetipc: xk_mach_to_netmach: added port %x to the locked ports list", port_desc.net_port_number);
	    msgPush(&locked_ports, (Pfv)pnumcopy, &port_desc.net_port_number, sizeof(mnportid_t), 0);
	    msg_attr->num_locked_ports++
#endif PORTLOCKS
	      ;
	  }

	  port_desc.net_port_rights = set_resend_right_type(item_name);
	  if (item_name != MACH_MSG_TYPE_PORT_RECEIVE &&
	      *port_ptr != MACH_PORT_NULL)
	    ;

#ifdef XK_DEBUG
	  traceportmaintp = savetr;
	  tracemachripcp  = savemtr;
#endif XK_DEBUG
	}
	if (made_portd == FALSE) {
	  msgConstructAppend(&port_dict_msg, PORT_CHUNK_SIZE, (char **)&port_buffer);
	  made_portd = TRUE;
	}
	msgAppend(&port_dict_msg, (MStoreFun)port_append_func,
		  (mn_netport_t *)&port_desc, sizeof(mn_netport_t),
		  (void *)0, PORT_CHUNK_SIZE);
	bcopy((char *)&port_item_number, ((char *)port_ptr), NETPORTINDEXSIZE);
	port_item_number++;
	port_ptr++;
      }
      if (datainline) break;
    default:
      xTrace0(machripcp, TR_DETAILED, "machnetipc: mach_msg_convert data item");
      if (!datainline) {
	Msg data_msg;
	vm_address_t *local_item_addr = (vm_address_t *)item_ptr;
	int no_bytes;

	xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_mach_msg_convert ool data item");
	if (made_datad == FALSE) {
	  msgConstructEmpty(&outdata_dict_msg);
	  made_datad = TRUE;
	}
	no_bytes = ((MNBYTESIZE - 1) + (item_size * item_count))/MNBYTESIZE;

	if (ool_list_index == MAX_OOL_INDEX) {
	  msgAppend(&ool_list_msg, (Pfv)ool_copy, ools,
		    MAX_OOL_INDEX * sizeof(struct ool_list), 0,
		    MAX_OOL_INDEX * sizeof(struct ool_list));
	  bzero((char *)ools, MAX_OOL_INDEX * sizeof(struct ool_list));
	  ool_list_index = 0;
	}
	ools[ool_list_index].data = *(char **)item_ptr;
	ools[ool_list_index].size = no_bytes;
	ool_list_index++;

	msg_attr->num_ools++;

	msgConstructInplace(&data_msg, (char *) *(int **)item_ptr, no_bytes, (Pfv)no_free_func);
	msgJoin(&outdata_dict_msg, &outdata_dict_msg, &data_msg);
	msgDestroy(&data_msg);
	bcopy((char *)&notinline, (char *)local_item_addr, NETDATAINDEXSIZE);
	notinline++;
      }
      break;
    }
    msg_size -= hdr_size + item_byte_size;
    (char *)msg_ptr  = ((char *)msg_ptr) + hdr_size + item_byte_size;
  }

  /* misuse the sequence num field to indicate whether or not
     a port transfer completion notice will be required          */
  if (mapResolve(xkMsgIdMap, &msgid, &ports) == XK_SUCCESS) {
      ((machnetipc_hdr *)nethdr)->content.machmsg.sequence_num = 1<<ports;
      mapUnbind(xkMsgIdMap, &msgid);
    }

  if (port_item_number ) {
    xTrace1(machripcp, TR_DETAILED, "machnetipc: convert_to_netmsg adds %d ports", port_item_number);
    ((machnetipc_hdr *)nethdr)->content.machmsg.netport_count = port_item_number;
    xIfTrace(machripcp, TR_DETAILED) {
      msgShow(&port_dict_msg);
      msgShow(fulloutmsg);
    }
    msgJoin(fulloutmsg, &port_dict_msg, fulloutmsg);
    msgDestroy(&port_dict_msg);

    xIfTrace(machripcp, TR_DETAILED) {
      msgShow(fulloutmsg);
    }
  }
  if (notinline) {
    xTrace0(machripcp, TR_DETAILED, "machnetipc: convert_to_netmsg adds data");
    msgJoin(fulloutmsg, fulloutmsg, &outdata_dict_msg);
    msgDestroy(&outdata_dict_msg);
    ((machnetipc_hdr *)nethdr)->content.machmsg.notinline_count = notinline;
    msgAppend(&ool_list_msg, (Pfv)ool_copy, ools,
	      (ool_list_index+1) * sizeof(struct ool_list), 0,
	      (ool_list_index+1) * sizeof(struct ool_list));
  }

#ifdef PORTLOCKS  
  msg_attr->locked_ports = locked_ports;
#endif
  msg_attr->ool_list_msg = ool_list_msg;
  msgSetAttr(fulloutmsg, 0, (void *)msg_attr, sizeof(struct msg_attribute));

  /* mach_msg_destroy(msg); */
  xIfTrace(machripcp, TR_DETAILED) {
    printf("machnetipc: convert mach to netmach msg dump:\n");
    msgShow(fulloutmsg);
  }
  xTrace0(machripcp, TR_FULL_TRACE, "machripc: full convert to netmsg exits");
}


/*
 * xk_ports_and_ool_convert
 *
 * Converts incoming message from compatible architecture to local form.
 * Convert all transferred ports, out-of-line data, etc.
 * The two header ports have been converted by the caller
 * The incoming message has had the type field and the two mnetport
 * structures popped off.
 * 
 */
mach_msg_header_t *
xk_ports_and_ool_convert(Msg			*netmsg,
			 int			mach_netport_count,
			 int			mach_notinline_count,
			 msg_id_t		msgid,
			 IPhost			srchost,
			 int                    xfercompletion)
{
  long			xk_msg_length;
  int			mach_msg_length;
  mach_msg_header_t	outmsg;
  mach_msg_header_t	*new_mach_msg;
  mach_msg_type_t	*msg_ptr;
  int			port_seg_count = 0;
  int			outsegcount = 0;
  struct addrinfo {
    char		*index;
    int			 size;
    unsigned int	 type; } *addrs = NULL;
  mn_netport_t		*port_dict = NULL;
  mach_port_t		*local_port_dict = NULL;

  xTrace0(machripcp, TR_FULL_TRACE, "xk_ports_and_ool_convert");

  xk_msg_length = msgLen(netmsg);

  port_seg_count = mach_netport_count;
  xTrace1(machripcp, TR_EVENTS, "machnetipc: ports and ool convert: port seg count %d", port_seg_count);
  xIfTrace(machripcp, TR_FULL_TRACE) {
    msgShow(netmsg);
  }

  if (port_seg_count) {
    int port_size = port_seg_count * sizeof(mn_netport_t);
    int i;

    port_dict = (mn_netport_t *)xMalloc(port_size);
    local_port_dict = (mach_port_t *)xMalloc(port_seg_count * sizeof(mach_port_t));
    msgPop(netmsg, (Pfl)justcopy, port_dict, port_size, (void *)1);

    xTrace0(machripcp, TR_FULL_TRACE, "machnetipc: ports and ool convert: popped port dictionary");
    if (xfercompletion) {
      if (xfercompletion & 2) rrxTransferComplete(srchost, msgid);
      srxTransferComplete(srchost, msgid);
    }

    for (i=0; i<port_seg_count; i++) {
      xTrace2(machripcp, TR_FULL_TRACE, "machnetipc: ports and ool convert: port %x right %x",
	      port_dict[i].net_port_number,
	      port_dict[i].net_port_rights);
      local_port_dict[i] = 
	convert_netport_to_mach_port(&port_dict[i],
				     0 /* self */,
				     ERR_XOBJ /* 0 HASNAIN: lower_session */,
				     0 /* reply_msg */,
				     msgid,
				     srchost);
    }
  }

  if (mach_notinline_count) {
    addrs = (struct addrinfo *)xMalloc(mach_notinline_count * sizeof(struct addrinfo));
  }

  /* get all the contiguous msg into one clump */
  msgPop(netmsg, (Pfl)justcopy_andlie, &outmsg,
	 sizeof(mach_msg_header_t), (void *)1);

  if (outmsg.msgh_size > 0) {
    new_mach_msg = (mach_msg_header_t *)xMalloc(outmsg.msgh_size);

    /* copy entire message into new_mach_msg */
    msgPop(netmsg, (Pfl)justcopy, (char *)new_mach_msg,
	   outmsg.msgh_size, (void *)1);
    msg_ptr = (mach_msg_type_t *)(((char *)new_mach_msg)
				  + sizeof(mach_msg_header_t));
    mach_msg_length = outmsg.msgh_size - sizeof(mach_msg_header_t);
  }
  else {
    xTrace0(machripcp, TR_ERRORS, "machnetipc: ports_and_ool_convert: length 0");
    if (port_dict) xFree((char *)port_dict);
    if (local_port_dict) xFree((char *)local_port_dict);
    return 0;
  }

  xTrace1(machripcp, TR_EVENTS, "machnetipc: convert_ports_and_ool_data: length %d", mach_msg_length);
  if (xk_msg_length < sizeof(mach_msg_header_t)) {
    xTrace2(machripcp, TR_ERRORS, "machnetipc: convert_ports_and_ool_data: msg: bogus length %d dec(%x hex)", xk_msg_length, xk_msg_length);
    xk_msg_length = 0;
    if (port_dict) xFree((char *)port_dict);
    if (local_port_dict) xFree((char *)local_port_dict);
    return 0;
  }

  /* scan through once for all msginline conversions */
  while (mach_msg_length > 0)
    {
      long			item_size;
      /* what if conversions make the message longer?  should we scan
	 first to determine the length?  for now, assume length does
	 not change  */
	int			msginline;
        int			hdr_size;
	unsigned int		segtype;
	unsigned int		segtypesize;
        int			segcount;
        char			*outmsgdata;
        int			longform;
        mach_msg_type_t		*short_hdr;
	mach_msg_type_long_t	*long_hdr;

	if (msg_ptr->msgt_longform) {
	    long_hdr = (mach_msg_type_long_t *)msg_ptr;

	    item_size = long_hdr->msgtl_size;
	    segcount = long_hdr->msgtl_number;
	    /* this is the data block size in bytes */
	    item_size = ((MNBYTESIZE - 1) + (item_size * segcount))/MNBYTESIZE;
	    segtype = long_hdr->msgtl_name;
	    msginline  = ((mach_msg_type_t *)long_hdr)->msgt_inline;
	    hdr_size = MACHNETLONGHDRSIZE;
	    outmsgdata = ((char *)msg_ptr) + sizeof(mach_msg_type_long_t);
	    longform = 1;
	  }
	else
	  {
	    short_hdr = msg_ptr;

	    item_size = short_hdr->msgt_size;
	    segcount = short_hdr->msgt_number;
	    /* this is the data block size in bytes */
	    item_size = ((MNBYTESIZE - 1) + (item_size * segcount))/MNBYTESIZE;
	    segtype = short_hdr->msgt_name;
	    msginline = short_hdr->msgt_inline;
	    hdr_size = MACHNETTYPEHDRSIZE;
	    outmsgdata = ((char *)msg_ptr) + sizeof(mach_msg_type_t);
	    longform = 0;
	  }

      xTrace5(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: size %d count %d name %x inline %d longform %d",
	      item_size, segcount, segtype, msginline, longform);

      if (item_size == 0) {
	(char *)msg_ptr += hdr_size;
	mach_msg_length -= hdr_size;
	xTrace0(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: zero length segment");
	continue;
      }
      segtypesize = item_size_table[segtype];

      switch(segtype) {
	  mach_port_type_t port_send_type;

	case MACH_MSG_TYPE_COPY_SEND:
	case MACH_MSG_TYPE_MAKE_SEND:
	  xTrace4(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: should not have this name 0x%x 0x%x 0x%x 0x%x", segtype, MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND, MACH_MSG_TYPE_PORT_SEND);
	  port_send_type = MACH_MSG_TYPE_PORT_SEND;
	  goto process_port;
	case MACH_MSG_TYPE_MAKE_SEND_ONCE:
	  xTrace1(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: should not have this name 0x%x", segtype);
	  port_send_type = MACH_MSG_TYPE_PORT_SEND_ONCE;
	  goto process_port;
	  /* these are what I expect to see */
	case MACH_MSG_TYPE_PORT_RECEIVE:
	  port_send_type = MACH_MSG_TYPE_MOVE_RECEIVE;
	  goto process_port;
	case MACH_MSG_TYPE_PORT_SEND:
	  port_send_type = MACH_MSG_TYPE_MOVE_SEND;
	  goto process_port;
	case MACH_MSG_TYPE_PORT_SEND_ONCE:
	  port_send_type = MACH_MSG_TYPE_MAKE_SEND_ONCE;

process_port:	

	  xTrace1(machripcp, TR_EVENTS, "machnetipc: ports_and_ool_convert: port type %x", port_send_type);

	    if (longform)
	      ((mach_msg_type_long_t *)msg_ptr)->msgtl_name =
		port_send_type;
	    else
	      ((mach_msg_type_t *)msg_ptr)->msgt_name =
		port_send_type;

	  if (msginline) {
	    mach_port_t local_port;

	    while (segcount-- > 0) {
	      int		port_index;

	      port_index = *((int *)outmsgdata);

	      xTrace1(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: port index %d", port_index);
	      local_port = MACH_PORT_NULL;
	      if (port_index >= 0 && port_index < port_seg_count) {
		  local_port = local_port_dict[port_index];
	      }
	      *(mach_port_t *)outmsgdata = local_port;
	      xTrace2(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: port is %x in msg at addr %x", local_port, outmsgdata);
	      outmsgdata += sizeof(mach_port_t);
	    }
	    ((char *)msg_ptr) += item_size;
	    mach_msg_length -= item_size;
	  }
	  else {
	      xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: ool port(s)");
	      goto processool;
	    }
	  break;
	default:
	  xTrace0(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: data segment");
	  
	  port_send_type = segtype;
	  if (msginline) {
	    ((char *)msg_ptr) += item_size;
	    mach_msg_length -= item_size;
	  }
	  else
	    {
	      kern_return_t ret;
	      char *addr_ptr;

processool:
	      xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: ool data");
	      if ((ret=
		   vm_allocate(mach_task_self(),
			       (vm_address_t *)&addrs[outsegcount++].index,
			       item_size,
			       TRUE)) != KERN_SUCCESS)
		{
		  xTrace1(machripcp, TR_ERRORS, "machnetipc: xk_ports_and_ool_convert: cannot allocate space for incoming data or ports, code %x",
			  ret);
		  addrs[outsegcount-1].index = 0;
		}
	      addr_ptr = ((char *)msg_ptr) + hdr_size;
	      *((vm_address_t **)addr_ptr) = 
		(vm_address_t *)addrs[outsegcount - 1].index;
	      /* should always set the deallocate bit */
	      addrs[outsegcount-1].size = item_size;
	      addrs[outsegcount-1].type = port_send_type;
	      if (longform)
		((mach_msg_type_long_t *)msg_ptr)->msgtl_header.msgt_deallocate = 1;
	      else
		msg_ptr->msgt_deallocate = 1;
	      ((char *)msg_ptr) += sizeof(vm_address_t); /* NETADDRSIZE */
	      mach_msg_length -= sizeof(vm_address_t);
	    }
	  break;
	}
      ((char *)msg_ptr) += hdr_size;
      mach_msg_length -= hdr_size;
    }
  xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: at end of inline processing");

    /* the msg_ptr should be at the end of the message; now we just
       fill in the data at the previosly malloc'd addresses 
     */
  
  xIfTrace(machripcp, TR_ERRORS) {
    if (outsegcount != mach_notinline_count)
      printf("machnetipc: ports_and_ool_convert: out of line segment mismatch: %d %d\n",
	     outsegcount, mach_notinline_count);
  }

  outsegcount = 0;
  while (outsegcount < mach_notinline_count) {
    msgPop(netmsg, (Pfl)justcopy, addrs[outsegcount].index,
	   addrs[outsegcount].size, (void *)1);

    if ((addrs[outsegcount].type == MACH_PORT_TYPE_SEND) ||
	(addrs[outsegcount].type == MACH_PORT_TYPE_SEND_ONCE) ||
	(addrs[outsegcount].type == MACH_PORT_TYPE_RECEIVE) ||
	(addrs[outsegcount].type == MACH_MSG_TYPE_MOVE_RECEIVE) ||
	(addrs[outsegcount].type == MACH_MSG_TYPE_MOVE_SEND) ||
	(addrs[outsegcount].type == MACH_MSG_TYPE_MOVE_SEND_ONCE)) {

      int i;
      mach_port_t *port_ptr = (mach_port_t *)addrs[outsegcount].index;
      
      xTrace0(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: ool ports");

      /* step through the popped off data; should increment by netsize */
      for (i=0; i<addrs[outsegcount].size; i++) {
	*port_ptr = local_port_dict[*((int *)port_ptr)];
	port_ptr++;
      }
    }
    outsegcount++;
  }
  xTrace0(machripcp, TR_EVENTS, "machnetipc: ports and ool data convert: normal return");

  if (addrs) xFree((char *)addrs);
  if (port_dict) xFree((char *)port_dict);
  if (local_port_dict) xFree((char *)local_port_dict);
  return (mach_msg_header_t *)new_mach_msg;
}

/*
 * set_resend_outward_port_type
 *
 *    message going to net needs standard port type
 */

static mach_port_type_t
set_resend_outward_port_type(segtype)
     mach_port_type_t  segtype;  /* mach msg name type */
{
  switch ( segtype ) {
  case MACH_MSG_TYPE_MOVE_SEND_ONCE:
  case MACH_MSG_TYPE_MAKE_SEND_ONCE:
    return MACH_MSG_TYPE_PORT_SEND_ONCE;
  case MACH_MSG_TYPE_COPY_SEND:
  case MACH_MSG_TYPE_MAKE_SEND:
    return MACH_MSG_TYPE_PORT_SEND;
  case MACH_MSG_TYPE_MOVE_RECEIVE:
    return MACH_MSG_TYPE_PORT_RECEIVE;
  }
  return segtype;
}

/*
 * set_resend_outward_port_type
 *
 *    message going to net needs standard port type
 */

static mach_port_right_t
set_resend_right_type(segtype)
     mach_port_type_t  segtype;  /* mach msg name type */
{
  switch ( segtype ) {
  case MACH_MSG_TYPE_MOVE_SEND_ONCE:
  case MACH_MSG_TYPE_MAKE_SEND_ONCE:
    return MACH_PORT_TYPE_SEND_ONCE;
  case MACH_MSG_TYPE_COPY_SEND:
  case MACH_MSG_TYPE_MAKE_SEND:
  case MACH_MSG_TYPE_MOVE_SEND:
    return MACH_PORT_TYPE_SEND;
  case MACH_MSG_TYPE_MOVE_RECEIVE:
    return MACH_PORT_TYPE_RECEIVE;
  }
  xTrace1(machripcp, TR_ERRORS, "machnetipc: set_resend_right_type: unknown seg type: %x", segtype);
  return segtype;
}

/*
 * need_to_swap
 *
 *
 *
 */
int need_to_swap(data_type)
     int data_type;
{
  switch (data_type) {

/*  case MACH_MSG_TYPE_BIT:
  case MACH_MSG_TYPE_BOOLEAN: */
  case MACH_MSG_TYPE_UNSTRUCTURED:
/*  case MACH_MSG_TYPE_BYTE: */
  case MACH_MSG_TYPE_INTEGER_8:
  case MACH_MSG_TYPE_CHAR:
/*  case MACH_MSG_TYPE_STRING: */
  case MACH_MSG_TYPE_STRING_C:
    return 0;

  case MACH_MSG_TYPE_INTEGER_16:
  case MACH_MSG_TYPE_INTEGER_32:
  case MACH_MSG_TYPE_REAL:
    return 1;

  default:
    xTrace1(machripcp, TR_ERRORS, "machnetipc: need_to_swap: unknown msg type 0x%x", data_type);
    return 0;
  }
}



/*
 *  data_pop_and_swap
 *
 *
 */
long data_pop_and_swap(output, input, len, arg)
     char *output;
     char *input;
     long  len;
     struct datargs *arg;
{
  int           size, i;
  float         flt;

  xTrace0(machripcp, TR_FULL_TRACE, "machnetipc: data_pop_and_swap starts");

  size = item_size_table[arg->data_type];
  i = len;
  while (i > 0) {
    /* apply conversion function */
    switch(arg->data_type) {
    case MACH_MSG_TYPE_INTEGER_16:
      *(unsigned short *)output = unpermute_int16[arg->arch_index](input);
      break;
      
    case MACH_MSG_TYPE_INTEGER_32:
      *(unsigned int *)output = unpermute_int32[arg->arch_index](input);
      break;
      
    case MACH_MSG_TYPE_REAL:
      flt = unpermute_real32[arg->arch_index](input);
      bcopy((char *)&flt, output, size);
      break;
    }

    output += size;
    input += size;
    i -= size;
  }
  return len;
}


/*
 *  port_hdr_pop_and_swap
 *
 *
 */
long port_hdr_pop_and_swap(outptr, inptr, len, argf)
     mn_netport_t *outptr;
     char *inptr;
     long len;
     void *(*argf[])();
{
  int i;

  xTrace0(machripcp, TR_FULL_TRACE, "machnetipc: port_hdr_pop_and_swap starts");

  /* GENERALIZE for size of elements */

  for (i=0; i < len; i += sizeof(mn_netport_t)) {
    bcopy(inptr, (char *)&outptr->net_port_number, PORTID_NETLEN);
    outptr->net_port_number = (int)argf[PORTID_NETTYPE]((char *)&outptr->net_port_number);
    inptr += PORTID_NETLEN;
    
    bcopy (inptr, (char *)&outptr->net_port_rights, PORTRIGHT_NETLEN);
    outptr->net_port_rights = (int)argf[PORTRIGHT_NETTYPE]((char *)&outptr->net_port_rights);
    inptr += PORTRIGHT_NETLEN;
    
    bcopy (inptr, (char *)&outptr->receiver_host_addr, HOSTNETLEN);
    inptr += HOSTNETLEN;
    
    bcopy (inptr, (char *)&outptr->make_send_count, NETMAKESENDSIZE);
    outptr->make_send_count = (int)argf[NETMAKESENDTYPE]((char *)&outptr->make_send_count);
    inptr += NETMAKESENDSIZE;

    outptr++;
  }

  return(len);
}

/*
 *  port_append_func
 *
 *    move a brief description of the port to the outgoing message
 *
 */
static long
port_append_func(input, output, len, arg)
     mnetport *input;
     char     *output;
     int       len;
     struct datargs *arg;
{
  bcopy((char *)input, output, sizeof(mn_netport_t));
  return sizeof(mn_netport_t);
}

#ifdef LOCALMSGAPPEND
static void
msgAppendLocal(msgarg, func, data, length, newlength)
     struct msgappendargs *msgarg;
     Pfv func;
     void *data;
     long length;
     long newlength;
{
  char *fmsg;
  char *cmsg;
  char *buffer;
  long  blen;
  char *buffertail;

  fmsg = &msgarg->msg;
  cmsg =  msgarg->cmsg;
  buffer = msgarg->buffer;
  blen = msgarg->blen;
  buffertail = msgarg->buffertail;
  
  if (!buffer) {
    msgConstructAllocate(fmsg, newlength, &msgarg->buffer);
    msgarg->blen = newlength;
    msgarg->buffertail = msgarg->buffer;
    msgarg->cmsg = fmsg;
  }
  if (buffertail + length > buffer + blen) {
    msgTruncate(cmsg, buffertail - buffer);
    if (cmsg != fmsg) msgJoin(fmsg, fmsg, cmsg);
    msgConstructAllocate(&msgarg->rcmsg, newlength, &msgarg->buffer);
    msgarg->blen = newlength;
    msgarg->buffertail = msgarg->buffer;
    msgarg->cmsg = &msgarg->rcmsg;
    cmsg = msgarg->cmsg;
    buffer = msgarg->buffer;
  }
  bcopy(data, buffer, length);
}

static void
msgAppendDone(msgarg)
     struct msgappendargs *msgarg;
{
  if (!msgarg->buffer) msgConstructEmpty(&msgarg->msg);
  else {
    msgTruncate(msgarg->cmsg, msgarg->buffertail - msgarg->buffer);
    msgJoin(&msgarg->msg, &msgarg->msg, msgarg->cmsg);
    msgarg->buffer = 0;
  }
}
#endif LOCALMSGAPPEND

/*
 * longhdr_pop
 *
 * Skip the msgtl_header part - was already converted.
 */
static long 
longhdr_pop( output, input, len, argf)
     mach_msg_type_long_t  *output;
     char *input;
     long  len;
     void  *(*argf[])();
{
  char *first_input = input;
  
  xTrace2(machripcp, TR_DETAILED, "longhdr_pop: %x %x", input, output);

  /* skip the msgtl_header field */
  input += 4;
  bcopy(input, (char *)&output->msgtl_name, 4);
  output->msgtl_name = (int)argf[MACH_MSG_TYPE_INTEGER_16](&output->msgtl_name);
  input += 2;
  bcopy(input, (char *)&output->msgtl_size, 2);
  output->msgtl_size = (int)argf[MACH_MSG_TYPE_INTEGER_16](&output->msgtl_size);
  input += 2;
  bcopy(input, (char *)&output->msgtl_number, 4);
  output->msgtl_number = (int)argf[MACH_MSG_TYPE_INTEGER_32](&output->msgtl_number);
  return input - first_input + 4; 
}

/*
 * justcopy
 *
 *
 */
static long 
justcopy( input, output, len, direction)
     char *input;
     char  *output;
     long  len;
     void *direction;
{
  xTrace5(machripcp, TR_DETAILED, "justcopy: %x %x %ld (0x%lx) dir %d", input, output, len, len, direction);

  if (!direction) 
    bcopy(input, output, len);
  else
    bcopy(output, input, len);
  return len;
}

/*
 * justcopy_andlie
 *
 *
 */
static long
justcopy_andlie( input, output, len, direction)
     char *input;
     char  *output;
     long  len;
     void *direction;
{
  xTrace4(machripcp, TR_DETAILED, "justcopy_andlie: %x %x dir %d len %ld", input, output, direction, len);

  if (!direction)
    bcopy(input, output, len);
  else
    bcopy(output, input, len);
  return 0;
}

/*
 * xk_netmach_msg_to_mach
 *
 * Converts incoming message from other architecture to local form.
 * Convert all transferred ports, out-of-line data, byte swaps, etc.
 * The two header ports have been converted by the caller
 * The incoming message has had the type field and the two mnetport
 * structures popped off.
 * 
 */
mach_msg_header_t *
xk_netmach_msg_to_mach(Msg			*netmsg,
			 int			mach_netport_count,
			 int			mach_notinline_count,
			 msg_id_t		msgid,
			 IPhost			srchost,
			 int                    xfercompletion,
                         mn_arch_tag_t		arch_type)
{
  long			xk_msg_length;
  int			mach_msg_length;
  mach_msg_header_t	outmsg;
  mach_msg_header_t	*new_mach_msg;
  mach_msg_type_t	*msg_ptr;
  int			port_seg_count = 0;
  int			outsegcount = 0;
  struct addrinfo {
    char		*index;
    int			 size;
    unsigned int	 type;
    int                  port_flag;} *addrs = NULL;
  mn_netport_t		*port_dict = NULL;
  mach_port_t		*local_port_dict = NULL;
  void			(*convec[MAX_MACH_DATA_TYPE])();
  int                    arch_index = arch_unpermute_index(arch_type);

  xTrace0(machripcp, TR_FULL_TRACE, "xk_ports_and_ool_convert");

  /* might as well just have one for each architecture type all ready */
  set_convert_vector(arch_type, convec);

  xk_msg_length = msgLen(netmsg);

  port_seg_count = mach_netport_count;
  xTrace1(machripcp, TR_EVENTS, "machnetipc: ports and ool convert: port seg count %d", port_seg_count);
  xIfTrace(machripcp, TR_FULL_TRACE) {
    msgShow(netmsg);
  }

  if (port_seg_count) {
    int port_size = port_seg_count * sizeof(mn_netport_t);
    int i;

    port_dict = (mn_netport_t *)xMalloc(port_size);
    local_port_dict = (mach_port_t *)xMalloc(port_seg_count * sizeof(mach_port_t));

/***** byte swap ports: check again *****/
    msgPop(netmsg, (Pfl)port_hdr_pop_and_swap, port_dict, port_size, convec);

    xTrace0(machripcp, TR_FULL_TRACE, "machnetipc: ports and ool convert: popped port dictionary");
    if (xfercompletion) {
      if (xfercompletion & 2) rrxTransferComplete(srchost, msgid);
      srxTransferComplete(srchost, msgid);
    }

    for (i=0; i<port_seg_count; i++) {
      xTrace2(machripcp, TR_FULL_TRACE, "machnetipc: ports and ool convert: port %x right %x",
	      port_dict[i].net_port_number,
	      port_dict[i].net_port_rights);
      local_port_dict[i] = 
	convert_netport_to_mach_port(&port_dict[i],
				     0 /* self */,
				     ERR_XOBJ /* lower_session */,
				     0 /* reply_msg */,
				     msgid,
				     srchost);
    }
  }

  if (mach_notinline_count) {
    addrs = (struct addrinfo *)xMalloc(mach_notinline_count * sizeof(struct addrinfo));
  }

  /* get all the contiguous msg into one clump */
  msgPop(netmsg, (Pfl)justcopy_andlie, &outmsg,
	 sizeof(mach_msg_header_t), (void *)1);

/***** byte swap message header - I just need the size field ******/
  if (arch_index) {
    outmsg.msgh_size = unpermute_int32[arch_index]((char *)&outmsg.msgh_size);
  }

  if (outmsg.msgh_size > 0) {
    new_mach_msg = (mach_msg_header_t *)xMalloc(outmsg.msgh_size);
    msgPop(netmsg, (Pfl)justcopy, (char *)new_mach_msg,
	   outmsg.msgh_size, (void *)1);

    /* update the header fields: bits, local and remote port
       will be set outside this routine */
    new_mach_msg->msgh_size = outmsg.msgh_size;
    new_mach_msg->msgh_id=unpermute_int32[arch_index]((char *)&outmsg.msgh_id);
    new_mach_msg->msgh_seqno=unpermute_int32[arch_index]((char *)&outmsg.msgh_seqno);

    msg_ptr = (mach_msg_type_t *)(((char *)new_mach_msg)
				  + sizeof(mach_msg_header_t));
    mach_msg_length = outmsg.msgh_size - sizeof(mach_msg_header_t);
  }
  else {
    xTrace0(machripcp, TR_ERRORS, "machnetipc: ports_and_ool_convert: length 0");
    if (port_dict) xFree((char *)port_dict);
    if (local_port_dict) xFree((char *)port_dict);
    if (addrs) xFree((char *)addrs);
    return 0;
  }

  xTrace1(machripcp, TR_EVENTS, "machnetipc: convert_ports_and_ool_data: length %d", mach_msg_length);
  if (xk_msg_length < sizeof(mach_msg_header_t)) {
    xTrace2(machripcp, TR_ERRORS, "machnetipc: convert_ports_and_ool_data: msg: bogus length %d dec(%x hex)", xk_msg_length, xk_msg_length);
    xk_msg_length = 0;
    if (port_dict) xFree((char *)port_dict);
    if (local_port_dict) xFree((char *)port_dict);
    if (addrs) xFree((char *)addrs);
    return 0;
  }

  /* scan through once for all msginline conversions */
  while (mach_msg_length > 0)
    {
      long			item_size;
      /* what if conversions make the message longer?  should we scan
	 first to determine the length?  for now, assume length does
	 not change  */
	int			msginline;
        int			hdr_size;
	unsigned int		segtype;
	unsigned int		segtypesize;
        int			segcount;
        char			*outmsgdata;
        int			longform;
        mach_msg_type_t		short_hdr;
	mach_msg_type_long_t	long_hdr;

/***** byte swap the type descriptor pointed to by msg_ptr but dont save it back yet ******/
        short_hdr = unpermute_msg_type[arch_index]((char *)msg_ptr);

	if (short_hdr.msgt_longform) {
/***** byte swap the long header : and save ******/
	    /* will only convert the name, size and number fields */
	    longhdr_pop(&long_hdr, (char *)msg_ptr, 0, (void *(**)())convec);
	    *(mach_msg_type_long_t *)msg_ptr = long_hdr;
	    *msg_ptr = short_hdr;

	    item_size = long_hdr.msgtl_size;
	    segcount = long_hdr.msgtl_number;
	    /* this is the data block size in bytes */
	    item_size = ((MNBYTESIZE - 1) + (item_size * segcount))/MNBYTESIZE;
	    segtype = long_hdr.msgtl_name;
	    msginline  = short_hdr.msgt_inline;
	    hdr_size = MACHNETLONGHDRSIZE;
	    outmsgdata = ((char *)msg_ptr) + sizeof(mach_msg_type_long_t);
	    longform = 1;
	  }
	else
	  {
/******* save the byte swapped short header back to msg *********/	    
	    *msg_ptr = short_hdr;

	    item_size = short_hdr.msgt_size;
	    segcount = short_hdr.msgt_number;
	    /* this is the data block size in bytes */
	    item_size = ((MNBYTESIZE - 1) + (item_size * segcount))/MNBYTESIZE;
	    segtype = short_hdr.msgt_name;
	    msginline = short_hdr.msgt_inline;
	    hdr_size = MACHNETTYPEHDRSIZE;
	    outmsgdata = ((char *)msg_ptr) + sizeof(mach_msg_type_t);
	    longform = 0;
	  }

      xTrace5(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: size %d count %d name %x inline %d longform %d",
	      item_size, segcount, segtype, msginline, longform);

      if (item_size == 0) {
	(char *)msg_ptr += hdr_size;
	mach_msg_length -= hdr_size;
	xTrace0(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: zero length segment");
	continue;
      }
      segtypesize = item_size_table[segtype];

      switch(segtype) {
	  mach_port_type_t port_send_type;

	case MACH_MSG_TYPE_COPY_SEND:
	case MACH_MSG_TYPE_MAKE_SEND:
	  xTrace4(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: should not have this name 0x%x 0x%x 0x%x 0x%x", segtype, MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND, MACH_MSG_TYPE_PORT_SEND);
	  port_send_type = MACH_MSG_TYPE_PORT_SEND;
	  goto process_port;
	case MACH_MSG_TYPE_MAKE_SEND_ONCE:
	  xTrace1(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: should not have this name 0x%x", segtype);
	  port_send_type = MACH_MSG_TYPE_PORT_SEND_ONCE;
	  goto process_port;
	  /* these are what I expect to see */
	case MACH_MSG_TYPE_PORT_RECEIVE:
	  port_send_type = MACH_MSG_TYPE_MOVE_RECEIVE;
	  goto process_port;
	case MACH_MSG_TYPE_PORT_SEND:
	  port_send_type = MACH_MSG_TYPE_MOVE_SEND;
	  goto process_port;
	case MACH_MSG_TYPE_PORT_SEND_ONCE:
	  port_send_type = MACH_MSG_TYPE_MAKE_SEND_ONCE;

process_port:	

	  xTrace1(machripcp, TR_EVENTS, "machnetipc: ports_and_ool_convert: port type %x", port_send_type);
	  if (longform)
	    ((mach_msg_type_long_t *)msg_ptr)->msgtl_name =
	      port_send_type;
	  else
	    ((mach_msg_type_t *)msg_ptr)->msgt_name =
	      port_send_type;

	  if (msginline) {
	    mach_port_t local_port;

	    while (segcount-- > 0) {
	      int		port_index;

/***** byte swap port index ******/
	      port_index = unpermute_int32[arch_index](outmsgdata);

	      xTrace1(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: port index %d", port_index);
	      local_port = MACH_PORT_NULL;
	      if (port_index >= 0 && port_index < port_seg_count) {
		local_port = local_port_dict[port_index];
/*
		&& quick_netport_lookup(port_dict[port_index].net_port_number,
					 &portdesc)
		    == XK_SUCCESS) {
		local_port = portdesc->real_receive_port;
*/
	      }
	      *(mach_port_t *)outmsgdata = local_port;

	      xTrace2(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: port is %x in msg at addr %x", local_port, outmsgdata);
	      outmsgdata += sizeof(mach_port_t);
	    }
	    ((char *)msg_ptr) += item_size;
	    mach_msg_length -= item_size;
	  }
	  else {
	      xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: ool port(s)");
	      goto processool;
	    }
	  break;
	default:
	  xTrace0(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: data segment");

          port_send_type = segtype;
	  if (msginline) {

	    if (arch_index) { 
	      struct datargs data_arg;
	      long len = item_size;
	      char *inlinedata;

	      /********* have to byte swap in line data ***********/	    

	      data_arg.arch_index = arch_index;
	      data_arg.data_type = segtype;
	      
	      inlinedata = ((char *)msg_ptr) + hdr_size;

	      if (need_to_swap(data_arg.data_type))
		data_pop_and_swap((char *)inlinedata, (char *)inlinedata, len,
				  (void *)&data_arg);
	    }

	    ((char *)msg_ptr) += item_size;
	    mach_msg_length -= item_size;
	  }
	  else
	    {
	      kern_return_t ret;
	      char *addr_ptr;

processool:
	      xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: ool data");
	      if ((ret=
		   vm_allocate(mach_task_self(),
			       (vm_address_t *)&addrs[outsegcount++].index,
			       item_size,
			       TRUE)) != KERN_SUCCESS)
		{
		  xTrace1(machripcp, TR_ERRORS, "machnetipc: xk_ports_and_ool_convert: cannot allocate space for incoming data or ports, code %x",
			  ret);
		  addrs[outsegcount-1].index = 0;
		}
	      addr_ptr = ((char *)msg_ptr) + hdr_size;
	      *((vm_address_t **)addr_ptr) = 
		(vm_address_t *)addrs[outsegcount - 1].index;
	      addrs[outsegcount-1].size = item_size;

	      addrs[outsegcount-1].port_flag =
		(port_send_type == MACH_MSG_TYPE_PORT_SEND ||
		 port_send_type == MACH_MSG_TYPE_PORT_SEND_ONCE ||
		 port_send_type == MACH_MSG_TYPE_MOVE_RECEIVE ||
		 port_send_type == MACH_MSG_TYPE_MOVE_SEND ||
		 port_send_type == MACH_MSG_TYPE_MOVE_SEND_ONCE);

	      if (addrs[outsegcount-1].port_flag)
		addrs[outsegcount-1].type = MACH_MSG_TYPE_INTEGER_32;
	      else
		addrs[outsegcount-1].type = port_send_type;

	      /* should always set the deallocate bit */
	      if (longform)
		((mach_msg_type_long_t *)msg_ptr)->msgtl_header.msgt_deallocate = 1;
	      else
		msg_ptr->msgt_deallocate = 1;

	      ((char *)msg_ptr) += sizeof(vm_address_t); /* NETADDRSIZE */
	      mach_msg_length -= sizeof(vm_address_t);
	    }
	  break;
	}
      ((char *)msg_ptr) += hdr_size;
      mach_msg_length -= hdr_size;
    }
  xTrace0(machripcp, TR_DETAILED, "machnetipc: xk_ports_and_ool_convert: at end of inline processing");

    /* the msg_ptr should be at the end of the message; now we just
       fill in the data at the previosly malloc'd addresses 
     */
  
  xIfTrace(machripcp, TR_ERRORS) {
    if (outsegcount != mach_notinline_count)
      printf("machnetipc: ports_and_ool_convert: out of line segment mismatch: %d %d\n",
	     outsegcount, mach_notinline_count);
  }

  outsegcount = 0;
  while (outsegcount < mach_notinline_count) {
    struct datargs data_arg;

    data_arg.arch_index = arch_index;
    data_arg.data_type = addrs[outsegcount].type;

/******** byte swap the ool data : use type of data ******/

    if (need_to_swap(data_arg.data_type))
      msgPop(netmsg, (Pfl)data_pop_and_swap, addrs[outsegcount].index,
	     addrs[outsegcount].size, (void *)&data_arg);
    else
      msgPop(netmsg, (Pfl)justcopy, addrs[outsegcount].index,
	     addrs[outsegcount].size, (void *)1);

    if (addrs[outsegcount].port_flag) {
      int i;
      mach_port_t *port_ptr = (mach_port_t *)addrs[outsegcount].index;

      xTrace0(machripcp, TR_DETAILED, "machnetipc: ports and ool convert: ool ports");
      /* step through the popped off data; should increment by netsize */
      for (i=0; i<addrs[outsegcount].size; i += sizeof(mach_port_t)) {
	*port_ptr = local_port_dict[*((int *)port_ptr)];
	port_ptr++;
      }
    }
    outsegcount++;
  }
  xTrace0(machripcp, TR_EVENTS, "machnetipc: ports and ool data convert: normal return");
  if (port_dict) xFree((char *)port_dict);
  if (local_port_dict) xFree((char *)port_dict);
  if (addrs) xFree((char *)addrs);
  return (mach_msg_header_t *)new_mach_msg;
}
