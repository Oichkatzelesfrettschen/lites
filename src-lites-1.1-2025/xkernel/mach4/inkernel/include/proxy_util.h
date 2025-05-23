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
#ifdef __STDC__
					VOID *, int
#endif
					);
typedef struct {
    bool		valid;
    DeallocFunc		func;
    VOID		*arg;
    int			len;
} LingeringMsg;



#ifdef __STDC__

void		lingeringMsgClear( void );
void		lingeringMsgSave( DeallocFunc, VOID *, int );
bool		msgIsOkToMangle( Msg *m, char **machMsg, int offset );
bool		msgIsContiguous( Msg *m );
xkern_return_t	msgToOol( Msg *, char **, DeallocFunc *, VOID **arg );
void		oolToMsg( char *, int, Msg * );

#  ifdef XKMACHKERNEL

void	oolFreeIncoming( VOID *, int );
void	oolFreeOutgoing( VOID *, int );

#  else

/* 
 * Deallocate the virtual memory region containing 'p'.  
 */
void	oolFree( VOID *, int );

#  endif  /* XKMACHKERNEL */

#else	/* ! __STDC__ */

void	lingeringMsgClear();
void	lingeringMsgSave();
bool	msgIsContiguous();
bool	msgIsOkToMangle();

#  ifdef XKMACHKERNEL

void	oolFreeIncoming();
void	oolFreeOutgoing();

#  else

/* 
 * Deallocate the virtual memory region containing 'p'.  
 */
void	oolFree();

#  endif  /* XKMACHKERNEL */

#endif /* __STDC__ */


#endif /* ! proxy_util_h */
