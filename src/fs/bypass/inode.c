
#include <puscon/fs.h>

static puscon_file_system_type bypass_fs_type = {
	.name		= "bypass",
};

int puscon_bypass_fs_init() {
	return puscon_register_filesystem(&bypass_fs_type);
}
