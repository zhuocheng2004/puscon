
#include <puscon/err.h>
#include <puscon/fs.h>
#include <puscon/fs_context.h>
#include <puscon/mount.h>
#include <puscon/puscon.h>

#include "mount.h"


static puscon_mount* alloc_vfsmnt(const char* name) {
	puscon_mount *mnt = puscon_kzalloc(sizeof(puscon_mount));
	if (mnt) {
		INIT_LIST_HEAD(&mnt->mnt_child);
		INIT_LIST_HEAD(&mnt->mnt_mounts);
	}
	return mnt;
}

puscon_vfsmount* puscon_vfs_create_mount(struct puscon_fs_context* fc) {
	puscon_mount *mnt;

	if (!fc->root)
		return ERR_PTR(-EINVAL);

	mnt = alloc_vfsmnt(fc->source ?: "none");
	if (!mnt)
		return ERR_PTR(-ENOMEM);

	mnt->mnt.mnt_sb		= fc->root->d_sb;
	mnt->mnt.mnt_root	= puscon_dget(fc->root);
	mnt->mnt_mountpoint	= mnt->mnt.mnt_root;
	mnt->mnt_parent		= mnt;

	list_add_tail(&mnt->mnt_instance, &mnt->mnt.mnt_sb->s_mounts);
	return &mnt->mnt;
}

puscon_vfsmount* puscon_fc_mount(struct puscon_fs_context* fc) {
	int err = puscon_vfs_get_tree(fc);
	if (!err) {
		return puscon_vfs_create_mount(fc);
	}
	return ERR_PTR(err);
}

puscon_vfsmount* puscon_vfs_kern_mount(puscon_context* context, struct puscon_file_system_type* type, int flags, 
	const char* name, void* data) {
	puscon_fs_context *fc;
	puscon_vfsmount *mnt;
	int ret = 0;

	if (!type)
		return ERR_PTR(-EINVAL);

	fc = puscon_fs_context_for_mount(context, type, flags);
	if (IS_ERR(fc))
		return ERR_CAST(fc);

	mnt = puscon_fc_mount(fc);

	puscon_put_fs_context(fc);
	return mnt;
}


static int rootfs_init_fs_context(puscon_fs_context* fc) {
	return puscon_ramfs_init_fs_context(fc);
}

static puscon_file_system_type rootfs_fs_type = {
	.name		= "rootfs",
	.init_fs_context = rootfs_init_fs_context,
	.kill_sb	= puscon_kill_litter_super,
};

static int init_mount_tree(puscon_context* context) {
	puscon_vfsmount *mnt;
	puscon_mount *m;
	puscon_path root;

	mnt = puscon_vfs_kern_mount(context, &rootfs_fs_type, 0, "rootfs", NULL);
	if (IS_ERR(mnt)) {
		puscon_printk(KERN_EMERG "Error: can't create rootfs.\n");
		return 1;
	}
	m = real_mount(mnt);

	root.mnt = mnt;
	root.dentry = mnt->mnt_root;

	puscon_task_info *entry_task = context->task_context.entry_task;
	entry_task->fs.pwd = root;
	entry_task->fs.root = root;

	return 0;
}

int puscon_mnt_init(puscon_context* context) {
	int err;

	err = init_mount_tree(context);
	if (err) {
		puscon_printk(KERN_EMERG "Error: init_mount_tree() failed.\n");
		return err;
	}

	return 0;
}
