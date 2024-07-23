
#include <signal.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <puscon/puscon.h>

int puscon_syscall_handle(puscon_context* context, pid_t child_pid) {
	task_info *task = context->entry_task;
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	u64 syscall = regs.orig_rax;

	printf("Intercepted: ORIG_RAX = %lld, RIP = 0x%llx \n", regs.orig_rax, regs.rip);
	ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

	if (syscall == SYS_exit) {
		kill(child_pid, SIGKILL);
		context->should_stop = 1;
	}
	return 0;
}
