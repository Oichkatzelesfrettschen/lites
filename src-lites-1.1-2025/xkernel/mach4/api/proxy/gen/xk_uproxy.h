#ifndef	_xk_uproxy_user_
#define	_xk_uproxy_user_

/* Module xk_uproxy */

#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <mach/std_types.h>
#include "xk_mig_sizes.h"
#include "xk_mig_t.h"
#include "assert.h"
#include "proxy_offset.h"

/* Routine abort */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_abort
#if	defined(LINTLIBRARY)
    (reqPort, lproxyPort)
	mach_port_t reqPort;
	mach_port_t lproxyPort;
{ return call_uproxy_abort(reqPort, lproxyPort); }
#else
(
	mach_port_t reqPort,
	mach_port_t lproxyPort
);
#endif

/* Routine xGetProtlByName */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xGetProtlByName
#if	defined(LINTLIBRARY)
    (reqPort, name, protl)
	mach_port_t reqPort;
	xk_string_80_t name;
	xobj_ext_id_t *protl;
{ return call_uproxy_xGetProtlByName(reqPort, name, protl); }
#else
(
	mach_port_t reqPort,
	xk_string_80_t name,
	xobj_ext_id_t *protl
);
#endif

/* Routine xControl */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xControl
#if	defined(LINTLIBRARY)
    (reqPort, obj, opcode, buf, bufLen)
	mach_port_t reqPort;
	xobj_ext_id_t obj;
	int opcode;
	xk_ctl_buf_t buf;
	int *bufLen;
{ return call_uproxy_xControl(reqPort, obj, opcode, buf, bufLen); }
#else
(
	mach_port_t reqPort,
	xobj_ext_id_t obj,
	int opcode,
	xk_ctl_buf_t buf,
	int *bufLen
);
#endif

/* Routine xOpenEnable */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xOpenEnable
#if	defined(LINTLIBRARY)
    (reqPort, lproxyPort, retVal, hlpRcv, hlpType, llp, part)
	mach_port_t reqPort;
	mach_port_t lproxyPort;
	xkern_return_t *retVal;
	xobj_ext_id_t hlpRcv;
	xobj_ext_id_t hlpType;
	xobj_ext_id_t llp;
	xk_part_t part;
{ return call_uproxy_xOpenEnable(reqPort, lproxyPort, retVal, hlpRcv, hlpType, llp, part); }
#else
(
	mach_port_t reqPort,
	mach_port_t lproxyPort,
	xkern_return_t *retVal,
	xobj_ext_id_t hlpRcv,
	xobj_ext_id_t hlpType,
	xobj_ext_id_t llp,
	xk_part_t part
);
#endif

/* Routine xOpenDisable */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xOpenDisable
#if	defined(LINTLIBRARY)
    (reqPort, lproxyPort, retVal, hlpRcv, hlpType, llp, part)
	mach_port_t reqPort;
	mach_port_t lproxyPort;
	xkern_return_t *retVal;
	xobj_ext_id_t hlpRcv;
	xobj_ext_id_t hlpType;
	xobj_ext_id_t llp;
	xk_part_t part;
{ return call_uproxy_xOpenDisable(reqPort, lproxyPort, retVal, hlpRcv, hlpType, llp, part); }
#else
(
	mach_port_t reqPort,
	mach_port_t lproxyPort,
	xkern_return_t *retVal,
	xobj_ext_id_t hlpRcv,
	xobj_ext_id_t hlpType,
	xobj_ext_id_t llp,
	xk_part_t part
);
#endif

/* Routine xOpen */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xOpen
#if	defined(LINTLIBRARY)
    (reqPort, lproxyPort, retObj, hlpRcv, hlpType, llp, part)
	mach_port_t reqPort;
	mach_port_t lproxyPort;
	xobj_ext_id_t *retObj;
	xobj_ext_id_t hlpRcv;
	xobj_ext_id_t hlpType;
	xobj_ext_id_t llp;
	xk_part_t part;
{ return call_uproxy_xOpen(reqPort, lproxyPort, retObj, hlpRcv, hlpType, llp, part); }
#else
(
	mach_port_t reqPort,
	mach_port_t lproxyPort,
	xobj_ext_id_t *retObj,
	xobj_ext_id_t hlpRcv,
	xobj_ext_id_t hlpType,
	xobj_ext_id_t llp,
	xk_part_t part
);
#endif

/* Routine dumpXObj */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_dumpXObj
#if	defined(LINTLIBRARY)
    (reqPort, retVal, obj, dump)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t obj;
	xk_xobj_dump_t *dump;
{ return call_uproxy_dumpXObj(reqPort, retVal, obj, dump); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t obj,
	xk_xobj_dump_t *dump
);
#endif

