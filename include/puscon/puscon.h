#ifndef PUSCON_PUSCON_H
#define PUSCON_PUSCON_H

#include <sys/user.h>

#include <puscon/types.h>


/*
 * Constant Definitions
 */

/* custom syscalls */
#define		SYS_puscon_base			0x1000
#define		SYS_puscon_exit_init		SYS_puscon_base + 0
#define		SYS_puscon_bypass_enable	SYS_puscon_base + 1
#define		SYS_puscon_bypass_disable	SYS_puscon_base + 2

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

	/* whether the thread is in init phase; during init, syscalls bypass */
	u32		initialized : 1;

	/* whether syscalls should bypass */
	u32		bypass : 1;

	/* whether the task is performing a real syscall */
	u32		in_syscall : 1;

	/* whether the task is skipping a syscall */
	u32		skipping : 1;
} task_info;

/*
 * an instance of puscon simulation environment
 */
typedef struct puscon_context {
	puscon_config*	config;

	task_info*	entry_task;
}  puscon_context;

/*
 * Function Declarations
 */

int puscon_main(puscon_config* config);

int puscon_syscall_handle(puscon_context* context, pid_t child_pid);

#endif
