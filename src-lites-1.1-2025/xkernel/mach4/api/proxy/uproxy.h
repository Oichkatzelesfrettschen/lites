/*     
 * $RCSfile: uproxy.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.9 $
 * $Date: 1993/09/21 00:23:23 $
 */

#ifdef __STDC__

kern_return_t	do_uproxy_abort( PORT_TYPE,
				PORT_TYPE);

kern_return_t	do_uproxy_dumpXObj(PORT_TYPE,
				   xkern_return_t *,
				   xobj_ext_id_t,
				   xk_xobj_dump_t *);

kern_return_t	do_uproxy_ping( PORT_TYPE );

kern_return_t	do_uproxy_pingtest( PORT_TYPE,
				   PORT_TYPE,
				   int );

kern_return_t	do_uproxy_xClose( PORT_TYPE,
				 xkern_return_t *,
				 xobj_ext_id_t );

kern_return_t	do_uproxy_xControl( PORT_TYPE,
				   xobj_ext_id_t,
				   int,
				   xk_ctl_buf_t,
				   int *);

kern_return_t	do_uproxy_xDuplicate( PORT_TYPE,
				     PORT_TYPE,
				     xkern_return_t *,
				     xobj_ext_id_t,
				     xobj_ext_id_t );

kern_return_t	do_uproxy_xOpenEnable( PORT_TYPE,
				      PORT_TYPE,
				      xkern_return_t *,
				      xobj_ext_id_t,
				      xobj_ext_id_t,
				      xobj_ext_id_t,
				      xk_part_t );

kern_return_t	do_uproxy_xOpenDisable( PORT_TYPE,
				       PORT_TYPE,
				       xkern_return_t *,
				       xobj_ext_id_t,
				       xobj_ext_id_t,
				       xobj_ext_id_t,
				       xk_part_t );

kern_return_t	do_uproxy_xOpen( PORT_TYPE,
				PORT_TYPE,
				xobj_ext_id_t *,
				xobj_ext_id_t,
				xobj_ext_id_t,
				xobj_ext_id_t,
				xk_part_t );

kern_return_t	do_uproxy_xGetProtlByName( PORT_TYPE,
					  xk_string_t,
					  xobj_ext_id_t * );

kern_return_t	do_uproxy_xPush( PORT_TYPE,
				xmsg_handle_t *,
				xobj_ext_id_t,
				char *,
				mach_msg_type_number_t,
				char *,
				mach_msg_type_number_t,
				boolean_t,
				xk_msg_attr_t,
				mach_msg_type_number_t );

kern_return_t	do_uproxy_xCall( PORT_TYPE,
				xkern_return_t *,
				xobj_ext_id_t,
				char *,
				mach_msg_type_number_t,
				char *,
				mach_msg_type_number_t,
				boolean_t,
				char *,
				mach_msg_type_number_t *,
				char **,
				mach_msg_type_number_t *,
				xk_msg_attr_t,
				mach_msg_type_number_t );

#endif
