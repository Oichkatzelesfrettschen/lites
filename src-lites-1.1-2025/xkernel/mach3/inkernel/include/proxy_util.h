/*     
 * $RCSfile: proxy_util.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.3 $
 * $Date: 1993/07/16 00:47:18 $
 */

#ifndef proxy_util_h
#define proxy_util_h


typedef 	void	(* DeallocFunc)(
					VOID *, int
					);
typedef struct {
    bool		valid;
    DeallocFunc		func;
    VOID		*arg;
    int			len;
} LingeringMsg;




void		lingeringMsgClear( void );
void		lingeringMsgSave( DeallocFunc, VOID *, int );
bool		msgIsOkToMangle( Msg *m, char **machMsg, int offset );
bool		msgIsContiguous( Msg *m );
xkern_return_t	msgToOol( Msg *, char **, DeallocFunc *, VOID **arg );
void		oolToMsg( char *, int, Msg * );

#  ifdef XKMACHKERNEL

void	oolFreeIncoming( VOID *, int );
void	oolFreeOutgoing( VOID *, int );




#endif /* ! proxy_util_h */
