
	.section	.text
	.globl		_start
_start:
	mov	$0x1000, %rax
	syscall

	call	kernel_main

	mov	%rax, %rdi
	mov	$60, %rax
	syscall

