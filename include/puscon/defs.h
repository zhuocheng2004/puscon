#ifndef PUSCON_DEFS_H
#define PUSCON_DEFS_H

#include <arch_defs.h>

#define __user

#define HAPPY_CLANG		;


/* custom syscalls */
#define		SYS_puscon_base			0x1000
#define		SYS_puscon_nop			SYS_puscon_base + 0
#define		SYS_puscon_kernel_enter		SYS_puscon_base + 1
#define		SYS_puscon_kernel_exit		SYS_puscon_base + 2
#define		SYS_puscon_bypass_enable	SYS_puscon_base + 3
#define		SYS_puscon_bypass_disable	SYS_puscon_base + 4
#define		SYS_puscon_set_syscall_entry	SYS_puscon_base + 5


#endif
