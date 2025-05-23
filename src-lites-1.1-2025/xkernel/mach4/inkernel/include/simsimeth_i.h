/* 
 * simsimeth_i.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/03/26 17:06:20 $
 */

#define MAX_ETH_SIZE (EMAXPAK+sizeof(ETHhdr))

#define MAX_ERROR_COUNT 1000


#define CLEAR_REF(_bp) (_bp)->ref = NOREF

#define EMAXPAK 	(MAX_ETH_DATA_SZ + sizeof(ETHhdr))



typedef struct {
    XObj	self;
    Msg		*msg;
} EthMsg;

typedef struct {
    XObj	arp;
    Map		map;
    int		port;
    ETHhost	myHost;
} PState;

#define simsimethMapSize 40 

