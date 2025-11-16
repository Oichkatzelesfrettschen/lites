/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * File:	mach/machine.h
 * Purpose:
 *	Machine type definitions and cpu type/subtype codes
 */

#ifndef	_MACH_MACHINE_H_
#define _MACH_MACHINE_H_

/*
 * Machine types (cpu_type_t and cpu_subtype_t)
 */

typedef integer_t	cpu_type_t;
typedef integer_t	cpu_subtype_t;

/*
 * Machine CPU types
 */
#define CPU_TYPE_ANY		((cpu_type_t) -1)
#define	CPU_TYPE_VAX		((cpu_type_t) 1)
#define CPU_TYPE_ROMP		((cpu_type_t) 2)
#define CPU_TYPE_NS32032	((cpu_type_t) 4)
#define CPU_TYPE_NS32332	((cpu_type_t) 5)
#define CPU_TYPE_MC680x0	((cpu_type_t) 6)
#define CPU_TYPE_I386		((cpu_type_t) 7)
#define CPU_TYPE_X86_64		((cpu_type_t) (CPU_TYPE_I386 | CPU_ARCH_ABI64))
#define CPU_TYPE_MIPS		((cpu_type_t) 8)
#define CPU_TYPE_NS32532	((cpu_type_t) 9)
#define CPU_TYPE_HPPA		((cpu_type_t) 11)
#define CPU_TYPE_ARM		((cpu_type_t) 12)
#define CPU_TYPE_MC88000	((cpu_type_t) 13)
#define CPU_TYPE_SPARC		((cpu_type_t) 14)
#define CPU_TYPE_I860		((cpu_type_t) 15)
#define CPU_TYPE_I860_LITTLE	((cpu_type_t) 16)
#define CPU_TYPE_RS6000		((cpu_type_t) 17)
#define CPU_TYPE_POWERPC	((cpu_type_t) 18)
#define CPU_TYPE_POWERPC64	((cpu_type_t) (CPU_TYPE_POWERPC | CPU_ARCH_ABI64))

#define CPU_ARCH_MASK		0xff000000		/* mask for architecture bits */
#define CPU_ARCH_ABI64		0x01000000		/* 64 bit ABI */

/*
 * Machine CPU subtypes
 */
#define CPU_SUBTYPE_MULTIPLE		((cpu_subtype_t) -1)
#define CPU_SUBTYPE_LITTLE_ENDIAN	((cpu_subtype_t) 0)
#define CPU_SUBTYPE_BIG_ENDIAN		((cpu_subtype_t) 1)

/*
 * VAX subtypes
 */
#define	CPU_SUBTYPE_VAX_ALL	((cpu_subtype_t) 0)
#define CPU_SUBTYPE_VAX780	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_VAX785	((cpu_subtype_t) 2)
#define CPU_SUBTYPE_VAX750	((cpu_subtype_t) 3)
#define CPU_SUBTYPE_VAX730	((cpu_subtype_t) 4)
#define CPU_SUBTYPE_UVAXI	((cpu_subtype_t) 5)
#define CPU_SUBTYPE_UVAXII	((cpu_subtype_t) 6)
#define CPU_SUBTYPE_VAX8200	((cpu_subtype_t) 7)
#define CPU_SUBTYPE_VAX8500	((cpu_subtype_t) 8)
#define CPU_SUBTYPE_VAX8600	((cpu_subtype_t) 9)
#define CPU_SUBTYPE_VAX8650	((cpu_subtype_t) 10)
#define CPU_SUBTYPE_VAX8800	((cpu_subtype_t) 11)
#define CPU_SUBTYPE_UVAXIII	((cpu_subtype_t) 12)

/*
 * 680x0 subtypes
 */
#define	CPU_SUBTYPE_MC680x0_ALL	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_MC68030		((cpu_subtype_t) 1)
#define CPU_SUBTYPE_MC68040		((cpu_subtype_t) 2)
#define CPU_SUBTYPE_MC68030_ONLY	((cpu_subtype_t) 3)

/*
 * I386 subtypes
 */
#define	CPU_SUBTYPE_I386_ALL		((cpu_subtype_t) 3)
#define CPU_SUBTYPE_386				((cpu_subtype_t) 3)
#define CPU_SUBTYPE_486				((cpu_subtype_t) 4)
#define CPU_SUBTYPE_486SX			((cpu_subtype_t) 132)
#define CPU_SUBTYPE_586				((cpu_subtype_t) 5)
#define CPU_SUBTYPE_PENT			CPU_SUBTYPE_586
#define CPU_SUBTYPE_PENTPRO			((cpu_subtype_t) 22)
#define CPU_SUBTYPE_PENTII_M3		((cpu_subtype_t) 54)
#define CPU_SUBTYPE_PENTII_M5		((cpu_subtype_t) 86)

#define CPU_SUBTYPE_INTEL(f, m)		((cpu_subtype_t) (f) + ((m) << 4))

/*
 * X86_64 subtypes
 */
#define CPU_SUBTYPE_X86_64_ALL		((cpu_subtype_t) 3)
#define CPU_SUBTYPE_X86_64_H		((cpu_subtype_t) 8)

/*
 * MIPS subtypes
 */
#define	CPU_SUBTYPE_MIPS_ALL		((cpu_subtype_t) 0)
#define CPU_SUBTYPE_MIPS_R2300		((cpu_subtype_t) 1)
#define CPU_SUBTYPE_MIPS_R2600		((cpu_subtype_t) 2)
#define CPU_SUBTYPE_MIPS_R2800		((cpu_subtype_t) 3)
#define CPU_SUBTYPE_MIPS_R2000a		((cpu_subtype_t) 4)

/*
 * Machine slot structure
 */

struct machine_info {
	integer_t	major_version;	/* kernel major version id */
	integer_t	minor_version;	/* kernel minor version id */
	integer_t	max_cpus;	/* max number of cpus compiled */
	integer_t	avail_cpus;	/* number of cpus now available */
	vm_size_t	memory_size;	/* size of memory in bytes */
};

typedef struct machine_info	*machine_info_t;
typedef struct machine_info	machine_info_data_t;	/* bogus */

typedef integer_t	*machine_info_array_t;

#define MACHINE_INFO_COUNT	((mach_msg_type_number_t) \
	(sizeof(machine_info_data_t)/sizeof(integer_t)))

/*
 * Machine slot information
 */

struct machine_slot {
	boolean_t	is_cpu;		/* is there a cpu in this slot? */
	cpu_type_t	cpu_type;	/* type of cpu */
	cpu_subtype_t	cpu_subtype;	/* subtype of cpu */
	volatile boolean_t	running;	/* is cpu running */
	integer_t	cpu_ticks[3];	/* time spent in user/system/idle */
	integer_t	clock_freq;	/* clock interrupt frequency */
};

typedef struct machine_slot	*machine_slot_t;
typedef struct machine_slot	machine_slot_data_t;

#endif	/* _MACH_MACHINE_H_ */
