
#include "kernel.h"

void* kernel_main() {
	bypass_enable();

	extern void* syscall_entry;
	set_syscall_entry((u64) (&syscall_entry));

	write(1, "ABC\n", 4);

	bypass_disable();
	kernel_exit();

	write(1, "DEF\n", 4);
	write(1, "GHI\n", 4);
	
	_exit(0);
	
	return 0;
}
