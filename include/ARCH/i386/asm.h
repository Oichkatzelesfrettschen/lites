
#pragma once

/* Generic assembly macros modelled after the 4.4BSD i386 header. */

#ifdef __ELF__
#define _ENTRY(name)    .globl name; .type name,@function; name
#define _LABEL(name)    name
#else
#define _ENTRY(name)    .globl _##name; .type _##name,@function; _##name
#define _LABEL(name)    _##name
#endif

#define ENTRY(name)     _ENTRY(name):
#define Entry(name)     ENTRY(name)
#define END(name)       .size _LABEL(name), . - _LABEL(name)

/* System call instruction */
#define SVC             int $0x80

/*
 * Additional low-level macros used by the Mach kernel are provided
 * in the original asm.h header.  Include it here so that both user
 * and kernel code share a single entry point.
 */
#include "mach/machine/asm.h"

