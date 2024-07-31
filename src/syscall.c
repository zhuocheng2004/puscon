
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <wait.h>

#include <puscon/puscon.h>
#include <puscon/util.h>

void skip_syscall(puscon_context* context) {
	puscon_task_info *task = context->task_context.current_task;
	pid_t child_pid = task->host_pid;

	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
	regs.orig_rax = SYS_puscon_nop;		// -ENOSYS is what we want: skipping it
	ptrace(PTRACE_SETREGS, child_pid, NULL, &regs);
	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	waitpid(child_pid, NULL, __WALL);
}

/**
 * This function will make a syscall be executed directly in child context.
 * 
 * We first make the child jump to the specified entry with arguments in registers (with regs saved), 
 * execute the syscall, get the result,and then return the child to the original position.
 */
int puscon_child_syscall6(puscon_context* context, u64* ret,
	u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

	puscon_task_info *task = context->task_context.current_task;
	pid_t host_pid = task->host_pid;

	/* save regs */
	struct user_regs_struct regs_saved, regs;
	ptrace(PTRACE_GETREGS, host_pid, NULL, &regs);
	regs_saved = regs;

	/* jump to helper entry */
	regs = regs_saved;
	regs.rip = task->syscall_entry;
	ptrace(PTRACE_SETREGS, host_pid, NULL, &regs);

	/* syscall enter */
	int child_status;
	ptrace(PTRACE_SYSCALL, host_pid, NULL, NULL);
	waitpid(host_pid, &child_status, __WALL);
	if (!(WIFSTOPPED(child_status) && WSTOPSIG(child_status) == SIGTRAP)) {
		puscon_printk(KERN_EMERG "[PID %d] Error: syscall entry call failed, sig=%d.\n", 
			task->pid, WSTOPSIG(child_status));
		return 1;
	}
	ptrace(PTRACE_GETREGS, host_pid, NULL, &regs);
	regs.orig_rax = nr;
	regs.rdi = arg0;
	regs.rsi = arg1;
	regs.rdx = arg2;
	regs.r10 = arg3;
	regs.r8  = arg4;
	regs.r9  = arg5;
	ptrace(PTRACE_SETREGS, host_pid, NULL, &regs);

	/* syscall exit */
	ptrace(PTRACE_SYSCALL, host_pid, NULL, NULL);
	waitpid(host_pid, &child_status, __WALL);
	if (!(WIFSTOPPED(child_status) && WSTOPSIG(child_status) == SIGTRAP)) {
		puscon_printk(KERN_EMERG "[PID %d] Error: syscall entry call failed, sig=%d.\n", 
			task->pid, WSTOPSIG(child_status));
		return 1;
	}
	ptrace(PTRACE_GETREGS, host_pid, NULL, &regs);
	*ret = regs.rax;

	/* restore regs */
	ptrace(PTRACE_SETREGS, host_pid, NULL, &regs_saved);

	return 0;
}

int puscon_syscall_handle(puscon_context* context) {
	int err = 0;

	puscon_task_info *task = context->task_context.current_task;

	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, task->host_pid, 0, &regs);
	u64 syscall = regs.orig_rax;

	puscon_printk(KERN_DEBUG "[PID %d] Intercepted: ORIG_RAX = %lld, RIP = 0x%llx \n", task->pid, regs.orig_rax, regs.rip);

	skip_syscall(context);

	u64 a;
	if (puscon_child_syscall6(context, &a, SYS_getpid, 0, 0, 0, 0, 0, 0)) {
		err = 1;
		goto out;
	}
	puscon_printk(KERN_DEBUG "a = %lld \n", a);

	if (syscall == SYS_exit) {
		kill(task->host_pid, SIGKILL);
		context->task_context.pid_map[task->host_pid] = 0;
		puscon_idmap_free(&context->task_context.tasks, task->pid);
		puscon_printk(KERN_INFO "Child [pid=%d, host_pid=%d] exited with status %d.\n", task->pid, task->host_pid, regs.rdi);

		/* pid=0 is always occupied */
		if (puscon_idmap_occupied(&context->task_context.tasks) == 1) {
			// no running children
			err = 0;
			goto out;
		}
	}

	return 0;

out:
	context->should_stop = 1;
	return err;
}
