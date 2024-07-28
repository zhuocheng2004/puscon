
#include <puscon/container_of.h>
#include <puscon/fs.h>
#include <puscon/mount.h>
#include <puscon/types.h>

typedef struct puscon_mount {
	struct puscon_mount*	mnt_parent;
	puscon_dentry*		mnt_mountpoint;
	puscon_vfsmount		mnt;

	list_head		mnt_mounts;	/* list of children, anchored here */
	list_head		mnt_child;	/* and going through their mnt_child */
	list_head		mnt_instance;	/* mount instance on sb->s_mounts */
} puscon_mount;

static inline puscon_mount* real_mount(puscon_vfsmount* mnt) {
	return container_of(mnt, puscon_mount, mnt);
}

static inline int mnt_has_parent(puscon_mount* mnt) {
	return mnt != mnt->mnt_parent;
}
