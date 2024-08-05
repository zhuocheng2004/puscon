
#include <puscon/err.h>
#include <puscon/fs.h>
#include <puscon/fs_context.h>

/**
 * alloc_fs_context - Create a filesystem context.
 * @fs_type: The filesystem type.
 * @reference: The dentry from which this one derives (or NULL)
 * @sb_flags: Filesystem/superblock flags (SB_*)
 * @sb_flags_mask: Applicable members of @sb_flags
 * @purpose: The purpose that this configuration shall be used for.
 *
 * Open a filesystem and create a mount context.  The mount context is
 * initialised with the supplied flags and, if a submount/automount from
 * another superblock (referred to by @reference) is supplied, may have
 * parameters such as namespaces copied across from that superblock.
 */
static puscon_fs_context* alloc_fs_context(puscon_context* context, puscon_file_system_type* fs_type,
	puscon_dentry* reference, unsigned int sb_flags, unsigned int sb_flags_mask,
	puscon_fs_context_purpose purpose) {
	int (*init_fs_context) (puscon_fs_context*);
	puscon_fs_context *fc;
	int ret = -ENOMEM;

	fc = puscon_kzalloc(sizeof(puscon_fs_context));
	if (!fc)
		return ERR_PTR(-ENOMEM);

	fc->context	= context;
	fc->purpose	= purpose;
	fc->fs_type	= puscon_get_filesystem(context, fs_type);

	switch (purpose) {
		case PUSCON_FS_CONTEXT_FOR_MOUNT:
			break;
	}

	init_fs_context = fc->fs_type->init_fs_context;
	if (!init_fs_context) {
		ret = -EINVAL;
		goto err_fc;
	}

	ret = init_fs_context(fc);
	if (ret < 0)
		goto err_fc;
	fc->need_free = true;
	return fc;

err_fc:
	puscon_put_fs_context(fc);
	return ERR_PTR(ret);
}

puscon_fs_context* puscon_fs_context_for_mount(puscon_context* context, puscon_file_system_type* fs_type, unsigned int sb_flags) {
	return alloc_fs_context(context, fs_type, NULL, sb_flags, 0, PUSCON_FS_CONTEXT_FOR_MOUNT);
}

/**
 * put_fs_context - Dispose of a superblock configuration context.
 * @fc: The context to dispose of.
 */
void puscon_put_fs_context(puscon_fs_context* fc) {
	puscon_super_block *sb;

	if (fc->root) {
		sb = fc->root->d_sb;
		puscon_dput(fc->root);
		fc->root = NULL;
	}

	if (fc->need_free && fc->ops && fc->ops->free)
		fc->ops->free(fc);

	puscon_put_filesystem(fc->context, fc->fs_type);
	if (fc->source)
		puscon_kfree((void*) fc->source);
	puscon_kfree(fc);
}
