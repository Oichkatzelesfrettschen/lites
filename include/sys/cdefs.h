/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */

#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

/*
 * Testing against Clang-specific extensions.
 */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#ifndef __has_extension
#define __has_extension(x) 0
#endif
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

/*
 * Compiler-dependent macros to declare that functions take printf-like
 * or scanf-like arguments. They are null except for versions of gcc
 * that are known to support the features properly.
 */
#if defined(__GNUC__) && (__GNUC__ > 2 || __GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define __printflike(fmtarg, firstvararg) \
    __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#define __scanflike(fmtarg, firstvararg) \
    __attribute__((__format__ (__scanf__, fmtarg, firstvararg)))
#else
#define __printflike(fmtarg, firstvararg)
#define __scanflike(fmtarg, firstvararg)
#endif

/*
 * Compiler-dependent macros to declare that functions never return.
 */
#if defined(__GNUC__) && (__GNUC__ > 2 || __GNUC__ == 2 && __GNUC_MINOR__ >= 5)
#define __dead __attribute__((__noreturn__))
#define __noreturn __attribute__((__noreturn__))
#else
#define __dead
#define __noreturn
#endif

/*
 * Compiler-dependent macros for pure functions (no side effects).
 */
#if defined(__GNUC__) && (__GNUC__ > 2 || __GNUC__ == 2 && __GNUC_MINOR__ >= 96)
#define __pure __attribute__((__pure__))
#else
#define __pure
#endif

/*
 * Compiler-dependent macros for unused variables/functions.
 */
#if defined(__GNUC__)
#define __unused __attribute__((__unused__))
#else
#define __unused
#endif

/*
 * Compiler-dependent macros for packed structures.
 */
#if defined(__GNUC__)
#define __packed __attribute__((__packed__))
#else
#define __packed
#endif

/*
 * Compiler-dependent macros for aligned data.
 */
#if defined(__GNUC__)
#define __aligned(x) __attribute__((__aligned__(x)))
#else
#define __aligned(x)
#endif

/*
 * Keywords for C99 and later.
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __restrict restrict
#else
#define __restrict
#endif

/*
 * Compiler-dependent macros for indicating that a function may be inlined.
 */
#if defined(__GNUC__) || defined(__clang__)
#define __always_inline __attribute__((__always_inline__))
#else
#define __always_inline
#endif

/*
 * NULL pointer definition
 */
#ifndef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

/*
 * ANSI C / POSIX feature test macros
 */
#if defined(__STDC__)
#define __CONCAT(x,y)   x ## y
#define __STRING(x)     #x
#else
#define __CONCAT(x,y)   x/**/y
#define __STRING(x)     "x"
#endif

/*
 * GCC visibility attributes
 */
#if defined(__GNUC__) && __GNUC__ >= 4
#define __dso_public    __attribute__((__visibility__("default")))
#define __dso_hidden    __attribute__((__visibility__("hidden")))
#else
#define __dso_public
#define __dso_hidden
#endif

/*
 * Macro to test if we're using a GNU C compiler of a specific vintage
 * or later, for e.g. features that appeared in a particular version
 * of GNU C.  Usage:
 *
 *	#if __GNUC_PREREQ__(major, minor)
 *	...cool feature...
 *	#else
 *	...delete feature...
 *	#endif
 */
#ifdef __GNUC__
#define __GNUC_PREREQ__(x, y) \
    ((__GNUC__ > (x)) || (__GNUC__ == (x) && __GNUC_MINOR__ >= (y)))
#else
#define __GNUC_PREREQ__(x, y) 0
#endif

/*
 * The __BEGIN_DECLS and __END_DECLS macros are used to allow C++ code
 * to use C headers.
 */
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

/*
 * Deprecated and used attribute macros
 */
#if __has_attribute(deprecated) || __GNUC_PREREQ__(3, 1)
#define __deprecated __attribute__((__deprecated__))
#else
#define __deprecated
#endif

#if __has_attribute(used) || __GNUC_PREREQ__(3, 1)
#define __used __attribute__((__used__))
#else
#define __used
#endif

/*
 * Weak symbol attribute
 */
#if __has_attribute(weak) || defined(__GNUC__)
#define __weak_symbol __attribute__((__weak__))
#else
#define __weak_symbol
#endif

/*
 * Section placement
 */
#if __has_attribute(section) || defined(__GNUC__)
#define __section(x) __attribute__((__section__(x)))
#else
#define __section(x)
#endif

#endif /* !_SYS_CDEFS_H_ */
