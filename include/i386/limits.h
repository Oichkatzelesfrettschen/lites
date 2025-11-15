/*
 * Machine-dependent limits for x86_64 and i386
 */

#ifndef _MACHINE_LIMITS_H_
#define _MACHINE_LIMITS_H_

#define	CHAR_BIT	8		/* number of bits in a char */

#define	SCHAR_MIN	(-128)		/* min value for a signed char */
#define	SCHAR_MAX	127		/* max value for a signed char */
#define	UCHAR_MAX	255		/* max value for an unsigned char */

#ifdef __CHAR_UNSIGNED__
#define	CHAR_MIN	0		/* min value for a char */
#define	CHAR_MAX	UCHAR_MAX	/* max value for a char */
#else
#define	CHAR_MIN	SCHAR_MIN
#define	CHAR_MAX	SCHAR_MAX
#endif

#define	SHRT_MIN	(-32768)	/* min value for a short */
#define	SHRT_MAX	32767		/* max value for a short */
#define	USHRT_MAX	65535		/* max value for an unsigned short */

#define	INT_MIN		(-2147483647-1)	/* min value for an int */
#define	INT_MAX		2147483647	/* max value for an int */
#define	UINT_MAX	4294967295U	/* max value for an unsigned int */

#if defined(__x86_64__) || defined(__amd64__)
#define	LONG_MIN	(-9223372036854775807L-1) /* min value for a long */
#define	LONG_MAX	9223372036854775807L	/* max value for a long */
#define	ULONG_MAX	18446744073709551615UL	/* max value for an unsigned long */
#else  /* i386 */
#define	LONG_MIN	(-2147483647L-1)	/* min value for a long */
#define	LONG_MAX	2147483647L		/* max value for a long */
#define	ULONG_MAX	4294967295UL		/* max value for an unsigned long */
#endif

#define	LLONG_MIN	(-9223372036854775807LL-1) /* min value for a long long */
#define	LLONG_MAX	9223372036854775807LL	/* max value for a long long */
#define	ULLONG_MAX	18446744073709551615ULL	/* max value for an unsigned long long */

#if defined(__x86_64__) || defined(__amd64__)
#define	LONG_BIT	64
#else
#define	LONG_BIT	32
#endif

#define	MB_LEN_MAX	1		/* no multibyte characters */

#endif /* !_MACHINE_LIMITS_H_ */
