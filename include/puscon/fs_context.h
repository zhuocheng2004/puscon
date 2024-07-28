#ifndef PUSCON_FS_CONTEXT_H
#define PUSCON_FS_CONTEXT_H

struct puscon_dentry;
struct puscon_file_system_type;
struct puscon_fs_context_operations;
struct puscon_super_block;

typedef enum puscon_fs_context_purpose {
	PUSCON_FS_CONTEXT_FOR_MOUNT,		/* New superblock for explicit mount */
} puscon_fs_context_purpose;

/*
 * Filesystem context for holding the parameters used in the creation or
 * reconfiguration of a superblock.
 * 
 * Superblock creation fills in ->root whereas reconfiguration begins with this
 * already set.
 */
typedef struct puscon_fs_context {
	const struct puscon_fs_context_operations*	ops;
	struct puscon_file_system_type*	fs_type;
	void*				fs_private;	/* The filesystem's context */
	struct puscon_dentry*		root;		/* The root and superblock */
	const char*			source;		/* The source name (eg. dev path) */
	puscon_fs_context_purpose	purpose : 8;
	bool				need_free : 1;	/* Need to call ops->free() */
} puscon_fs_context;

typedef struct puscon_fs_context_operations {
	void	(*free) (puscon_fs_context* fc);
	int	(*dup) (puscon_fs_context* fc, puscon_fs_context* src_fc);
	int	(*get_tree) (puscon_fs_context* fc);
	int	(*reconfigure) (puscon_fs_context* fc);
} puscon_fs_context_operations;

puscon_fs_context* puscon_fs_context_for_mount(struct puscon_file_system_type* fs_type, unsigned int sb_flags);

int puscon_vfs_get_tree(puscon_fs_context* fc);

void puscon_put_fs_context(puscon_fs_context* fc);

int puscon_get_tree_nodev(puscon_fs_context* fc,
	int (*fill_super) (struct puscon_super_block* sb, puscon_fs_context* fc));

#endif
