#ifndef PUSCON_DCACHE_H
#define PUSCON_DCACHE_H

struct puscon_inode;
struct puscon_super_block;

typedef struct puscon_dentry {
	struct puscon_dentry*	d_parent;
	struct puscon_inode*	d_inode;

	const struct puscon_dentry_operations*	d_op;
	struct puscon_super_block*	d_sb;	/* The root of the dentry tree */
} puscon_dentry;

typedef struct puscon_dentry_operations {
	int		(*d_delete) (const puscon_dentry*);
	int		(*d_init) (puscon_dentry*);
	void		(*d_releasse) (puscon_dentry*);
	void		(*d_iput) (puscon_dentry*);
} puscon_dentry_operations;

#endif
