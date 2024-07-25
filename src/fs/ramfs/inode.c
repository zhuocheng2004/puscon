
#include <puscon/fs.h>
#include <puscon/fs_context.h>

static puscon_file_system_type ramfs_fs_type = {
	.name		= "ramfs",
};

int puscon_init_ramfs_fs(void) {
	return puscon_register_filesystem(&ramfs_fs_type);
}
