
#define _GNU_SOURCE
#include <sys/uio.h>

#include <puscon/uaccess.h>

unsigned long puscon_copy_from_user(puscon_context* context, void* to, const void __user* from, unsigned long n) {
	pid_t pid = context->task_context.current_task->host_pid;
	//puscon_printk(KERN_WARNING "Copying from user pid=%d.\n", pid);

	struct iovec local, remote;
	local.iov_base = to;
	local.iov_len = n;
	remote.iov_base = (void*) from;
	remote.iov_len = n;

	return process_vm_readv(pid, &local, 1, &remote, 1, 0);
}

unsigned long puscon_copy_to_user(puscon_context* context, void __user* to, const void* from, unsigned long n) {
	pid_t pid = context->task_context.current_task->host_pid;
	//puscon_printk(KERN_WARNING "Copying to user pid=%d.\n", pid);

	struct iovec local, remote;
	local.iov_base = (void*) from;
	local.iov_len = n;
	remote.iov_base = to;
	remote.iov_len = n;

	return process_vm_writev(pid, &local, 1, &remote, 1, 0);
}
