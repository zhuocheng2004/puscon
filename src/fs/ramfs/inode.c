
#include <puscon/err.h>
#include <puscon/fs.h>
#include <puscon/fs_context.h>

#include "internal.h"

static const puscon_super_operations ramfs_ops;
static const puscon_inode_operations ramfs_dir_inode_operations;

static puscon_inode* ramfs_get_inode(puscon_super_block* sb, const puscon_inode* dir, mode_t mode) {
	puscon_inode *inode = puscon_new_inode(sb);

	if (inode) {
		switch (mode & __S_IFMT) {
			default:
				break;
			case __S_IFREG:
				inode->i_op = &puscon_ramfs_file_inode_operations;
				inode->i_fop = &puscon_ramfs_file_operations;
				break;
			case __S_IFDIR:
				inode->i_op = &ramfs_dir_inode_operations;
				inode->i_fop = NULL;
				puscon_inc_nlink(inode);
				break;
		}
	}

	return inode;
}

static int ramfs_mknod(puscon_inode* dir, puscon_dentry* dentry, mode_t mode) {
	puscon_inode* inode = ramfs_get_inode(dir->i_sb, dir, mode);
	int error = -ENOSPC;

	if (inode) {
		puscon_d_instantiate(dentry, inode);
		puscon_dget(dentry);
		error = 0;
	}

	return error;
}

static int ramfs_mkdir(puscon_inode* dir, puscon_dentry* dentry, mode_t mode) {
	int retval = ramfs_mknod(dir, dentry, mode | __S_IFDIR);
	if (!retval)
		puscon_inc_nlink(dir);
	return retval;
}

static int ramfs_create(puscon_inode* dir, puscon_dentry* dentry, mode_t mode, bool excl) {
	return ramfs_mknod(dir, dentry, mode | __S_IFREG);
}

static const puscon_inode_operations ramfs_dir_inode_operations = {
	.create		= ramfs_create,
	.lookup		= NULL,
	.mkdir		= ramfs_mkdir,
	.rmdir		= NULL,
};

static const puscon_super_operations ramfs_ops = {
	.drop_inode	= puscon_generic_delete_inode,
};

static int ramfs_fill_super(puscon_super_block* sb, puscon_fs_context* fc) {
	puscon_inode *inode;

	sb->s_op	= &ramfs_ops;

	inode = ramfs_get_inode(sb, NULL, __S_IFDIR);
	sb->s_root = puscon_d_make_root(inode);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}

static int ramfs_get_tree(puscon_fs_context* fc) {
	return puscon_get_tree_nodev(fc, ramfs_fill_super);
}

static void ramfs_free_fc(puscon_fs_context* fc) { }

static const puscon_fs_context_operations ramfs_context_ops = {
	.free		= ramfs_free_fc,
	.get_tree	= ramfs_get_tree,
};

int puscon_ramfs_init_fs_context(puscon_fs_context* fc) {
	fc->ops = &ramfs_context_ops;
	return 0;
}

static void ramfs_kill_sb(puscon_super_block* sb) {
	puscon_kill_litter_super(sb);
}

static puscon_file_system_type ramfs_fs_type = {
	.name		= "ramfs",
	.init_fs_context = puscon_ramfs_init_fs_context,
	.kill_sb	= ramfs_kill_sb,
};

int puscon_init_ramfs_fs(void) {
	return puscon_register_filesystem(&ramfs_fs_type);
}
