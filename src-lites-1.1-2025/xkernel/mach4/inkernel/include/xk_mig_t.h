/*     
 * $RCSfile: xk_mig_t.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.8 $
 * $Date: 1993/09/21 00:24:00 $
 */

#ifndef XK_MIG_T_H
#define XK_MIG_T_H

#include "part.h"
#include "xk_mig_sizes.h"
#include "msg.h"


#ifdef XKMACHKERNEL
#  define PORT_TYPE	ipc_port_t
#  include <ipc/ipc_port.h>
#else
#  define PORT_TYPE	mach_port_t
#endif


typedef	char		xk_string_80_t[80];

typedef xk_string_80_t 	xk_string_t;

typedef int		xobj_ext_id_t;

typedef struct {
    int			type;
    xobj_ext_id_t	hlpRcv;
    xobj_ext_id_t	hlpType;
    xobj_ext_id_t	myprotl;
    xk_string_t		name;
    xk_string_t		instName;
} xk_xobj_dump_t;

typedef int		xk_part_t[PART_EXT_BUF_LEN];

typedef struct {
    int	type;
    int	len;
} xk_msg_info_t;

typedef	char	*xk_msg_data_t;
typedef	char	*xk_msg_attr_t;
typedef	char	*xk_large_msg_data_t;

typedef struct {
    VOID	*machMsg;
    Msg		*xkMsg;
} xk_and_mach_msg_t;

typedef char	xk_ctl_buf_t[XK_MAX_CTL_BUF_LEN];

#define offsetof(type, mem) ((int) \
			((char *)&((type *) 0)->mem - (char *)((type *) 0)))

#endif ! XK_MIG_T_H

