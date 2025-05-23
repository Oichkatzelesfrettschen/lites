/*
 * time.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.10 $
 * $Date: 1993/02/02 00:05:23 $
 */

#ifdef XKMACHKERNEL

#include <mach/time_value.h>
extern time_value_t time;

#else

#include <sys/time.h>

#endif XKMACHKERNEL

#include "xtime.h"

void
xGetTime(where)
XTime *where;
{
#ifdef XKMACHKERNEL
  where->sec = time.seconds;
  where->usec = time.microseconds;
#else
#ifdef XKMACH4
  host_get_time(mach_host_self(), where);
#if 0
  where->sec = 0;		/* FAKE! TODO! PNR */
  where->usec = 0;
#endif
#else
  gettimeofday((struct timeval *)where, 0);
#endif XKMACH4
#endif XKMACHKERNEL
}


