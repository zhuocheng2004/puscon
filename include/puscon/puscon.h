#ifndef PUSCON_PUSCON_H
#define PUSCON_PUSCON_H

#include <sys/user.h>

#include <puscon/types.h>


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

/*
 * Type Definitions
 */

/*
 * config to run puscon main function
 */
typedef struct puscon_config {
	/* filename of the helper kernel */
	char*		kernel_filename;
} puscon_config;

/*
 * informational about each thread
 */
typedef struct task_info {
	/* virtual id */
	u32		pid;

	/* 
	 * flags
	 */

	/* wether we are in kernel mode */
	u32		kernel : 1;

	/* whether syscalls should bypass */
	u32		bypass : 1;
} task_info;

/*
 * an instance of puscon simulation environment
 */
typedef struct puscon_context {
	puscon_config*	config;

	task_info*	entry_task;

	int 		should_stop;
}  puscon_context;

/*
 * Function Declarations
 */

int puscon_main(puscon_config* config);

int puscon_syscall_handle(puscon_context* context, pid_t child_pid);

#endif
