
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <wait.h>

#include <puscon/puscon.h>
#include <puscon/types.h>
#include <puscon/util.h>

int skip_syscall(puscon_task_info* task) {
	//puscon_log(LOG_DEBUG "skip_syscall enter.\n");
	pid_t child_pid = task->host_pid;

	arch_regs regs;
	long err = regs_get(child_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_get failed.\n");
		return 1;
	}

	// This should not cause trouble. (Note: some Android devices do not allow bad syscall.)
	// (not working) regs_syscall(&regs) = SYS_getpid;
	err = syscall_nr_set(child_pid, &regs, SYS_getpid);
	if (err) {
		puscon_log(LOG_EMERG "Error: syscall_nr_set failed.\n");
		return 1;
	}
	err = regs_set(child_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_set failed.\n");
		return 1;
	}

	err = ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	if (err) {
		puscon_log(LOG_EMERG "Error: PTRACE_SYSCALL failed.\n");
		return 1;
	}
	waitpid(child_pid, NULL, __WALL);
	//puscon_log(LOG_DEBUG "skip_syscall exit.\n");

	return 0;
}

/**
 * This function will make a syscall be executed directly in child context.
 * 
 * We first make the child jump to the specified entry with arguments in registers (with regs saved), 
 * execute the syscall, get the result, and then return the child to the original position.
 */
int puscon_child_syscall6(puscon_task_info* task, u64* ret,
	u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

	pid_t host_pid = task->host_pid;

	/* save regs */
	arch_regs regs_saved, regs;
	long err = regs_get(host_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_get failed.\n");
		return 1;
	}
	regs_saved = regs;

	/* jump to helper entry */
	regs = regs_saved;
	regs_pc(&regs) = task->syscall_entry;
	err = regs_set(host_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_set failed.\n");
		return 1;
	}

	/* syscall enter */
	int child_status;
	err = ptrace(PTRACE_SYSCALL, host_pid, NULL, NULL);
	if (err) {
		puscon_log(LOG_EMERG "Error: PTRACE_SYSCALL failed.\n");
		return 1;
	}
	waitpid(host_pid, &child_status, __WALL);
	if (!(WIFSTOPPED(child_status) && WSTOPSIG(child_status) == SIGTRAP)) {
		puscon_log(LOG_EMERG "[PID %d] Error: syscall entry call failed, sig=%d.\n", 
			task->pid, WSTOPSIG(child_status));
		return 1;
	}
	err = regs_get(host_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_get failed.\n");
		return 1;
	}
	// (not working) regs_syscall(&regs) = nr;
	err = syscall_nr_set(host_pid, &regs, nr);
	if (err) {
		puscon_log(LOG_EMERG "Error: syscall_nr_set failed.\n");
		return 1;
	}
	regs_arg0(&regs) = arg0;
	regs_arg1(&regs) = arg1;
	regs_arg2(&regs) = arg2;
	regs_arg3(&regs) = arg3;
	regs_arg4(&regs) = arg4;
	regs_arg5(&regs) = arg5;
	err = regs_set(host_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_set failed.\n");
		return 1;
	}

	/* syscall exit */
	err = ptrace(PTRACE_SYSCALL, host_pid, NULL, NULL);
	if (err) {
		puscon_log(LOG_EMERG "Error: PTRACE_SYSCALL failed.\n");
		return 1;
	}
	waitpid(host_pid, &child_status, __WALL);
	if (!(WIFSTOPPED(child_status) && WSTOPSIG(child_status) == SIGTRAP)) {
		puscon_log(LOG_EMERG "[PID %d] Error: syscall entry call failed, sig=%d.\n", 
			task->pid, WSTOPSIG(child_status));
		return 1;
	}
	err = regs_get(host_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_get failed.\n");
		return 1;
	}
	*ret = regs_ret(&regs);

	/* restore regs */
	err = regs_set(host_pid, &regs_saved);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_set failed.\n");
		return 1;
	}

	return 0;
}

int puscon_syscall_handle(puscon_task_info* task) {
	long err = 0;

	arch_regs regs;
	err = regs_get(task->host_pid, &regs);
	if (err) {
		puscon_log(LOG_EMERG "Error: regs_get failed.\n");
		return 1;
	}
	unsigned long syscall = regs_syscall(&regs);

	puscon_log(LOG_DEBUG "[PID %d] Intercepted: SYSCALL = %lld, PC = 0x%llx \n", task->pid, syscall, regs_pc(&regs));

	skip_syscall(task);

	u64 a;
	if (puscon_child_syscall6(task, &a, SYS_getppid, 0, 0, 0, 0, 0, 0)) {
		err = 1;
		goto out;
	}
	puscon_log(LOG_DEBUG "Yes: ppid = %lld \n", a);

	if (syscall == SYS_exit) {
		kill(task->host_pid, SIGKILL);
		task->context->task_context.pid_map[task->host_pid] = 0;
		puscon_idmap_free(&task->context->task_context.tasks, task->pid);
		puscon_log(LOG_INFO "Child [pid=%d, host_pid=%d] exited with status %d.\n", task->pid, task->host_pid, regs_arg0(&regs));

		/* pid=0 is always occupied */
		if (puscon_idmap_occupied(&task->context->task_context.tasks) == 1) {
			// no running children
			err = 0;
			goto out;
		}
	}

	return 0;

out:
	task->context->should_stop = 1;
	return err;
}
