
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <puscon/puscon.h>

int puscon_start(puscon_config* config) {
	if (!config->kernel_filename) {
		fprintf(stderr, "no kernel specified");
		return 1;
	}

	pid_t child_pid = fork();

	if (child_pid < 0) {
		fprintf(stderr, "failed to fork process when trying to exec\n");
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
			fprintf(stderr, "Error: failed to exec %s\n", config->kernel_filename);
			exit(1);
		}
	}

	/* parent */

	return 0;
}

int puscon_main(puscon_config* config) {

	int err = puscon_start(config);
	if (err)
		return err;

	pid_t child_pid;
	int child_status;

	while (1) {
		child_pid = wait(&child_status);
		if (WIFEXITED(child_status)) {
			if (WEXITSTATUS(child_status))
				err = 1;
			break;
		}

		ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	}

	return err;
}
