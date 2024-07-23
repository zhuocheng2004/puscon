
	.section	.text
	.globl		_start
_start:
	call	kernel_start

	# disable bypassing
	call	bypass_disable

	# test
	mov	$1, %rdi	# stdout
	lea	msg(%rip), %rsi
	mov	$4, %rdx
	mov	$1, %rax	# SYS_write
	syscall

	call	bypass_enable

	mov	$1, %rdi	# stdout
	lea	msg(%rip), %rsi
	mov	$4, %rdx
	mov	$1, %rax	# SYS_write
	syscall

	call	bypass_disable

	# exit kernel mode
	call	kernel_exit

	mov	$0, %rdi
	mov	$60, %rax
	syscall

	.section	.data
msg:
	.asciz		"ABC\n"
