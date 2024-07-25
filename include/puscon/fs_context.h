#ifndef PUSCON_FS_CONTEXT_H
#define PUSCON_FS_CONTEXT_H

struct puscon_dentry;
struct puscon_file_system_type;
struct puscon_fs_context_operations;

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
	void*				fs_private;
	struct puscon_dentry*			root;
	const char*			source;
} puscon_fs_context;

typedef struct puscon_fs_context_operations {
	void	(*free) (puscon_fs_context* fc);
	int	(*dup) (puscon_fs_context* fc, puscon_fs_context* src_fc);
	int	(*get_tree) (puscon_fs_context* fc);
	int	(*reconfigure) (puscon_fs_context* fc);
} puscon_fs_context_operations;

#endif
