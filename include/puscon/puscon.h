#ifndef PUSCON_PUSCON_H
#define PUSCON_PUSCON_H

/*
 * Type Definitions
 */

typedef struct puscon_config {
	/* filename of the helper kernel */
	char*		kernel_filename;
} puscon_config;

/*
 * Function Declarations
 */

int puscon_main(puscon_config* config);

#endif
