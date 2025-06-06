/* 
 * arp_table.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.6 $
 * $Date: 1993/02/01 22:19:26 $
 */

typedef struct arpent	*ArpTbl;

/*
 * arpLookup -- Find the ETH host equivalent of the given IP host.
 * If the value is not in the table, network ARP requests will be sent.
 * Returns 0 if the lookup was successful and -1 if it was not.
 */
int	arpLookup(
		  XObj, IPhost *, ETHhost *
		  );

/*
 * arpRevLookup -- Find the IP host equivalent of the given ETH host.
 * If the value is not in the table, network RARP requests will be sent.
 * Returns 0 if the lookup was successful and -1 if it was not.
 */
int	arpRevLookup(
		     XObj, IPhost *, ETHhost *
		     );

/*
 * arpRevLookupTable -- Find the IP host equivalent of the given ETH host.
 * Only looks in the local table, does not send network requests.
 * Returns 0 if the lookup was successful and -1 if it was not.
 */
int	arpRevLookupTable(
			  XObj, IPhost *, ETHhost *
			  );

/*
 * Initialize the arp table.
 */
ArpTbl	arpTableInit(
		     void
		     );

/*
 * Save the IPhost<->ETHhost binding, releasing any previous bindings
 * that either of these addresses might have had.  Unblocks processes
 * waiting for this binding.  One of ip or eth can be
 * null, in which case the blocked processes will be freed and told that
 * the address could not be resolved
 */
void 	arpSaveBinding(
		       ArpTbl, IPhost *ip, ETHhost *eth
		       );


/* 
 * Remove all entries from the table which are not on the same subnet
 * as the given host.  Entries will be removed regardless of lock
 * status. 
 */
void
arpTblPurge(
	    ArpTbl, IPhost *
	    );


/*
 * arpLock -- lock the entry with the given IP host so the entry always
 * remains in the cache.
 */
void	arpLock(
		ArpTbl, IPhost *h
		);


/*
 * arpForEach -- call the function in ArpForEach for each of the
 * entries in the arp table
 */
void
arpForEach( 
    ArpTbl, ArpForEach *
	   );