/* Routine xDuplicate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xDuplicate
#if	defined(LINTLIBRARY)
    (reqPort, lproxyPort, retVal, hlp, obj)
	mach_port_t reqPort;
	mach_port_t lproxyPort;
	xkern_return_t *retVal;
	xobj_ext_id_t hlp;
	xobj_ext_id_t obj;
{ return call_uproxy_xDuplicate(reqPort, lproxyPort, retVal, hlp, obj); }
#else
(
	mach_port_t reqPort,
	mach_port_t lproxyPort,
	xkern_return_t *retVal,
	xobj_ext_id_t hlp,
	xobj_ext_id_t obj
);
#endif

/* Routine xPush */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xPush
#if	defined(LINTLIBRARY)
    (reqPort, handle, lls, xmsg, xmsgCnt, xmsgool, xmsgoolCnt, attr, attrCnt)
	mach_port_t reqPort;
	xmsg_handle_t *handle;
	xobj_ext_id_t lls;
	xk_msg_data_t xmsg;
	mach_msg_type_number_t xmsgCnt;
	xk_large_msg_data_t xmsgool;
	mach_msg_type_number_t xmsgoolCnt;
	xk_msg_attr_t attr;
	mach_msg_type_number_t attrCnt;
{ return call_uproxy_xPush(reqPort, handle, lls, xmsg, xmsgCnt, xmsgool, xmsgoolCnt, attr, attrCnt); }
#else
(
	mach_port_t reqPort,
	xmsg_handle_t *handle,
	xobj_ext_id_t lls,
	xk_msg_data_t xmsg,
	mach_msg_type_number_t xmsgCnt,
	xk_large_msg_data_t xmsgool,
	mach_msg_type_number_t xmsgoolCnt,
	xk_msg_attr_t attr,
	mach_msg_type_number_t attrCnt
);
#endif

/* Routine xCall */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xCall
#if	defined(LINTLIBRARY)
    (reqPort, retVal, lls, reqmsg, reqmsgCnt, reqool, reqoolCnt, repmsg, repmsgCnt, repool, repoolCnt, reqAttr, reqAttrCnt)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t lls;
	xk_msg_data_t reqmsg;
	mach_msg_type_number_t reqmsgCnt;
	xk_large_msg_data_t reqool;
	mach_msg_type_number_t reqoolCnt;
	xk_msg_data_t repmsg;
	mach_msg_type_number_t *repmsgCnt;
	xk_large_msg_data_t *repool;
	mach_msg_type_number_t *repoolCnt;
	xk_msg_attr_t reqAttr;
	mach_msg_type_number_t reqAttrCnt;
{ return call_uproxy_xCall(reqPort, retVal, lls, reqmsg, reqmsgCnt, reqool, reqoolCnt, repmsg, repmsgCnt, repool, repoolCnt, reqAttr, reqAttrCnt); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls,
	xk_msg_data_t reqmsg,
	mach_msg_type_number_t reqmsgCnt,
	xk_large_msg_data_t reqool,
	mach_msg_type_number_t reqoolCnt,
	xk_msg_data_t repmsg,
	mach_msg_type_number_t *repmsgCnt,
	xk_large_msg_data_t *repool,
	mach_msg_type_number_t *repoolCnt,
	xk_msg_attr_t reqAttr,
	mach_msg_type_number_t reqAttrCnt
);
#endif

/* Routine xClose */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_xClose
#if	defined(LINTLIBRARY)
    (reqPort, retVal, lls)
	mach_port_t reqPort;
	xkern_return_t *retVal;
	xobj_ext_id_t lls;
{ return call_uproxy_xClose(reqPort, retVal, lls); }
#else
(
	mach_port_t reqPort,
	xkern_return_t *retVal,
	xobj_ext_id_t lls
);
#endif

/* Routine ping */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_ping
#if	defined(LINTLIBRARY)
    (reqPort)
	mach_port_t reqPort;
{ return call_uproxy_ping(reqPort); }
#else
(
	mach_port_t reqPort
);
#endif

/* Routine pingtest */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t call_uproxy_pingtest
#if	defined(LINTLIBRARY)
    (reqPort, repPort, number)
	mach_port_t reqPort;
	mach_port_t repPort;
	int number;
{ return call_uproxy_pingtest(reqPort, repPort, number); }
#else
(
	mach_port_t reqPort,
	mach_port_t repPort,
	int number
);
#endif

#endif	/* not defined(_xk_uproxy_user_) */
