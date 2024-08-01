
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <puscon/puscon.h>
#include <puscon/version.h>


static void usage(const char* prog_name) {
	if (!prog_name)
		prog_name = "puscon-cli";
	fprintf(stderr, "Usage: %s [options] <kernel_filename>\n", prog_name);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "    --ansi-color        output logs with ansi colors for different levels.\n");
	fprintf(stderr, "    --help              print this help message.\n");
	fprintf(stderr, "    --level <level>     only output logs with level <= the level specified (0: fatal ... 7: debug).\n");
	fprintf(stderr, "    --version           print version.\n");
}

static void version() {
	fprintf(stderr, "puscon " PUSCON_VERSION_STRING "\n");
}

static int parse_options(puscon_config* config, int argc, char *argv[]) {
	/* reset pointers to NULL */
	memset(config, 0, sizeof(puscon_config));

	int pos, cnt = 0;
	for (pos = 1; pos < argc && cnt < 2; pos++) {
		char *s = argv[pos];
		if (*s == '-') {	// options
			if (s[1] == '\0') {
				pos++;
				s = argv[pos];
				goto normal;
			} else if (strcmp(s, "--ansi-color") == 0) {
				puscon_printk_use_ansi_color = 1;
			} else if (strcmp(s, "--help") == 0) {
				usage(argv[0]);
				exit(0);
			} else if (strcmp(s, "--level") == 0) {
				pos++;
				s = argv[pos];
				if (!s) {
					fprintf(stderr, "Error: --level requires one argument. \n");
					return 1;
				}
				int level = atoi(s);
				if (!(level >= 0 && level <= 7)) {
					fprintf(stderr, "Error: invalid level %s, which should be one of 0 ... 7.\n", s);
					return 1;
				}
				puscon_printk_level = level;
			} else if (strcmp(s, "--version") == 0) {
				version();
				exit(0);
			} else {
				fprintf(stderr, "Warning: unrecognized argument %s.\n", s);
			}
			continue;
		}
	normal:
		if (cnt == 0) {
			config->kernel_filename = s;
			cnt++;
		} else if (cnt == 1) {
			config->entry_filename = s;
			cnt++;
		}
	}

	// remaining arguments are passed to entry program
	config->entry_argc = argc - pos + 1;
	config->entry_argv = argv + pos - 1;
	
	if (!config->kernel_filename) {
		fprintf(stderr, "Error: no kernel specified.\n");
		return 1;
	}

	if (!config->entry_filename) {
		fprintf(stderr, "Error: no entry specified.\n");
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	/* global setup */
	puscon_printk_use_ansi_color = 0;
	puscon_printk_level = 0;

	/* parse options */
	puscon_config config;
	int option_error = parse_options(&config, argc, argv);

	if (option_error) {
		fprintf(stderr, "Run %s --help to see usage.\n", argv[0]);
		return 1;
	}

	int err = puscon_main(&config);

	return err;
}
