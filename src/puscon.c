
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <puscon/fs.h>
#include <puscon/puscon.h>


int puscon_context_init_fs(puscon_context* context, puscon_config* config) {
	puscon_mnt_init(context);

	return 0;
}

int puscon_context_init(puscon_context* context, puscon_config* config) {
	if (!context) {
		puscon_printk(KERN_EMERG "Error: bad context.\n");
		goto err_out;
	}

	if (!config) {
		puscon_printk(KERN_EMERG "Error: bad config.\n");
		goto err_out;
	}

	context->config = config;

	if (puscon_idmap_init(&context->task_context.tasks, PID_MAX_SHIFT)) {
		puscon_printk(KERN_EMERG "Error: failed to init idmap \"tasks\".\n");
		goto err_out;
	}

	if (!(context->task_context.pid_map = calloc(1 << HOST_PID_MAX_SHIFT, 4))) {
		puscon_printk(KERN_EMERG "Error: failed to alloc pid_map.\n");
		goto err_destroy_tasks;
	}

	puscon_task_info *entry_task = calloc(1, sizeof(puscon_task_info));
	if (!entry_task)
		goto err_free_pidmap;

	// mark pid 0 as unusable
	context->task_context.tasks.free_map[0] = 1;

	entry_task->pid = 1;
	context->task_context.entry_task = entry_task;
	context->task_context.tasks.free_map[1] = 1;
	context->task_context.tasks.ptrs[1] = entry_task;

	context->should_stop = 1;

	if (puscon_context_init_fs(context, config)) {
		puscon_printk(KERN_EMERG "ERROR: failed to init filesystem.\n");
		goto err_free_pidmap;
	}

	return 0;

err_free_pidmap:
	free(context->task_context.pid_map);
err_destroy_tasks:
	puscon_idmap_destroy(&context->task_context.tasks);
err_out:
	return 1;
}

int puscon_context_destroy(puscon_context* context) {
	if (!context) {
		puscon_printk(KERN_EMERG "Error: bad context.\n");
		return 1;
	}

	if (context->task_context.entry_task)
		free(context->task_context.entry_task);

	if (context->task_context.pid_map)
		free(context->task_context.pid_map);

	puscon_idmap_destroy(&context->task_context.tasks);

	return 0;
}

int puscon_start(puscon_context* context) {
	if (!context || !context->config) {
		puscon_printk(KERN_EMERG "Error: bad context.\n");
		return 1;
	}
	puscon_config *config = context->config;

	if (!config->kernel_filename) {
		puscon_printk(KERN_EMERG "Error: no kernel specified.\n");
		return 1;
	}

	pid_t child_pid = fork();

	if (child_pid < 0) {
		puscon_printk(KERN_EMERG "Error: failed to fork process when trying to exec.\n");
		return 1;
	}

	if (child_pid == 0) {
		/* child */
		char* argv[] = {
			config->kernel_filename, config->entry_filename, NULL
		};

		char* env[] = {
			NULL,
		};

		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		raise(SIGSTOP);
		int err = execve(config->kernel_filename, argv, NULL);
		if (err) {
			puscon_printk(KERN_EMERG "Error: failed to exec %s.\n", config->kernel_filename);
		}
		exit(1);
	}

	/* parent */
	ptrace(PTRACE_SETOPTIONS, child_pid, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC);

	puscon_task_info *entry_task = context->task_context.entry_task;
	entry_task->host_pid = child_pid;
	entry_task->kernel = 1;
	entry_task->bypass = 1;
	if (child_pid >= (1 << HOST_PID_MAX_SHIFT)) {
		puscon_printk(KERN_EMERG "Error: host pid %d is too large (max: %d).\n", child_pid, (1 << HOST_PID_MAX_SHIFT) - 1);
		return 1;
	}
	context->task_context.pid_map[child_pid] = 1;

	context->task_context.current_task = entry_task;

	context->should_stop = 0;

	waitpid(child_pid, NULL, __WALL);
	puscon_printk(KERN_INFO "Child [pid=1, host_pid=%d] is now under control.\n", child_pid);

	ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);

	return 0;
}

static char* get_args_string(int argc, char* argv[]) {
	int end = 0;
	for (int i = 0; i < argc; i++)
		end = end + strlen(argv[i]) + 4;
	if (end > 0)
		end -= 2;

	if (end == 0)
		return NULL;

	char* s = malloc(end + 4);
	char* cur = s;
	for (int i = 0; i < argc; i++) {
		*cur = '\"'; cur++;
		char* arg = argv[i];
		strcpy(cur, arg); cur += strlen(arg);
		*cur = '\"'; cur++;
		*cur = ','; cur++;
		*cur = ' '; cur++;
	}
	s[end] = '\0';

	return s;
}

