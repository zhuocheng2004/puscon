
	.section	.text
	.globl		_start
_start:
	bl	kernel_main

	br	x0


	.globl		syscall_entry
syscall_entry:
	svc	#0
