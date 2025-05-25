/*
 * protocol initialization calls
 *
 * Warning: this file is generated from graph.comp and is overwritten
 * every time 'make compose' is run
 */

#include "upi.h"
#include "x_util.h"
#include "compose.h"
#include "protocols.h"


XObj		protl_tab[7+1];
static XObj	argv[7];


void
build_pgraph_dev()
{
}


void
build_pgraph()
{

  /*
   *  building protocol XKLANCE
   */
  protl_tab[0] = xCreateProtl(xklance_init, "xklance", "SE0",
#ifdef XK_DEBUG
			&tracexklancep,
#else
			0,
#endif
				0, argv);
  if ( protl_tab[0] == ERR_XOBJ ) {
      Kabort("Could not create xklance protocol");
  }


  /*
   *  building protocol ETH
   */
  argv[0] = protl_tab[0];
  protl_tab[1] = xCreateProtl(eth_init, "eth", "",
#ifdef XK_DEBUG
			&traceethp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[1] == ERR_XOBJ ) {
      Kabort("Could not create eth protocol");
  }


  /*
   *  building protocol ARP
   */
  argv[0] = protl_tab[1];
  protl_tab[2] = xCreateProtl(arp_init, "arp", "",
#ifdef XK_DEBUG
			&tracearpp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[2] == ERR_XOBJ ) {
      Kabort("Could not create arp protocol");
  }


  /*
   *  building protocol VNET
   */
  argv[0] = protl_tab[1];
  argv[1] = protl_tab[2];
  protl_tab[3] = xCreateProtl(vnet_init, "vnet", "",
#ifdef XK_DEBUG
			&tracevnetp,
#else
			0,
#endif
				2, argv);
  if ( protl_tab[3] == ERR_XOBJ ) {
      Kabort("Could not create vnet protocol");
  }


  /*
   *  building protocol IP
   */
  argv[0] = protl_tab[3];
  protl_tab[4] = xCreateProtl(ip_init, "ip", "",
#ifdef XK_DEBUG
			&traceipp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[4] == ERR_XOBJ ) {
      Kabort("Could not create ip protocol");
  }


  /*
   *  building protocol TCP
   */
  argv[0] = protl_tab[4];
  protl_tab[5] = xCreateProtl(tcp_init, "tcp", "",
#ifdef XK_DEBUG
			&tracetcpp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[5] == ERR_XOBJ ) {
      Kabort("Could not create tcp protocol");
  }


  /*
   *  building protocol TCPTEST
   */
  argv[0] = protl_tab[5];
  protl_tab[6] = xCreateProtl(tcptest_init, "tcptest", "",
#ifdef XK_DEBUG
			&tracetcptestp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[6] == ERR_XOBJ ) {
      Kabort("Could not create tcptest protocol");
  }

}
