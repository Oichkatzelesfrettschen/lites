/*     
 * $RCSfile: lproxy.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.11 $
 * $Date: 1993/09/16 22:02:19 $
 */

#ifdef __STDC__

kern_return_t	do_lproxy_dumpXObj( mach_port_t,
				    xkern_return_t *,
				    xobj_ext_id_t,
				    xk_xobj_dump_t * );

kern_return_t	do_lproxy_ping( mach_port_t );

kern_return_t	do_lproxy_xCallDemux( mach_port_t,
				      xkern_return_t *,
				      xobj_ext_id_t,
				      xk_msg_data_t,
				      mach_msg_type_number_t,
				      int,
				      xk_large_msg_data_t,
				      mach_msg_type_number_t,
				      boolean_t,
				      xk_msg_data_t,
				      mach_msg_type_number_t *,
				      xk_large_msg_data_t *,
				      mach_msg_type_number_t *,
				      xk_msg_attr_t,
				      mach_msg_type_number_t
				     );

kern_return_t	do_lproxy_xDemux(mach_port_t,
				 xkern_return_t *,
				 xobj_ext_id_t,
				 xk_msg_data_t,
				 mach_msg_type_number_t,
				 int,
				 xk_large_msg_data_t,
				 mach_msg_type_number_t,
				 boolean_t,
				 xk_msg_attr_t,
				 mach_msg_type_number_t
				);


kern_return_t	do_lproxy_xOpenDone( mach_port_t,
				    xkern_return_t *,
				    xobj_ext_id_t,
				    xobj_ext_id_t,
				    xobj_ext_id_t,
				    xobj_ext_id_t );

#endif

