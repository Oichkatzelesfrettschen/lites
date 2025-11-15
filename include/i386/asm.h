/*
 * i386 Assembly Language Macros
 * For Lites 1.1.u3 build on x86 architecture
 *
 * Provides standard assembly macros (ENTRY, END, ALIGN, etc.)
 * Based on BSD/Mach conventions for i386
 */

#ifndef _I386_ASM_H_
#define _I386_ASM_H_

/*
 * ENTRY(name) - Define a global function entry point
 * Creates a .globl symbol and .align directive
 */
#define ENTRY(name) \
    .globl name; \
    .align 4,0x90; \
    name:

/*
 * END(name) - Mark end of function (optional, for documentation)
 */
#define END(name)

/*
 * ALIGN - Align to specified boundary
 */
#define ALIGN 4

/*
 * Function call/return macros
 */
#define CALL(func) call func
#define RET ret

/*
 * Data alignment macros
 */
#define DATA(name) \
    .data; \
    .globl name; \
    .align 4; \
    name:

#define LENTRY(name) \
    .align 4,0x90; \
    name:

/*
 * Common x86 assembly directives
 */
#define TEXT .text
#define DATA_SECTION .data
#define BSS .bss

#endif /* _I386_ASM_H_ */
