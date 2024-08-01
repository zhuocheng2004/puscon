#ifndef PUSCON_X86_64_SYSCALL_H
#define PUSCON_X86_64_SYSCALL_H

#include <puscon/types.h>


#define SYS_write	0x40
#define SYS_exit	0x5d


static inline u64 syscall0(u64 nr) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr)
	: "x8", "x0");
	return ret;
}

static inline u64 syscall1(u64 nr, u64 arg0) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x0, %2  \n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0)
	: "x8", "x0");
	return ret;
}

static inline u64 syscall2(u64 nr, u64 arg0, u64 arg1) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x1, %3  \n\t"
		"mov     x0, %2  \n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1)
	: "x8", "x0", "x1");
	return ret;
}

static inline u64 syscall3(u64 nr, u64 arg0, u64 arg1, u64 arg2) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x2, %4  \n\t"
		"mov     x1, %3  \n\t"
		"mov     x0, %2  \n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2)
	: "x8", "x0", "x1", "x2");
	return ret;
}

static inline u64 syscall4(u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x3, %5  \n\t"
		"mov     x2, %4  \n\t"
		"mov     x1, %3  \n\t"
		"mov     x0, %2  \n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3)
	: "x8", "x0", "x1", "x2", "x3");
	return ret;
}

static inline u64 syscall5(u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x4, %6  \n\t"
		"mov     x3, %5  \n\t"
		"mov     x2, %4  \n\t"
		"mov     x1, %3  \n\t"
		"mov     x0, %2  \n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4)
	: "x8", "x0", "x1", "x2", "x3", "x4");
	return ret;
}

static inline u64 syscall6(u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
	u64 ret;
	asm volatile ("\n\t"
		"mov     x5, %7  \n\t"
		"mov     x4, %6  \n\t"
		"mov     x3, %5  \n\t"
		"mov     x2, %4  \n\t"
		"mov     x1, %3  \n\t"
		"mov     x0, %2  \n\t"
		"mov     x8, %1  \n\t"
		"svc     #0      \n\t"
		"mov     %0, x0  \n\t"
	: "=r" (ret)
	: "r" (nr), "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4), "r" (arg5)
	: "x8", "x0", "x1", "x2", "x3", "x4", "x5");
	return ret;
}

#endif
