/* 
 * $RCSfile: vdisorder_i.h,v $
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/05/05 22:48:56 $
 */


#define	VDISORDER_ACT_MAP_SZ		11
#define	VDISORDER_PAS_MAP_SZ		11
#define VDISORDER_MAX_INTERVAL		20
#define VDISORDER_MAX_QUEUED_MESSAGES	10

#define VDISORDER_STATE_PASSTHROUGH	1
#define VDISORDER_STATE_DISORDERING	2
#define VDISORDER_STATE_RELEASE		3

#define VDISORDER_DEF_ARRIVAL_INTERVAL	5
#define VDISORDER_DEF_DISORDER_INTERVAL	3

#ifdef XK_DEBUG
#define VDISORDER_DEF_TIMEOUT_INTERVAL	3000
#else
#define VDISORDER_DEF_TIMEOUT_INTERVAL	1
#endif

typedef struct {
    Map		activeMap;
    Map 	passiveMap;
} PState;

typedef struct {
    int		disorderState;
    int		arrivalInterval;
    int		arrivalCount;
    int		disorderInterval;
    int		disorderCount;
    int		timeoutInterval;
    Event	disorderEvent;
    Msg		disorderMsg[VDISORDER_MAX_QUEUED_MESSAGES];
} SState;

