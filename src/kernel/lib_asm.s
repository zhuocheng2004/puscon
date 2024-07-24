
	.section	.text

	.globl		kernel_enter
kernel_enter:
	push	%rax
	mov	$0x1001, %rax
	syscall
	pop	%rax
	ret

	.globl		kernel_exit
kernel_exit:
	push	%rax
	mov	$0x1002, %rax
	syscall
	pop	%rax
	ret

	.globl		bypass_enable
bypass_enable:
	push	%rax
	mov	$0x1003, %rax
	syscall
	pop	%rax
	ret

	.globl		bypass_disable
bypass_disable:
	push	%rax
	mov	$0x1004, %rax
	syscall
	pop	%rax
	ret
