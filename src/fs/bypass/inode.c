
#include <puscon/fs.h>

static puscon_file_system_type bypass_fs_type = {
	.name		= "bypass",
};

int puscon_bypass_fs_init(struct puscon_context* context) {
	return puscon_register_filesystem(context, &bypass_fs_type);
}
