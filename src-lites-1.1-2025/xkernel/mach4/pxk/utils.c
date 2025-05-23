/*
 * utils.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.21 $
 * $Date: 1993/02/13 00:00:33 $
 */

#include <stdio.h>

#include "xk_debug.h"
#include "x_libc.h"
#include "x_util.h"
#include "upi.h"


void
Kabort(s)
    char *s;
{
#ifdef XKMACH4
    printf("xkernel abort: %s\n", s);
#else
    fprintf(stderr, "xkernel abort: %s\n", s);
#endif
    xRapture();
    abort();
}

