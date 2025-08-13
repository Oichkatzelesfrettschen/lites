core/alloc.c:308:15: error: syntax error [syntaxError]
   ip->i_flag =| ILOCK;
              ^
core/clock.c:26:1: portability: Omitted return type of function 'clock' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
clock(dev, sp, r1, nps, r0, pc, ps)
^
core/clock.c:145:1: portability: Omitted return type of function 'timeout' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
timeout(fun, arg, tim)
^
core/fio.c:161:11: error: Syntax Error: AST broken, binary operator '=' doesn't have two operands. [internalAstError]
        m = >> 3;
          ^
core/iget.c:39:15: error: syntax error [syntaxError]
    p->i_flag =| IWANT;
              ^
core/ipc_mig.c:494:14: style: Parameter 'src' can be declared as pointer to const [constParameterPointer]
char *dest, *src;
             ^
core/main.c:100:17: error: syntax error [syntaxError]
 proc[0].p_flag =| SLOAD|SSYS;
                ^
core/malloc.c:28:1: portability: Omitted return type of function 'malloc' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
malloc(mp, size)
^
core/malloc.c:55:1: portability: Omitted return type of function 'mfree' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
mfree(mp, size, aa)
^
core/malloc.c:36:6: portability: Assigning a pointer to an integer is not portable. [AssignmentAddressToInteger]
   a = bp->m_addr;
     ^
core/malloc.c:37:15: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
   bp->m_addr =+ size;
              ^
core/malloc.c:38:20: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
   if ((bp->m_size =- size) == 0)
                   ^
core/malloc.c:65:18: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
  (bp-1)->m_size =+ size;
                 ^
core/malloc.c:76:15: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
   bp->m_addr =- size;
              ^
core/malloc.c:77:15: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
   bp->m_size =+ size;
              ^
core/malloc.c:79:6: portability: Assigning a pointer to an integer is not portable. [AssignmentAddressToInteger]
   t = bp->m_addr;
     ^
core/malloc.c:80:15: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
   bp->m_addr = a;
              ^
core/malloc.c:82:6: portability: Assigning a pointer to an integer is not portable. [AssignmentAddressToInteger]
   t = bp->m_size;
     ^
core/malloc.c:83:15: portability: Assigning an integer to a pointer is not portable. [AssignmentIntegerToAddress]
   bp->m_size = size;
              ^
core/nami.c:113:16: error: syntax error [syntaxError]
    dp->i_flag =| IUPD;
               ^
core/pipe.c:99:20: error: syntax error [syntaxError]
        ip->i_mode = | IREAD;
                   ^
core/prf.c:40:1: portability: Omitted return type of function 'printf' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
printf(fmt,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc)
^
core/prf.c:68:1: portability: Omitted return type of function 'printn' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
printn(n, b)
^
core/prf.c:84:1: portability: Omitted return type of function 'putchar' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
putchar(c)
^
core/prf.c:113:1: portability: Omitted return type of function 'panic' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
panic(s)
^
core/prf.c:129:1: portability: Omitted return type of function 'prdev' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
prdev(str, dev)
^
core/prf.c:142:1: portability: Omitted return type of function 'deverror' defaults to int, this is not supported by ISO C99 and later standards. [returnImplicitInt]
deverror(bp, o1, o2)
^
core/ptbl_static.c:38:15: style: The scope of the variable 'mapEnt' can be reduced. [variableScope]
    MapEntry *mapEnt;
              ^
core/ptbl_static.c:37:15: style: Variable 'e' is assigned a value that is never used. [unreadVariable]
    Entry  *e = entries;
              ^
core/rdwri.c:33:13: error: syntax error [syntaxError]
 ip->i_flag =| IACC;
            ^
core/sched.c:53:16: error: syntax error [syntaxError]
    TAILQ_HEAD(, thread_entry) queue;
               ^
core/sig.c:249:27: error: syntax error [syntaxError]
        u.u_procp->p_flag = | STRC;
                          ^
core/slp.c:250:13: error: syntax error [syntaxError]
 rp->p_flag =| SLOAD;
            ^
core/subr.c:54:15: error: syntax error [syntaxError]
   ip->i_mode =| ILARG;
              ^
core/sys1.c:308:31: error: syntax error [syntaxError]
                    p->p_flag = | SWTED;
                              ^
core/sys2.c:223:16: error: syntax error [syntaxError]
    ip->i_flag = | IUPD;
               ^
core/sys3.c:124:16: error: syntax error [syntaxError]
    ip->i_flag = | IMOUNT;
               ^
core/sys4.c:104:16: error: syntax error [syntaxError]
    ip->i_flag = | IUPD;
               ^
core/text.c:35:13: error: syntax error [syntaxError]
 rp->p_flag =| SLOCK;
            ^
core/trap.c:45:13: error: syntax error [syntaxError]
        dev = | USER;
            ^
nofile:0:0: information: Too many #ifdef configurations - cppcheck only checks 1 configurations. Use --force to check all configurations. For more details, use --enable=information. [toomanyconfigs]

