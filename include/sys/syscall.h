/*
 * System call numbers for BSD 4.4-Lite
 *
 * This file would normally be auto-generated from syscalls.master
 * This is a minimal version with essential system call numbers.
 */

#ifndef _SYS_SYSCALL_H_
#define _SYS_SYSCALL_H_

/* System call numbers */
#define	SYS_syscall	0
#define	SYS_exit	1
#define	SYS_fork	2
#define	SYS_read	3
#define	SYS_write	4
#define	SYS_open	5
#define	SYS_close	6
#define	SYS_wait4	7
					/* 8 is old creat */
#define	SYS_link	9
#define	SYS_unlink	10
					/* 11 is obsolete execv */
#define	SYS_chdir	12
#define	SYS_fchdir	13
#define	SYS_mknod	14
#define	SYS_chmod	15
#define	SYS_chown	16
#define	SYS_break	17
#define	SYS_getfsstat	18
					/* 19 is old lseek */
#define	SYS_getpid	20
#define	SYS_mount	21
#define	SYS_unmount	22
#define	SYS_setuid	23
#define	SYS_getuid	24
#define	SYS_geteuid	25
#define	SYS_ptrace	26
#define	SYS_recvmsg	27
#define	SYS_sendmsg	28
#define	SYS_recvfrom	29
#define	SYS_accept	30
#define	SYS_getpeername	31
#define	SYS_getsockname	32
#define	SYS_access	33
#define	SYS_chflags	34
#define	SYS_fchflags	35
#define	SYS_sync	36
#define	SYS_kill	37
					/* 38 is old stat */
#define	SYS_getppid	39
					/* 40 is old lstat */
#define	SYS_dup		41
#define	SYS_pipe	42
#define	SYS_getegid	43
#define	SYS_profil	44
#define	SYS_ktrace	45
#define	SYS_sigaction	46
#define	SYS_getgid	47
#define	SYS_sigprocmask	48
#define	SYS_getlogin	49
#define	SYS_setlogin	50
#define	SYS_acct	51
#define	SYS_sigpending	52
#define	SYS_sigaltstack	53
#define	SYS_ioctl	54
#define	SYS_reboot	55
#define	SYS_revoke	56
#define	SYS_symlink	57
#define	SYS_readlink	58
#define	SYS_execve	59
#define	SYS_umask	60
#define	SYS_chroot	61
					/* 62 is old fstat */
					/* 63 is old getkerninfo */
					/* 64 is old getpagesize */
#define	SYS_msync	65
#define	SYS_vfork	66
					/* 67 is obsolete vread */
					/* 68 is obsolete vwrite */
#define	SYS_sbrk	69
#define	SYS_sstk	70
					/* 71 is old mmap */
#define	SYS_vadvise	72
#define	SYS_munmap	73
#define	SYS_mprotect	74
#define	SYS_madvise	75
					/* 76 is obsolete vhangup */
					/* 77 is obsolete vlimit */
#define	SYS_mincore	78
#define	SYS_getgroups	79
#define	SYS_setgroups	80
#define	SYS_getpgrp	81
#define	SYS_setpgid	82
#define	SYS_setitimer	83
					/* 84 is old wait */
#define	SYS_swapon	85
#define	SYS_getitimer	86
					/* 87 is old gethostname */
					/* 88 is old sethostname */
#define	SYS_getdtablesize 89
#define	SYS_dup2	90
#define	SYS_fcntl	92
#define	SYS_select	93
#define	SYS_fsync	95
#define	SYS_setpriority	96
#define	SYS_socket	97
#define	SYS_connect	98
					/* 99 is old accept */
#define	SYS_getpriority	100

#define	SYS_MAXSYSCALL	200

#endif /* !_SYS_SYSCALL_H_ */
