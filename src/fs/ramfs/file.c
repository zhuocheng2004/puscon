
#include <puscon/fs.h>

#include "internal.h"

const puscon_file_operations puscon_ramfs_file_operations = {
	.read		= NULL,
	.write		= NULL,
	.llseek		= NULL,
};

const puscon_inode_operations puscon_ramfs_file_inode_operations = { };
