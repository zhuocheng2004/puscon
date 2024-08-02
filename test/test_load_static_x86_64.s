
	.section	.text

	.globl	_start
_start:
	mov	$0, %rdi
	mov	$60, %rax
	syscall
