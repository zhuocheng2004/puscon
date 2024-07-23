
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

#include <puscon/puscon.h>

int puscon_syscall_handle(puscon_context* context, pid_t child_pid) {
	task_info *task = context->entry_task;
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	u64 syscall = regs.orig_rax;
	return 1;
}
