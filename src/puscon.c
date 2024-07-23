
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <puscon/puscon.h>

int puscon_context_init(puscon_context* context, puscon_config* config) {
	if (!context) {
		fprintf(stderr, "Error: bad context.\n");
		return 1;
	}
	if (!config) {
		fprintf(stderr, "Error: bad config.\n");
		return 1;
	}

	context->config = config;
	task_info *entry_task = calloc(sizeof(task_info), 1);
	if (!entry_task)
		return 1;

	entry_task->pid = 1;
	entry_task->kernel = 1;
	entry_task->bypass = 1;
	context->entry_task = entry_task;

	context->should_stop = 0;

	return 0;
}

int puscon_context_destroy(puscon_context* context) {
	if (!context) {
		fprintf(stderr, "Error: bad context.\n");
		return 1;
	}
	if (context->entry_task)
		free(context->entry_task);
	return 0;
}

int puscon_start(puscon_context* context) {
	if (!context || !context->config) {
		fprintf(stderr, "Error: bad context.\n");
		return 1;
	}
	puscon_config *config = context->config;

	if (!config->kernel_filename) {
		fprintf(stderr, "Error: no kernel specified.\n");
		return 1;
	}

	pid_t child_pid = fork();

	if (child_pid < 0) {
		fprintf(stderr, "Error: failed to fork process when trying to exec.\n");
		return 1;
	}

	if (child_pid == 0) {
		/* child */
		char* argv[] = {
			config->kernel_filename, NULL,
		};

		char* env[] = {
			NULL,
		};

		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		int err = execve(config->kernel_filename, argv, NULL);
		if (err) {
			fprintf(stderr, "Error: failed to exec %s.\n", config->kernel_filename);
		}
		exit(1);
	}

	/* parent */
	ptrace(PTRACE_SETOPTIONS, child_pid, NULL, PTRACE_O_EXITKILL);

	return 0;
}

static void skip_syscall(pid_t child_pid, struct user_regs_struct* regs) {
	regs->orig_rax = -1;
	ptrace(PTRACE_SETREGS, child_pid, 0, regs);
}

int puscon_main(puscon_config* config) {
	int err;

	puscon_context context;
	err = puscon_context_init(&context, config);
	if (err) {
		fprintf(stderr, "Error: failed to init context.\n");
		return err;
	}

	fprintf(stderr, "Starting...\n");

	err = puscon_start(&context);
	if (err) {
		fprintf(stderr, "Error: failed to start process.\n");
		return err;
	}

	pid_t child_pid;
	int child_status;

	while (1) {
		child_pid = wait(&child_status);
		if (WIFEXITED(child_status)) {
			if (WEXITSTATUS(child_status))
				err = 1;
			break;
		}
		if (WIFSTOPPED(child_status)) {
			int sig = WSTOPSIG(child_status);

			switch (sig) {
				case SIGTRAP:
					task_info *task = context.entry_task;
					struct user_regs_struct regs;
					ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
					u64 syscall = regs.orig_rax;

					switch (syscall) {
						case SYS_puscon_nop:
							ptrace(task->kernel && task->bypass ? PTRACE_SYSCALL : PTRACE_SYSEMU, child_pid, NULL, NULL);
							break;
						case SYS_puscon_kernel_enter:
							fprintf(stderr, "Entering kernel mode.\n");
							task->kernel = 1;
							ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
							break;
						case SYS_puscon_kernel_exit:
							fprintf(stderr, "Exiting kernel mode.\n");
							task->kernel = 0;
							task->bypass = 0;
							ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
							break;
						case SYS_puscon_bypass_enable:
							if (!task->kernel) {
								fprintf(stderr, "Error: can only bypass in kernel mode.\n");
								err = 1;
								break;
							}
							fprintf(stderr, "Enabling bypassing.\n");
							task->bypass = 1;
							ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
							break;
						case SYS_puscon_bypass_disable:
							fprintf(stderr, "Disabling bypassing.\n");
							task->bypass = 0;
							ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
							break;
						default:
							if (task->kernel && task->bypass) {
								fprintf(stderr, "Bypassing: syscall %lld.\n", syscall);
								ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
							} else {
								err = puscon_syscall_handle(&context, child_pid);
								if (err) {
									fprintf(stderr, "Error: failed to handle syscall (%lld).\n", syscall);
									break;
								}
								ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
							}
					}
					break;
				case SIGSEGV:
					fprintf(stderr, "Error: child (pid=%d) Segmentation Fault.\n", child_pid);
					err = 1;
					break;
				default:
					fprintf(stderr, "Error: unhandled signal: %d \n", sig);
					err = 1;
			}

			if (err || context.should_stop) {
				break;
			}
		}
	}

	puscon_context_destroy(&context);

	fprintf(stderr, "Finished.\n");

	return err;
}
