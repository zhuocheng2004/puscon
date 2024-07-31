#ifndef PUSCON_TASK_H
#define PUSCON_TASK_H

#include <sys/ptrace.h>
#include <sys/user.h>

#include <puscon/path.h>
#include <puscon/types.h>
#include <puscon/util.h>

/*
 * informational about each thread
 */
typedef struct puscon_task_info {
	/* virtual pid */
	u32		pid;

	/* host pid */
	u32		host_pid;


	u64		syscall_entry;

	/* filesystem info */
	struct {
		puscon_path		pwd;
		puscon_path		root;
	}		fs;


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

	puscon_task_info*	current_task;

	puscon_idmap	tasks;

	/* map: real host pid -> virtual pid */
	u32*		pid_map;
} puscon_task_context;


#endif
