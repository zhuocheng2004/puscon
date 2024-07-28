#ifndef PUSCON_MOUNT_H
#define PUSCON_MOUNT_H

struct puscon_file_system_type;
struct puscon_fs_context;
struct puscon_super_block;
struct puscon_dentry;

typedef struct puscon_vfsmount {
	struct puscon_dentry*	mnt_root;	/* root of the mounted tree */
	struct puscon_super_block*	mnt_sb;	/* pointer to superblock */
} puscon_vfsmount;

puscon_vfsmount* puscon_fc_mount(struct puscon_fs_context* fc);
puscon_vfsmount* puscon_vfs_create_mount(struct puscon_fs_context* fc);
puscon_vfsmount* puscon_vfs_kern_mount(struct puscon_file_system_type* type, int flags, 
					const char* name, void* data);

#endif
