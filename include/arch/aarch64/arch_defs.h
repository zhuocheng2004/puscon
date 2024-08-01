#ifndef PUSCON_AARCH64_DEFS_H
#define PUSCON_AARCH64_DEFS_H

#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <puscon/puscon.h>

#include <arch_types.h>

#ifndef NT_PRSTATUS
#define NT_PRSTATUS	1
#endif

#ifndef NT_ARM_SYSTEM_CALL
#define NT_ARM_SYSTEM_CALL 0x404
#endif

static inline int syscall_nr_get(pid_t pid, arch_regs* regs, unsigned long* nr) {
	*nr = regs->regs[8];
	return 0;
}

static inline int syscall_nr_set(pid_t pid, arch_regs* regs, unsigned long nr) {
	unsigned long syscall_nr = nr;
	struct iovec buf = {
		.iov_base = &syscall_nr,
		.iov_len = sizeof (int),
	};
	return ptrace(PTRACE_SETREGSET, pid, NT_ARM_SYSTEM_CALL, &buf);
}

#define regs_syscall(_regs)		((_regs)->regs[8])
#define regs_ret(_regs)			((_regs)->regs[0])
#define regs_arg0(_regs)		((_regs)->regs[0])
#define regs_arg1(_regs)		((_regs)->regs[1])
#define regs_arg2(_regs)		((_regs)->regs[2])
#define regs_arg3(_regs)		((_regs)->regs[3])
#define regs_arg4(_regs)		((_regs)->regs[4])
#define regs_arg5(_regs)		((_regs)->regs[5])

#define regs_pc(regs)			((regs)->pc)


static inline int regs_get(pid_t pid, arch_regs* regs) {
	struct iovec buf = {
		.iov_base = regs,
		.iov_len = sizeof(arch_regs),
	};
	return ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &buf);
}

static inline int regs_set(pid_t pid, arch_regs* regs) {
	struct iovec buf = {
		.iov_base = regs,
		.iov_len = sizeof(arch_regs),
	};
	return ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &buf);
}

#endif
