#ifndef PUSCON_PUSCON_H
#define PUSCON_PUSCON_H

#include <stdarg.h>
#include <sys/user.h>

#include <puscon/types.h>
#include <puscon/util.h>


/*
 * Constant Definitions
 */

/* custom syscalls */
#define		SYS_puscon_base			0x1000
#define		SYS_puscon_nop			SYS_puscon_base + 0
#define		SYS_puscon_kernel_enter		SYS_puscon_base + 1
#define		SYS_puscon_kernel_exit		SYS_puscon_base + 2
#define		SYS_puscon_bypass_enable	SYS_puscon_base + 3
#define		SYS_puscon_bypass_disable	SYS_puscon_base + 4

#define		HOST_PID_MAX_SHIFT	22
#define		PID_MAX_SHIFT		16

#define		PRINTK_BUF_SIZE		4096

#define		KERN_SOH		"\001"
#define		KERN_SOH_ASCII		'\001'

#define		KERN_EMERG		KERN_SOH "0"
#define		KERN_ALERT		KERN_SOH "1"
#define		KERN_CRIT		KERN_SOH "2"
#define		KERN_ERR		KERN_SOH "3"
#define		KERN_WARNING		KERN_SOH "4"
#define		KERN_NOTICE		KERN_SOH "5"
#define		KERN_INFO		KERN_SOH "6"
#define		KERN_DEBUG		KERN_SOH "7"

#define SYSCALL(nr)	\
	asm("push rax; mov $" #nr " %rax; pop rax;")

/*
 * Type Definitions
 */

/*
 * config to run puscon main function
 */
typedef struct puscon_config {
	/* filename of the helper kernel */
	char*		kernel_filename;

	/* filename of the entry program */
	char*		entry_filename;

	int		entry_argc;

	char**		entry_argv;
} puscon_config;

/*
 * informational about each thread
 */
typedef struct puscon_task_info {
	/* virtual pid */
	u32		pid;

	/* host pid */
	u32		host_pid;

	/* 
	 * flags
	 */

	/* whether we are in kernel mode */
	u32		kernel : 1;

	/* whether syscalls should bypass */
	u32		bypass : 1;
} puscon_task_info;

/*
 * task management context
 */
typedef struct puscon_task_context {
	puscon_task_info*	entry_task;

	puscon_idmap	tasks;

	/* map: real host pid -> virtual pid */
	u32*		pid_map;
} puscon_task_context;

/*
 * an instance of puscon simulation environment
 */
typedef struct puscon_context {
	puscon_config*	config;

	puscon_task_context	task_context;

	int		should_stop;
}  puscon_context;

/*
 * Global Variables
 */
extern int puscon_printk_level;
extern int puscon_printk_use_ansi_color;

/*
 * Function Declarations
 */

int puscon_main(puscon_config* config);

int puscon_syscall_handle(puscon_context* context, pid_t child_pid);

int puscon_vprintk(const char *fmt, va_list args);
int puscon_printk(const char *fmt, ...);

#endif
