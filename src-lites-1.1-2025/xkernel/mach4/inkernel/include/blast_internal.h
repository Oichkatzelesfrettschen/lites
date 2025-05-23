/* 
 * blast_internal.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.37 $
 * $Date: 1993/07/21 16:28:43 $
 */

#ifndef blast_internal_h
#define blast_internal_h
 
#include "xkernel.h"
#include "blast.h"
#include "blast_mask16.h"
#include "blast_stack.h"
#include "chan.h"

#define BLASTHLEN  sizeof(BLAST_HDR) 

/*
 * Trace levels
 */
#define REXMIT_T TR_MAJOR_EVENTS

/* 
 * opcodes in the message header
 */
#define BLAST_SEND 1
#define BLAST_NACK 2
#define BLAST_RETRANSMIT 3

#define BLAST_OK 2
#define BLAST_FAIL 2
 
typedef unsigned int	BlastSeq;
#define BLAST_MAX_SEQ	0xffffffff

typedef struct  {
    u_int 		prot_id;
    BlastSeq 		seq;
    u_int 		len;
    u_short 		num_frag;
    u_char 		op;
    char		pad[BLAST_PADDING];
    BlastMask 		mask;
} BLAST_HDR;

typedef struct {
    BLAST_HDR 	hdr;
    Msg 	frags[BLAST_MAX_FRAGS+1];
    BlastMask 	mask;
    BlastMask 	old_mask;
    u_int 	wait;
    Bind 	binding;
    struct blast_state *state;
    Event 	event;
    int 	nacks_sent;
    int 	nacks_recieved;
    bool	canChangeBackoff;
    int		rcnt;
} MSG_STATE;


typedef struct blast_state {
    u_int	ircnt;    
    long	prot_id;
    BLAST_HDR 	short_hdr;
    BLAST_HDR 	cur_hdr;
    XObj 	self;
    int 	fragmentSize;
    int		per_packet;
    Map 	rec_map;
} SState;

typedef struct {
    Map 	active_map;
    Semaphore	createSem;
    Map 	passive_map;
    BlastSeq	max_seq;
    Semaphore 	outstanding_messages;
    int 	max_outstanding_messages;
    Stack	mstateStack;
    Map 	send_map;
} PState;

typedef struct {
        long	prot;
	XObj	lls;
} ActiveID;

typedef int PassiveID;
	
/*
 * timeout calculations:
 */
#define PER_PACKET	10 * 1000
#define BACKOFF 	2
#define MAX_PER_PACKET	(INT_MAX/BACKOFF)
#define SEND_CONST 	1000 * 1000
#define MAX_WAIT	1000 * 1000
#define NACK_LIMIT	3


/* Default number of concurrent outstanding messages allowed
 * (per protocol instantiation)
 */
#define OUTSTANDING_MESSAGES 64

#define BLAST_ACTIVE_MAP_SZ	101
#define BLAST_SEND_MAP_SZ	101
#define BLAST_REC_MAP_SZ	101
#define BLAST_PASSIVE_MAP_SZ	13
#define BLAST_MSTATE_STACK_SZ	(2 * OUTSTANDING_MESSAGES)


/* If BLAST_SIM_DROPS is defined, BLAST will occasionally drop a fragment
 * before it gets to the appropriate BLAST session.
 */
/* #define BLAST_SIM_DROPS */


extern BlastMask	blastFullMask[];
extern int 		traceblastp;

#ifdef __STDC__

xmsg_handle_t	blastPush( XObj, Msg * );
int		blastControlSessn( XObj, int, char *, int );
int		blastControlProtl( XObj, int, char *, int );
void		blastDecIrc( XObj );
MSG_STATE *	blastNewMstate( XObj );
xkern_return_t	blastPop( XObj, XObj, Msg *, VOID * );
xkern_return_t	blastDemux( XObj, XObj, Msg * );
XObj		blastCreateSessn( XObj, XObj, XObj, ActiveID * );
void		blast_mapFlush( Map );
int		blast_freeSendSeq( PState *, int );
int		blast_Retransmit( PState *, int );
long		blastHdrLoad( void *, char *, long, void * );
void		blastHdrStore( void *, char *, long, void * );
xkern_return_t	blastSenderPop( XObj, Msg *, BLAST_HDR * );
int		blast_mask_to_i( BLAST_MASK_PROTOTYPE );

void	blast_phdr( BLAST_HDR * );
char *	blastOpStr( int );
void	blastShowActiveKey( ActiveID *key, char *message );
void	blastShowMstate( MSG_STATE *m, char *message );
char *	blastShowMask( BLAST_MASK_PROTOTYPE );


#else

xmsg_handle_t	blastPush();
int		blastControlSessn();
int		blastControlProtl();
void		blastDecIrc();
xkern_return_t	blastPop();
xkern_return_t	blastDemux();
XObj		blastCreateSessn();
void		blast_mapFlush();
int		blast_freeSendSeq();
int		blast_Retransmit();
long		blastHdrLoad();
void		blastHdrStore();
MSG_STATE *	blastNewMstate();
xkern_return_t	blastSenderPop();
int		blast_mask_to_i();

#ifdef XK_DEBUG
void	blast_phdr();
char *	blastOpStr();
void	blastShowActiveKey();
void	blastShowMstate();
char *	blastShowMask();
#endif XK_DEBUG

#endif __STDC__

#endif

