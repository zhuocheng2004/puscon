
	.section	.text
	.globl		_start
_start:
	call	kernel_main

	jmp	*%rax


	.globl		syscall_entry
syscall_entry:
	syscall
