#ifndef PUSCON_FS_H
#define PUSCON_FS_H

#include <puscon/dcache.h>
#include <puscon/list.h>
#include <puscon/mount.h>
#include <puscon/puscon.h>
#include <puscon/types.h>


struct puscon_inode_operations;
struct puscon_file_operations;
struct puscon_fs_context;
struct puscon_super_block;
struct puscon_super_operations;
struct puscon_vfsmount;


typedef struct puscon_inode {
	mode_t				i_mode;
	const struct puscon_inode_operations*	i_op;
	struct puscon_super_block*	i_sb;

	/*
	 * Filesystems may only read i_nlink directly.  They shall use the
	 * following functions for modification:
	 *
	 *    (set|clear|inc|drop)_nlink
	 *    inode_(inc|dec)_link_count
	 */
	union {
		const unsigned int	i_nlink;
		unsigned int		__i_nlink;
	};

	list_head			i_sb_list;

	hlist_head			i_dentry;

	int				i_count;

	union {
		const struct puscon_file_operations*	i_fop;
		void	(*free_inode) (struct puscon_inode*);
	};

	void*				i_private;	/* fs or device private pointer */
} puscon_inode;


typedef struct puscon_file {
	mode_t		f_mode;
	int		f_count;
	off_t		f_pos;
	puscon_inode*	f_inode;
	const struct puscon_file_operations*	f_op;

	/* needed for tty driver, and maybe others */
	void*		private_data;
} puscon_file;

static inline puscon_file* get_file(puscon_file* f) {
	int prior = f->f_count;
	if (!prior)
		puscon_printk(KERN_WARNING "Warning: struct file::f_count incremented from zero; use-after-free condition present!\n");
	return f;
}

static inline puscon_inode* file_inode(const puscon_file* f) {
	return f->f_inode;
}


typedef struct puscon_super_block {
	list_head			s_list;		/* Kepp this first */
	struct puscon_file_system_type*	s_type;
	const struct puscon_super_operations*	s_op;
	puscon_dentry*			s_root;
	int				s_count;

	list_head			s_mounts;	/* list of mounts; _not_ for fs use */
	hlist_node			s_instances;

	char				s_id[32];	/* Informational name */

	const puscon_dentry_operations*	s_d_op;		/* default d_op for dentries */

	list_head			s_inodes;	/* all inodes */
} puscon_super_block;


/*
 * VFS helper functions..
 */
int puscon_vfs_mkdir(puscon_inode*, puscon_dentry*, mode_t);


typedef struct puscon_file_operations {
	off_t		(*llseek) (puscon_file*, off_t, int);
	ssize_t		(*read)	(puscon_file*, char*, size_t, off_t*);
	ssize_t		(*write) (puscon_file*, const char*, size_t, off_t*);
	int		(*open) (puscon_inode*, puscon_file*);
	int		(*flush) (puscon_file*);
	int		(*release) (puscon_inode*, puscon_file*);
} puscon_file_operations;


typedef struct puscon_inode_operations {
	puscon_dentry*	(*lookup) (puscon_inode*, puscon_dentry*, unsigned int);
	int		(*create) (puscon_inode*, puscon_dentry*, mode_t, bool);
	int		(*mkdir) (puscon_inode*, puscon_dentry*, mode_t);
	int		(*rmdir) (puscon_inode*, puscon_dentry*);
} puscon_inode_operations;


typedef struct puscon_super_operations {
	puscon_inode*	(*alloc_inode) (puscon_super_block* sb);
	void		(*destroy_inode) (puscon_inode*);
	void		(*free_inode) (puscon_inode*);

	int		(*drop_inode) (puscon_inode*);		// return !0 if ref dropped to zero
	void		(*evict_inode) (puscon_inode*);		// disconnect from lists it connected to and detach pages
								// will call destroy
	void		(*shutdown) (puscon_super_block* sb);
} puscon_super_operations;


void puscon_inc_nlink(puscon_inode* inode);
void puscon_drop_nlink(puscon_inode* inode);
void puscon_clear_nlink(puscon_inode* inode);
void puscon_set_nlink(puscon_inode* inode, unsigned int nlink);


typedef struct puscon_file_system_type {
	const char*	name;

	int		(*init_fs_context) (struct puscon_fs_context*);
	puscon_dentry*	(*mount) (struct puscon_file_system_type*, int, const char*, void*);
	void		(*kill_sb) (puscon_super_block*);
	struct puscon_file_system_type*	next;
	hlist_head	fs_supers;
} puscon_file_system_type;

void puscon_kill_litter_super(puscon_super_block* sb);

puscon_super_block* puscon_sget_fc(struct puscon_fs_context* fc,
	int (*test) (puscon_super_block*, struct puscon_fs_context*),
	int (*set) (puscon_super_block*, struct puscon_fs_context*));

void puscon_iput(puscon_inode*);


/* fs/open.c */
typedef struct puscon_filename {
	const char		*name;		/* pointer to actual string */
	const __user char	*uptr;		/* original userland pointer */
	int			refcnt;
	const char		iname[];
} puscon_filename;

puscon_filename* puscon_getname_flags(puscon_context*, const char __user*, int);
puscon_filename* puscon_getname(puscon_context*, const char __user*);
puscon_filename* puscon_getname_kernel(const char*);
void puscon_putname(puscon_filename* name);

#define __getname()		puscon_kmalloc(PATH_MAX)
#define __putname(name)		puscon_kfree((void*) name)


int puscon_register_filesystem(puscon_context*, puscon_file_system_type*);
int puscon_unregister_filesystem(puscon_context*, puscon_file_system_type*);

int puscon_generic_delete_inode(puscon_inode* inode);
static inline int puscon_generic_drop_inode(puscon_inode* inode) {
	return !inode->i_nlink;
}

void puscon___iget(puscon_inode* inode);
void puscon_clear_inode(puscon_inode*);
puscon_inode* puscon_new_inode(puscon_super_block* sb);

puscon_file_system_type* puscon_get_filesystem(puscon_context*, puscon_file_system_type* fs);
void puscon_put_filesystem(puscon_context*, puscon_file_system_type* fs);

int puscon_mnt_init(puscon_context* context);
int puscon_ramfs_init_fs_context(struct puscon_fs_context* fc);
int puscon_init_ramfs_fs(struct puscon_context* context);
int puscon_bypass_fs_init(struct puscon_context* context);

#endif
