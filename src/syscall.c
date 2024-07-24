
#include <signal.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <puscon/puscon.h>
#include <puscon/util.h>

int puscon_syscall_handle(puscon_context* context, pid_t child_pid) {
	int err = 0;

	u32 pid = context->task_context.pid_map[child_pid];
	if (pid == 0) {
		puscon_printk(KERN_EMERG "Error: cannot find child with host pid %d.\n", child_pid);
		err = 1;
		goto out;
	}

	puscon_task_info *task = context->task_context.tasks.ptrs[pid];
	if (!task) {
		puscon_printk(KERN_EMERG "Error: cannot find child with pid %d.\n", pid);
		err = 1;
		goto out;
	}

	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	u64 syscall = regs.orig_rax;

	puscon_printk(KERN_DEBUG "Intercepted: ORIG_RAX = %lld, RIP = 0x%llx \n", regs.orig_rax, regs.rip);

	//regs.rip = 0;
	//ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

	if (syscall == SYS_exit) {
		puscon_printk(KERN_INFO "Stopping child pid=%d, host_pid=%d.\n", pid, child_pid);
		kill(child_pid, SIGKILL);
		context->task_context.pid_map[child_pid] = 0;
		puscon_idmap_free(&context->task_context.tasks, pid);

		/* pid=0 is always occupied */
		if (puscon_idmap_occupied(&context->task_context.tasks) == 1) {
			err = 0;
			goto out;
		}
	}

	return 0;

out:
	context->should_stop = 1;
	return err;
}
