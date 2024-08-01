#ifndef PUSCON_KERNEL_SYSCALL_H
#define PUSCON_KERNEL_SYSCALL_H

#include <puscon/puscon.h>
#include <puscon/types.h>

#include "arch_syscall.h"


static inline void kernel_enter() {
	syscall0(SYS_puscon_kernel_enter);
}

static inline void kernel_exit() {
	syscall0(SYS_puscon_kernel_exit);
}

static inline void bypass_enable() {
	syscall0(SYS_puscon_bypass_enable);
}

static inline void bypass_disable() {
	syscall0(SYS_puscon_bypass_disable);
}

static inline void set_syscall_entry(u64 entry) {
	syscall1(SYS_puscon_set_syscall_entry, entry);
}


static inline ssize_t write(int fd, const void *buf, size_t count) {
	return syscall3(SYS_write, fd, (u64) buf, count);
}

static inline void _exit(int status) {
	syscall1(SYS_exit, status);
}

#endif
