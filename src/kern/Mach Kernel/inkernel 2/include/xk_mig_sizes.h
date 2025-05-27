/*     
 * $RCSfile: xk_mig_sizes.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.6 $
 * $Date: 1993/11/08 21:18:18 $
 */


#define XOBJ_EXT_ID_LEN		4

#define XK_MAX_CTL_BUF_LEN	200

/* 
 * Max number of bytes needed to externalize a participant list.
 * 5 participants at 10 stack elements each pointing at 20 bytes (+ overhead)
 */
#define PART_EXT_BUF_LEN	( 4 + 5 * (4 + 10 * (4 + 20)) )


/* 
 * length in 'int's
 */
#define XOBJ_DUMP_LEN		( (( 4 + 3 * XOBJ_EXT_ID_LEN + 2 * 80 ) + 3) / 4 )

#define XK_INLINE_THRESHOLD	4000
#define XK_MAX_MSG_INLINE_LEN	4000
#define XK_MAX_MSG_ATTR_LEN	200

#define XK_MAX_MIG_MSG_LEN	(XK_MAX_MSG_INLINE_LEN + 1000)

#define XK_MSG_INFO_LEN		2	/* size in int's */
