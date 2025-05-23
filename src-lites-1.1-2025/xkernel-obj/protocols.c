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


XObj		protl_tab[10+1];
static XObj	argv[10];


void
build_pgraph_dev()
{
}


void
build_pgraph()
{

  /*
   *  building protocol ETHDRV
   */
  protl_tab[0] = xCreateProtl(ethdrv_init, "ethdrv", "wd0",
#ifdef XK_DEBUG
			&traceethdrvp,
#else
			0,
#endif
				0, argv);
  if ( protl_tab[0] == ERR_XOBJ ) {
      Kabort("Could not create ethdrv protocol");
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
   *  building protocol ICMP
   */
  argv[0] = protl_tab[4];
  protl_tab[5] = xCreateProtl(icmp_init, "icmp", "",
#ifdef XK_DEBUG
			&traceicmpp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[5] == ERR_XOBJ ) {
      Kabort("Could not create icmp protocol");
  }


  /*
   *  building protocol TCP
   */
  argv[0] = protl_tab[4];
  protl_tab[6] = xCreateProtl(tcp_init, "tcp", "",
#ifdef XK_DEBUG
			&tracetcpp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[6] == ERR_XOBJ ) {
      Kabort("Could not create tcp protocol");
  }


  /*
   *  building protocol UDP
   */
  argv[0] = protl_tab[4];
  protl_tab[7] = xCreateProtl(udp_init, "udp", "",
#ifdef XK_DEBUG
			&traceudpp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[7] == ERR_XOBJ ) {
      Kabort("Could not create udp protocol");
  }


  /*
   *  building protocol XKSOCKET
   */
  argv[0] = protl_tab[6];
  argv[1] = protl_tab[7];
  protl_tab[8] = xCreateProtl(xksocket_init, "xksocket", "",
#ifdef XK_DEBUG
			&tracexksocketp,
#else
			0,
#endif
				2, argv);
  if ( protl_tab[8] == ERR_XOBJ ) {
      Kabort("Could not create xksocket protocol");
  }


  /*
   *  building protocol UDPTEST
   */
  argv[0] = protl_tab[7];
  protl_tab[9] = xCreateProtl(udptest_init, "udptest", "",
#ifdef XK_DEBUG
			&traceudptestp,
#else
			0,
#endif
				1, argv);
  if ( protl_tab[9] == ERR_XOBJ ) {
      Kabort("Could not create udptest protocol");
  }

}