int puscon_main(puscon_config* config) {
	int err;

	puscon_context context;
	err = puscon_context_init(&context, config);
	if (err) {
		puscon_printk(KERN_EMERG "Error: failed to init context.\n");
		return err;
	}

	char* arg_str = get_args_string(config->entry_argc, config->entry_argv);
	puscon_printk(KERN_INFO "Starting Puscon with kernel=\"%s\", entry=\"%s\", args=[%s].\n", config->kernel_filename, config->entry_filename, arg_str ? arg_str : "");
	free(arg_str);

	err = puscon_start(&context);
	if (err) {
		puscon_printk(KERN_EMERG "Error: failed to start process.\n");
		return err;
	}

	pid_t child_pid;
	int child_status;


	while (1) {
		err = 0;
		child_pid = wait(&child_status);
		if (WIFEXITED(child_status)) {
			if (WEXITSTATUS(child_status))
				err = 1;
			break;
		} else if (WIFSTOPPED(child_status)) {
			int sig = WSTOPSIG(child_status);

			switch (sig) {
				case SIGTRAP:
					struct user_regs_struct regs;
					ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
					u64 syscall = regs.orig_rax;

					u32 pid = context.task_context.pid_map[child_pid];
					if (pid == 0) {
						puscon_printk(KERN_EMERG "Error: cannot find child with host pid %d.\n", child_pid);
						err = 1;
						break;
					}

					puscon_task_info *task = context.task_context.tasks.ptrs[pid];
					context.task_context.current_task = task;

					switch (syscall) {
						case SYS_puscon_nop:
							skip_syscall(&context);
							break;
						case SYS_puscon_kernel_enter:
							puscon_printk(KERN_DEBUG "[PID %d] Entering kernel mode.\n", pid);
							task->kernel = 1;
							skip_syscall(&context);
							break;
						case SYS_puscon_kernel_exit:
							puscon_printk(KERN_DEBUG "[PID %d] Exiting kernel mode.\n", pid);
							task->kernel = 0;
							task->bypass = 0;
							skip_syscall(&context);
							break;
						case SYS_puscon_bypass_enable:
							if (!task->kernel) {
								puscon_printk(KERN_EMERG "[PID %d] Error: can only bypass in kernel mode.\n", pid);
								err = 1;
								break;
							}
							puscon_printk(KERN_DEBUG "[PID %d] Enabling bypassing.\n", pid);
							task->bypass = 1;
							skip_syscall(&context);
							break;
						case SYS_puscon_bypass_disable:
							puscon_printk(KERN_DEBUG "[PID %d] Disabling bypassing.\n", pid);
							task->bypass = 0;
							skip_syscall(&context);
							break;
						case SYS_puscon_set_syscall_entry:
							u64 syscall_entry = regs.rdi;
							puscon_printk(KERN_INFO "[PID %d] Syscall entry set to 0x%llx.\n", pid, syscall_entry);
							task->syscall_entry = syscall_entry;
							skip_syscall(&context);
							break;
						default:
							if (task->kernel && task->bypass) {
								puscon_printk(KERN_DEBUG "[PID %d] Bypassing: syscall %lld.\n", pid, syscall);
							} else {
								err = puscon_syscall_handle(&context);
								if (err) {
									puscon_printk(KERN_EMERG "[PID %d] Error: failed to handle syscall (nr=%lld).\n", pid, syscall);
								}
							}
					}
					if (!(err || context.should_stop))
						ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
					break;
				case SIGSEGV:
					puscon_printk(KERN_EMERG "[PID %d] Error: child (host_pid=%d) Segmentation Fault.\n", pid, child_pid);
					err = 1;
					break;
				default:
					puscon_printk(KERN_EMERG "[PID %d] Error: unhandled signal: %d \n", pid, sig);
					err = 1;
			}

			if (err || context.should_stop) {
				break;
			}
		} else {
			puscon_printk(KERN_EMERG "Unhandled signal status %d from child %d.\n", child_status, child_pid);
			err = 1;
			break;
		}
	}

	puscon_context_destroy(&context);

	puscon_printk(KERN_INFO "Puscon Exited.\n");

	return err;
}
