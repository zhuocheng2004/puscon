#ifndef PUSCON_FS_H
#define PUSCON_FS_H


#include <puscon/dcache.h>
#include <puscon/list.h>
#include <puscon/mount.h>
#include <puscon/puscon.h>
#include <puscon/types.h>


struct puscon_fs_context;
struct puscon_vfsmount;


typedef struct puscon_inode {
	;
} puscon_inode;


typedef struct puscon_file {
	puscon_inode*	f_inode;
	/* needed for tty driver, and maybe others */
	void*		private_data;
} puscon_file;

static inline puscon_inode* file_inode(const puscon_file* f)
{
	return f->f_inode;
}


typedef struct puscon_super_block {
} puscon_super_block;


typedef struct puscon_file_operations {
	u32		(*llseek) (puscon_file*, u32, int);
	u32		(*read)	(puscon_file*, char*, size_t, u32*);
	u32		(*write) (puscon_file*, const char*, size_t, u32*);
	int		(*open) (puscon_inode*, puscon_file*);
} puscon_file_operations;


typedef struct puscon_inode_operations {
	puscon_inode*	(*alloc_inode) (puscon_super_block* sb);
	void		(*destroy_inode) (puscon_inode*);
	void		(*free_inode) (puscon_inode*);
	void		(*shutdown) (puscon_super_block* sb);
} puscon_inode_operations;


typedef struct puscon_super_operations {
	;
} puscon_super_operations;


typedef struct puscon_file_system_type {
	const char*	name;

	int		(*init_fs_context) (struct puscon_fs_context*);
	puscon_dentry*	(*mount) (struct puscon_file_system_type*, int, const char*, void*);
	void		(*kill_sb) (puscon_super_block*);
	struct puscon_file_system_type*	next;
} puscon_file_system_type;

int puscon_register_filesystem(puscon_file_system_type*);
int puscon_unregister_filesystem(puscon_file_system_type*);

int puscon_init_ramfs_fs(void);
int puscon_bypass_fs_init(void);

#endif
