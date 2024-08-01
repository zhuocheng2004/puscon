#ifndef PUSCON_AARCH64_TYPES_H
#define PUSCON_AARCH64_TYPES_H

typedef struct arch_regs {
	unsigned long	regs[31];
	unsigned long	sp;
	unsigned long	pc;
	unsigned long	pstate;
} arch_regs;

#endif
