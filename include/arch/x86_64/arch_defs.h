#ifndef PUSCON_X86_64_DEFS_H
#define PUSCON_X86_64_DEFS_H

#include <sys/ptrace.h>
#include <sys/wait.h>

#include <puscon/puscon.h>

#include <arch_types.h>

static inline int syscall_nr_get(pid_t pid, arch_regs* regs, unsigned long* nr) {
	*nr = regs->orig_rax;
	return 0;
}

static inline int syscall_nr_set(pid_t pid, arch_regs* regs, unsigned long nr) {
	regs->orig_rax = nr;
	return 0;
}

#define regs_syscall(regs)	((regs)->orig_rax)
#define regs_ret(regs)		((regs)->rax)
#define regs_arg0(regs)		((regs)->rdi)
#define regs_arg1(regs)		((regs)->rsi)
#define regs_arg2(regs)		((regs)->rdx)
#define regs_arg3(regs)		((regs)->r10)
#define regs_arg4(regs)		((regs)->r8)
#define regs_arg5(regs)		((regs)->r9)

#define regs_pc(regs)		((regs)->rip)


static inline int regs_get(pid_t pid, arch_regs* regs) {
	return (int) ptrace(PTRACE_GETREGS, pid, NULL, regs);
}

static inline int regs_set(pid_t pid, arch_regs* regs) {
	return (int) ptrace(PTRACE_SETREGS, pid, NULL, regs);
}

#endif
