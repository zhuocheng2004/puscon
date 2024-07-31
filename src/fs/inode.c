
#include <string.h>

#include <puscon/err.h>
#include <puscon/fs.h>
#include <puscon/types.h>

static void destroy_inode(puscon_inode* inode);

static int no_open(puscon_inode* inode, puscon_file* file) {
	return -ENXIO;
}

/**
 * puscon_inode_init_always - perform inode structure initialisation
 * @sb: superblock inode belongs to
 * @inode: inode to initialise
 *
 * These are initializations that need to be done on every inode
 * allocation as the fields are not initialised by slab allocation.
 */
int puscon_inode_init_always(puscon_super_block* sb, puscon_inode* inode)
{
	static const puscon_inode_operations empty_iops;
	static const puscon_file_operations no_open_fops = { .open = no_open };

	inode->i_sb = sb;
	inode->i_count = 1;
	inode->i_op = &empty_iops;
	inode->i_fop = &no_open_fops;
	inode->__i_nlink = 1;

	return 0;
}

static puscon_inode* alloc_inode(puscon_super_block* sb) {
	const puscon_super_operations *ops = sb->s_op;
	puscon_inode *inode;

	if (ops->alloc_inode)
		inode = ops->alloc_inode(sb);
	else
		inode = puscon_kmalloc(sizeof(puscon_inode));

	if (!inode)
		return NULL;

	if (puscon_inode_init_always(sb, inode)) {
		destroy_inode(inode);
		return NULL;
	}

	return inode;
}


static void destroy_inode(puscon_inode* inode) {
	const puscon_super_operations *ops = inode->i_sb->s_op;

	if (ops->destroy_inode) {
		ops->destroy_inode(inode);
	}
	if (ops->free_inode) {
		ops->free_inode(inode);
	} else {
		puscon_kfree(inode);
	}
}

/**
 * drop_nlink - directly drop an inode's link count
 * @inode: inode
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.  In cases
 * where we are attempting to track writes to the
 * filesystem, a decrement to zero means an imminent
 * write when the file is truncated and actually unlinked
 * on the filesystem.
 */
void puscon_drop_nlink(puscon_inode* inode) {
	if (inode->i_nlink == 0) {
		puscon_printk(KERN_WARNING "Warning: dropping inode when i_nlink==0.\n");
	}
	inode->__i_nlink--;
}

/**
 * clear_nlink - directly zero an inode's link count
 * @inode: inode
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.  See
 * drop_nlink() for why we care about i_nlink hitting zero.
 */
void puscon_clear_nlink(puscon_inode* inode) {
	if (inode->i_nlink) {
		inode->__i_nlink = 0;
	}
}

/**
 * set_nlink - directly set an inode's link count
 * @inode: inode
 * @nlink: new nlink (should be non-zero)
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.
 */
void puscon_set_nlink(puscon_inode* inode, unsigned int nlink) {
	if (!nlink) {
		puscon_clear_nlink(inode);
	} else {
		inode->__i_nlink = nlink;
	}
}

/**
 * inc_nlink - directly increment an inode's link count
 * @inode: inode
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.  Currently,
 * it is only here for parity with dec_nlink().
 */
void puscon_inc_nlink(puscon_inode* inode) {
	inode->__i_nlink++;
}

/*
 * These are initializations that only need to be done
 * once, because the fields are idempotent across use
 * of the inode, so let the slab aware of that.
 */
void puscon_inode_init_once(puscon_inode* inode)
{
	INIT_LIST_HEAD(&inode->i_sb_list);
	memset(inode, 0, sizeof(*inode));
}

void puscon___iget(puscon_inode* inode)
{
	inode->i_count++;
}

/**
 * inode_sb_list_add - add inode to the superblock list of inodes
 * @inode: inode to add
 */
void puscon_inode_sb_list_add(puscon_inode* inode) {
	list_add(&inode->i_sb_list, &inode->i_sb->s_inodes);
}

static inline void inode_sb_list_del(puscon_inode* inode) {
	if (!list_empty(&inode->i_sb_list)) {
		list_del_init(&inode->i_sb_list);
	}
}


void puscon_clear_inode(puscon_inode* inode) { }

/*
 * Free the inode passed in, removing it from the lists it is still connected
 * to. We remove any pages still attached to the inode and wait for any IO that
 * is still in progress before finally destroying the inode.
 */
static void evict(puscon_inode* inode) {
	const puscon_super_operations *op = inode->i_sb->s_op;

	inode_sb_list_del(inode);

	if (op->evict_inode) {
		op->evict_inode(inode);
	} else {
		puscon_clear_inode(inode);
	}

	destroy_inode(inode);
}


/**
 *	new_inode 	- obtain an inode
 *	@sb: superblock
 */
puscon_inode* puscon_new_inode(puscon_super_block* sb) {
	puscon_inode *inode;

	inode = alloc_inode(sb);
	if (inode)
		puscon_inode_sb_list_add(inode);
	return inode;
}

int puscon_generic_delete_inode(puscon_inode* inode) {
	return 1;
}

/*
 * Called when we're dropping the last reference
 * to an inode.
 *
 * Call the FS "drop_inode()" function, defaulting to
 * the legacy UNIX filesystem behaviour.  If it tells
 * us to evict inode, do so.  Otherwise, retain inode
 * in cache if fs is alive, sync and evict if fs is
 * shutting down.
 */
static void iput_final(puscon_inode* inode) {
	puscon_super_block *sb = inode->i_sb;
	const puscon_super_operations *op = inode->i_sb->s_op;
	int drop;

	if (op->drop_inode)
		drop = op->drop_inode(inode);
	else
		drop = puscon_generic_drop_inode(inode);

	if (!drop) {
		return;
	}

	evict(inode);
}

/**
 *	iput	- put an inode
 *	@inode: inode to put
 *
 *	Puts an inode, dropping its usage count. If the inode use count hits
 *	zero, the inode is then freed and may also be destroyed.
 *
 *	Consequently, iput() can sleep.
 */
void puscon_iput(puscon_inode* inode) {
	if (!inode)
		return;

	if (--inode->i_count == 0) {
		iput_final(inode);
	}	
}
