/*
 * machine/pcb.h - i386 Process Control Block
 */

#ifndef _MACHINE_PCB_H_
#define _MACHINE_PCB_H_

#ifndef LOCORE
#include <sys/types.h>
#endif

/*
 * i386 process control block structure
 */
struct pcb {
	int	pcb_cr3;	/* page directory pointer */
	int	pcb_edi;
	int	pcb_esi;
	int	pcb_ebp;
	int	pcb_esp;
	int	pcb_ebx;
	int	pcb_eip;
	int	pcb_efl;	/* processor flags */

	int	pcb_esp0;	/* kernel stack pointer */
	int	pcb_ss0;	/* kernel stack segment */

	/* FPU state */
	char	pcb_savefpu[108];	/* FPU state save area */
	int	pcb_flags;
#define	FP_SOFTFP	0x01	/* process using software fpu */
#define PCB_SEGS	0x02	/* process has segment regs */
};

#endif /* _MACHINE_PCB_H_ */
