#ifndef PUSCON_MOUNT_H
#define PUSCON_MOUNT_H

struct puscon_super_block;
struct puscon_dentry;

typedef struct puscon_vfsmount {
	struct puscon_dentry*	mnt_root;	/* root of the mounted tree */
	struct puscon_super_block*	mnt_sb;	/* pointer to superblock */
} puscon_vfsmount;

#endif
