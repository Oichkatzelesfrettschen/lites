#ifndef	_xk_lproxy_user_
#define	_xk_lproxy_user_

/* Module xk_lproxy */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <mach/std_types.h>
#include <mach/mach_types.h>
#include "xk_mig_sizes.h"
#include "xk_mig_t.h"
#include "assert.h"
#include "proxy_offset.h"

/* Routine dumpXObj */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_lproxy_dumpXObj
#if	defined(LINTLIBRARY)
    (reqPort, retVal, obj, dump)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t obj;
	xk_xobj_dump_t *dump;
{ return call_lproxy_dumpXObj(reqPort, retVal, obj, dump); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t obj,
	xk_xobj_dump_t *dump
);
#endif

/* Routine xOpenDone */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_lproxy_xOpenDone
#if	defined(LINTLIBRARY)
    (reqPort, retVal, lls, llp, hlpRcv, hlpType)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t lls;
	xobj_ext_id_t llp;
	xobj_ext_id_t hlpRcv;
	xobj_ext_id_t hlpType;
{ return call_lproxy_xOpenDone(reqPort, retVal, lls, llp, hlpRcv, hlpType); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xobj_ext_id_t llp,
	xobj_ext_id_t hlpRcv,
	xobj_ext_id_t hlpType
);
#endif

/* Routine xDemux */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_lproxy_xDemux
#if	defined(LINTLIBRARY)
    (reqPort, retVal, lls, req, reqCnt, msgStart, msgOol, msgOolCnt, reqAttr, reqAttrCnt)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t lls;
	xk_msg_data_t req;
	mach_msg_type_number_t reqCnt;
	int msgStart;
	xk_large_msg_data_t msgOol;
	mach_msg_type_number_t msgOolCnt;
	xk_msg_attr_t reqAttr;
	mach_msg_type_number_t reqAttrCnt;
{ return call_lproxy_xDemux(reqPort, retVal, lls, req, reqCnt, msgStart, msgOol, msgOolCnt, reqAttr, reqAttrCnt); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xk_msg_data_t req,
	mach_msg_type_number_t reqCnt,
	int msgStart,
	xk_large_msg_data_t msgOol,
	mach_msg_type_number_t msgOolCnt,
	xk_msg_attr_t reqAttr,
	mach_msg_type_number_t reqAttrCnt
);
#endif

/* Routine xCallDemux */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_lproxy_xCallDemux
#if	defined(LINTLIBRARY)
    (reqPort, retVal, lls, req, reqCnt, reqMsgStart, reqool, reqoolCnt, rep, repCnt, repool, repoolCnt, reqAttr, reqAttrCnt)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t lls;
	xk_msg_data_t req;
	mach_msg_type_number_t reqCnt;
	int reqMsgStart;
	xk_large_msg_data_t reqool;
	mach_msg_type_number_t reqoolCnt;
	xk_msg_data_t rep;
	mach_msg_type_number_t *repCnt;
	xk_large_msg_data_t *repool;
	mach_msg_type_number_t *repoolCnt;
	xk_msg_attr_t reqAttr;
	mach_msg_type_number_t reqAttrCnt;
{ return call_lproxy_xCallDemux(reqPort, retVal, lls, req, reqCnt, reqMsgStart, reqool, reqoolCnt, rep, repCnt, repool, repoolCnt, reqAttr, reqAttrCnt); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xk_msg_data_t req,
	mach_msg_type_number_t reqCnt,
	int reqMsgStart,
	xk_large_msg_data_t reqool,
	mach_msg_type_number_t reqoolCnt,
	xk_msg_data_t rep,
	mach_msg_type_number_t *repCnt,
	xk_large_msg_data_t *repool,
	mach_msg_type_number_t *repoolCnt,
	xk_msg_attr_t reqAttr,
	mach_msg_type_number_t reqAttrCnt
);
#endif

/* Routine ping */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_lproxy_ping
#if	defined(LINTLIBRARY)
    (reqPort)
	mach_port_t reqPort;
{ return call_lproxy_ping(reqPort); }
#else
(
	mach_port_t reqPort
);
#endif

#endif	/* not defined(_xk_lproxy_user_) */
