/*
 *
 * nnstest.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.1 $
 * $Date: 1993/11/15 21:41:58 $
 */

/* 
 * nnstest.c:
 *
 *     A sample test program to test the netmsgserver using the netname 
 *     interface implemented in the nns layer of the server. This test
 *     accepts command line options to perform name checkins, checkouts 
 *     and lookups. 
 *
 *     Usuage examples:
 *
 *     1.    nnstest -i hello &
 *           checks in the service hello.
 *
 *     2.    nnstest -l hello 
 *           looks up the service hello.
 *
 *     3.    nnstest -ilo hello 
 *           performs a checkin(i), lookup(l) and checkout of the
 *           service hello.
 *
 *     4.    nnstest -hmachine -l hello
 *           does a remote lookup on machine for service hello.
 *            
 *     5.    nnstest -h'*' -l hello
 *           does a broadcast lookup for service hello on the local
 *           network. Note '*' is in quotes to bypass the shell.
 */

#include <stdio.h>
#include <cthreads.h>
#include <mach.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <mach/mach_port.h>
#include <servers/netname.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/resource.h>

#define NAME_SERVER "NetworkNameServer"

static void usage(name)
char *name;
{
  printf("Usage: %s [-hmachine_name] -[ilo] service_name\n", name);
  exit(-1);
}

void main(argc, argv)
int argc;
char *argv[];
{
  kern_return_t ret;
  mach_port_right_t server_registry_port;
  mach_msg_type_name_t returned_type;
  mach_port_right_t ns_port;
  mach_port_t signature, sig_send;
  int i, j=0;
  char hostname[100];
  char name[100];
  char options[100];
  int checkin, lookup = 0;
  struct rusage rbefore, rafter;
  struct timeval tbefore, tafter;

  printf("\nStart of nns test\n");

  *hostname = '\0';
  *name = '\0';
  *options = '\0';

  for (i=1; argv[i]; i++) {
    if (!strncmp(argv[i], "-h", 2))
      strcpy(hostname, argv[i]+2);
    else if (*argv[i] == '-')
      strcpy(options, argv[i]+1);
    else
      strcpy(name, argv[i]);
  }

  if (*name == '\0') usage(argv[0]);

  ret = mach_port_allocate(mach_task_self(),
			   MACH_PORT_RIGHT_RECEIVE,
			   &signature);
  if (ret != KERN_SUCCESS) {
    printf("Error: Could not create signature port %d(x%x)\n", ret, ret);
    exit(-19);
  }

  ret = mach_port_extract_right(mach_task_self(),
				signature,
				MACH_MSG_TYPE_MAKE_SEND,
				&sig_send,
				&returned_type); 
  if (ret != KERN_SUCCESS) {
    printf("Error: Could not create a send right to signature port %d(x%x)\n", ret, ret);
    exit(-20);
  }
  

  ret = mach_port_allocate(mach_task_self(),
			   MACH_PORT_RIGHT_RECEIVE,
			   &server_registry_port);
  if (ret != KERN_SUCCESS) {
    printf("Error: Could not create a server port %d(x%x)\n", ret, ret);
    exit(-1);
  }

  /* get the name server port for the specified host */
  printf("Looking for the name server port for host \"%s\"...", hostname);
  fflush(stdout);

  /* look for the network name server port on this host */
  ret = netname_look_up(name_server_port, "", NAME_SERVER, &name_server_port);
  if (ret != NETNAME_SUCCESS) {
    printf("Could not get the network name server port on my host %d(%x)\n", ret, ret);
    exit(-1);
  }

  /* look for the network name server port on remote host */
  ret = netname_look_up(name_server_port, hostname, NAME_SERVER, &name_server_port);
  if (ret != NETNAME_SUCCESS) {
    printf("Could not get the network name server port on host \"%s\" %d(%x)\n", hostname, ret, ret);
    exit(-1);
  }

  printf("\n");
  fflush(stdout);

  if (*hostname != '*')
    *hostname = '\0';

  checkin = 0;
  for (i=0; options[i]; i++) {
    if (options[i] == 'l') {
      printf("Looking up server \"%s\" on host %s...", name, hostname);
      fflush(stdout);
      lookup = 1;

      ret = netname_look_up(name_server_port, hostname, name, &ns_port);
      if (ret != NETNAME_SUCCESS)
	printf(" Lookup failed");
      else
	printf(" Lookup succeeded");
      printf("\n");
      fflush(stdout);
    }
    else if (options[i] == 'i') {
      printf("Checking in server port as \"%s\"...", name);
      fflush(stdout);
      ret = netname_check_in(name_server_port, name, 
			     sig_send, server_registry_port);
      if (ret != NETNAME_SUCCESS) {
	printf(" Checkin failed: %d(x%x)", ret, ret);
	exit(-1);
      }
      else {
	printf(" Checkin succeeded");
      }
      printf("\n");
      fflush(stdout);
      checkin = 1;
    }
    else if (options[i] == 'o') {
      printf("Checking out server port; signature = %d...", sig_send);
      fflush(stdout);
      ret = netname_check_out(name_server_port, name, sig_send);
      if (ret != NETNAME_SUCCESS)
	printf(" Checkout failed: %d(x%x)", ret, ret);
      else
	printf(" Checkout succeeded");
      printf("\n");
      fflush(stdout);
      checkin = 0;
    }
  }

  if (checkin) {
    printf("\nLooping infinitely; Kill me to check out the service!\n");
    for (; 1; ) system("sleep 100");
  }

  printf("End of nns test\n\n");
  exit(0);
}
