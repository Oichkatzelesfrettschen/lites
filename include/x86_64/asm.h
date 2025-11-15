/*
 * x86_64 Assembly Language Macros
 * For Lites 1.1.u3 build on x86_64 architecture
 *
 * Provides standard assembly macros (ENTRY, END, ALIGN, etc.)
 * Based on BSD/Mach conventions for x86_64
 */

#ifndef _X86_64_ASM_H_
#define _X86_64_ASM_H_

/*
 * ENTRY(name) - Define a global function entry point
 * Creates a .globl symbol and .align directive
 */
#define ENTRY(name) \
    .globl name; \
    .align 16,0x90; \
    name:

/*
 * END(name) - Mark end of function (optional, for documentation)
 */
#define END(name)

/*
 * ALIGN - Align to specified boundary (8-byte for x86_64)
 */
#define ALIGN 8

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
    .align 8; \
    name:

#define LENTRY(name) \
    .align 16,0x90; \
    name:

/*
 * Common x86_64 assembly directives
 */
#define TEXT .text
#define DATA_SECTION .data
#define BSS .bss

/*
 * System call instruction for x86_64
 * Uses syscall instruction instead of int $0x80
 */
#define SVC syscall

/*
 * x86_64 specific macros
 */
#define PUSH_REGS \
    pushq %rax; \
    pushq %rbx; \
    pushq %rcx; \
    pushq %rdx; \
    pushq %rsi; \
    pushq %rdi; \
    pushq %rbp; \
    pushq %r8; \
    pushq %r9; \
    pushq %r10; \
    pushq %r11; \
    pushq %r12; \
    pushq %r13; \
    pushq %r14; \
    pushq %r15

#define POP_REGS \
    popq %r15; \
    popq %r14; \
    popq %r13; \
    popq %r12; \
    popq %r11; \
    popq %r10; \
    popq %r9; \
    popq %r8; \
    popq %rbp; \
    popq %rdi; \
    popq %rsi; \
    popq %rdx; \
    popq %rcx; \
    popq %rbx; \
    popq %rax

#endif /* _X86_64_ASM_H_ */
