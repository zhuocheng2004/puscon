#ifndef PUSCON_X86_64_SYSCALL_H
#define PUSCON_X86_64_SYSCALL_H

#include <puscon/types.h>


#define SYS_write	0x01
#define SYS_exit	0x3c


static inline u64 syscall0(u64 nr) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr)
	: "rax");
	return ret;
}

static inline u64 syscall1(u64 nr, u64 arg0) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %2, %%rdi  \n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0)
	: "rax", "rdi");
	return ret;
}

static inline u64 syscall2(u64 nr, u64 arg0, u64 arg1) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %3, %%rsi  \n\t"
		"mov     %2, %%rdi  \n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1)
	: "rax", "rdi", "rsi");
	return ret;
}

static inline u64 syscall3(u64 nr, u64 arg0, u64 arg1, u64 arg2) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %4, %%rdx  \n\t"
		"mov     %3, %%rsi  \n\t"
		"mov     %2, %%rdi  \n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2)
	: "rax", "rdi", "rsi", "rdx");
	return ret;
}

static inline u64 syscall4(u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %5, %%r10  \n\t"
		"mov     %4, %%rdx  \n\t"
		"mov     %3, %%rsi  \n\t"
		"mov     %2, %%rdi  \n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3)
	: "rax", "rdi", "rsi", "rdx", "r10");
	return ret;
}

static inline u64 syscall5(u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %6, %%r8  \n\t"
		"mov     %5, %%r10  \n\t"
		"mov     %4, %%rdx  \n\t"
		"mov     %3, %%rsi  \n\t"
		"mov     %2, %%rdi  \n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4)
	: "rax", "rdi", "rsi", "rdx", "r10", "r8");
	return ret;
}

static inline u64 syscall6(u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     %7, %%r9  \n\t"
		"mov     %6, %%r8  \n\t"
		"mov     %5, %%r10  \n\t"
		"mov     %4, %%rdx  \n\t"
		"mov     %3, %%rsi  \n\t"
		"mov     %2, %%rdi  \n\t"
		"mov     %1, %%rax  \n\t"
		"syscall  \n\t"
		"mov     %%rax, %0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4), "r" (arg5)
	: "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9");
	return ret;
}

#endif
