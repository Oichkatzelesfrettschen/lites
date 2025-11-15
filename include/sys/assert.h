/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)assert.h	8.2 (Berkeley) 1/21/94
 */

#ifndef _SYS_ASSERT_H_
#define _SYS_ASSERT_H_

#ifdef KERNEL

/*
 * KASSERT(assertion, (printf-style arguments))
 *
 * Kernel assertion macro. If DIAGNOSTIC is defined, assertion failures
 * will cause a panic with the specified message.
 */

#ifdef DIAGNOSTIC
#define KASSERT(exp, msg) do { \
    if (!(exp)) { \
        printf("assertion \"%s\" failed: file \"%s\", line %d\n", \
               #exp, __FILE__, __LINE__); \
        printf msg; \
        panic("KASSERT"); \
    } \
} while (0)
#else
#define KASSERT(exp, msg)   /* nothing */
#endif

/*
 * KASSERTMSG(assertion, message, ...)
 *
 * Similar to KASSERT but with printf-style formatting support
 */
#ifdef DIAGNOSTIC
#define KASSERTMSG(exp, ...) do { \
    if (!(exp)) { \
        printf("assertion \"%s\" failed: file \"%s\", line %d\n", \
               #exp, __FILE__, __LINE__); \
        printf(__VA_ARGS__); \
        panic("KASSERTMSG"); \
    } \
} while (0)
#else
#define KASSERTMSG(exp, ...)   /* nothing */
#endif

/*
 * KDASSERT(assertion)
 *
 * Always-enabled kernel debug assertion
 * Unlike KASSERT, this is always compiled in
 */
#define KDASSERT(exp) do { \
    if (!(exp)) { \
        printf("debug assertion \"%s\" failed: file \"%s\", line %d\n", \
               #exp, __FILE__, __LINE__); \
        panic("KDASSERT"); \
    } \
} while (0)

/*
 * Simple assertion without message - panics with just the expression
 */
#ifdef DIAGNOSTIC
#define KERNEL_ASSERT(exp) do { \
    if (!(exp)) \
        panic("kernel assertion \"%s\" failed: file \"%s\", line %d", \
              #exp, __FILE__, __LINE__); \
} while (0)
#else
#define KERNEL_ASSERT(exp)  /* nothing */
#endif

/*
 * CTR (Compile-Time assertion with Return)
 * Compile-time assertion checking
 */
#define CTASSERT(x)     typedef int __assert ## __LINE__[(x) ? 1 : -1]

/*
 * Legacy macro for older code
 */
#ifdef DIAGNOSTIC
#ifndef ASSERT
#define ASSERT(exp) KASSERT(exp, ("ASSERT failed"))
#endif
#else
#ifndef ASSERT
#define ASSERT(exp)     /* nothing */
#endif
#endif

/*
 * Function prototypes for assertion support
 */
void panic(const char *fmt, ...);

#endif /* KERNEL */

#endif /* !_SYS_ASSERT_H_ */
