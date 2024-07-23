
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
	context->entry_task = entry_task;

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
			if (sig == SIGTRAP) {
				task_info *task = context.entry_task;
				struct user_regs_struct regs;
				ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
				u64 syscall = regs.orig_rax;

				if (!task->skipping) {
					switch (syscall) {
					case SYS_puscon_exit_init:
						skip_syscall(child_pid, &regs);
						task->skipping = 1;
						break;
					default:
						if (task->initialized && !task->bypass) {
							err = puscon_syscall_handle(&context, child_pid);
							if (err) {
								fprintf(stderr, "Error: failed to handle syscall (%lld).\n", syscall);
								break;
							}
						}
					}
				} else {
					task->skipping = 0;
				}

				switch (syscall)
				{
				case SYS_puscon_exit_init:
					ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
					task->initialized = 1;
					break;
				default:
					if (task->initialized && !task->bypass) {
						err = puscon_syscall_handle(&context, child_pid);
						if (err) {
							fprintf(stderr, "Error: failed to handle syscall (%lld).\n", syscall);
							break;
						}
						ptrace(PTRACE_SYSEMU, child_pid, NULL, NULL);
					} else {
						ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
					}
				}
			}

			if (err) {
				break;
			}
		}
	}

	puscon_context_destroy(&context);

	return err;
}
