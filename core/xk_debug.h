/*
 * xk_debug.h - Minimal stub for Lites 1.1.u3
 *
 * x-kernel debug macros - disabled for this build
 * Original from x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 */

#ifndef xk_debug_h
#define xk_debug_h

/* Disable XK_DEBUG for legacy build */
#undef XK_DEBUG

/* Define trace variables as disabled (0) */
extern int
	traceipp,
	traceicmpp,
	tracetcpp;

/* All trace macros become no-ops when XK_DEBUG is not defined */
#define xIfTrace(t, l) if (0)
#define xTrace0(t, l, f)
#define xTrace1(t, l, f, arg1)
#define xTrace2(t, l, f, arg1, arg2)
#define xTrace3(t, l, f, arg1, arg2, arg3)
#define xTrace4(t, l, f, arg1, arg2, arg3, arg4)
#define xTrace5(t, l, f, arg1, arg2, arg3, arg4, arg5)

#endif	/* xk_debug_h */
