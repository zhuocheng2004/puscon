
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <puscon/puscon.h>

static int parse_options(puscon_config* config, int argc, char *argv[]) {
	/* reset pointers to NULL */
	bzero(config, sizeof(puscon_config));

	if (argc <= 2)
		return 1;
	
	config->kernel_filename = argv[1];
	config->entry_filename = argv[2];

	return 0;
}

static void usage(const char* prog_name) {
	if (!prog_name)
		prog_name = "puscon-cli";
	puscon_printk(KERN_NOTICE "Usage: %s <kernel_filename>\n", prog_name);
}

int main(int argc, char *argv[]) {
	/* parse options */
	puscon_config config;
	int option_error = parse_options(&config, argc, argv);

	if (option_error) {
		puscon_printk(KERN_EMERG "Error: failed to parse options\n");
	} else if (!config.kernel_filename) {
		puscon_printk(KERN_EMERG "Error: no kernel specified\n");
		option_error = 1;
	}

	if (option_error) {
		usage(argv[0]);
		return 1;
	}

	puscon_printk_level = 7;
	puscon_printk_use_ansi_color = 1;

	int err = puscon_main(&config);

	return err;
}
