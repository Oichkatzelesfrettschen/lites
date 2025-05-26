#ifndef SERVER_NETISO_SIMD_H
#define SERVER_NETISO_SIMD_H

#if defined(__has_include)
#  if __has_include(<immintrin.h>)
#    include <immintrin.h>
#    define LITES_HAVE_SSE 1
#  elif __has_include(<arm_neon.h>)
#    include <arm_neon.h>
#    define LITES_HAVE_NEON 1
#  else
#    define LITES_HAVE_SSE 0
#    define LITES_HAVE_NEON 0
#  endif
#else
#  define LITES_HAVE_SSE 0
#  define LITES_HAVE_NEON 0
#endif

#endif /* SERVER_NETISO_SIMD_H */
