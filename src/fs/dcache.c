
#include <stdlib.h>
#include <string.h>

#include <puscon/dcache.h>
#include <puscon/fs.h>
#include <puscon/puscon.h>

const puscon_qstr empty_name = QSTR_INIT("", 0);
const puscon_qstr slash_name = QSTR_INIT("/", 1);
const puscon_qstr dotdot_name = QSTR_INIT("..", 2);


static inline int dname_external(const puscon_dentry* dentry) {
	return dentry->d_name.name != dentry->d_iname;
}

static inline void __d_set_inode_and_type(puscon_dentry* dentry, puscon_inode* inode) {
	dentry->d_inode = inode;
}

static void dentry_free(puscon_dentry* dentry) {
	free(dentry);
}

/*
 * Release the dentry's inode, using the filesystem
 * d_iput() operation if defined.
 */
static void dentry_unlink_inode(puscon_dentry* dentry) {
	puscon_inode *inode = dentry->d_inode;
	if (dentry->d_op && dentry->d_op->d_iput)
		dentry->d_op->d_iput(dentry, inode);
	else
		puscon_iput(inode);
}

static void ___d_drop(puscon_dentry* dentry) { }

void puscon___d_drop(puscon_dentry* dentry) { }

void puscon_d_drop(puscon_dentry* dentry) {
	puscon___d_drop(dentry);
}

static puscon_dentry* __dentry_kill(puscon_dentry* dentry) {
	puscon_dentry *parent = NULL;
	bool can_free = true;

	/* if it was on the hash then remove it */
	puscon___d_drop(dentry);
	if (dentry->d_inode)
		dentry_unlink_inode(dentry);

	if (dentry->d_op && dentry->d_op->d_release)
		dentry->d_op->d_release(dentry);

	if (!IS_ROOT(dentry)) {
		parent = dentry->d_parent;
	}

	if (can_free)
		dentry_free(dentry);

	if (parent && --parent->d_ref) {
		return NULL;
	}

	return parent;
}

/*
 * dput - release a dentry
 * @dentry: dentry to release 
 *
 * Release a dentry. This will drop the usage count and if appropriate
 * call the dentry unlink method as well as removing it from the queues and
 * releasing its resources. If the parent dentries were scheduled for release
 * they too may now get deleted.
 */
void puscon_dput(puscon_dentry* dentry) {
	if (!dentry)
		return;

	/*
	 * If we weren't the last ref, we're done.
	 */
	if (--dentry->d_ref != 0)
		return;

	dentry = __dentry_kill(dentry);
	if (!dentry)
		return;
}

puscon_dentry* puscon_dget_parent(puscon_dentry* dentry) {
	puscon_dentry *ret;

	ret = dentry->d_parent;

	if (!ret->d_ref) {
		puscon_printk(KERN_ERR "BUG: !ret->d_ref\n");
	}
	ret->d_ref++;
	return ret;
}

/**
 * __d_alloc	-	allocate a dcache entry
 * @sb: filesystem it will belong to
 * @name: qstr of the name
 *
 * Allocates a dentry. It returns %NULL if there is insufficient memory
 * available. On a success the dentry is returned. The name passed in is
 * copied and the copy passed in may be reused after this call.
 */
static puscon_dentry* __d_alloc(puscon_super_block* sb, const puscon_qstr* name) {
	puscon_dentry *dentry;
	char *dname;
	int err;

	dentry = malloc(sizeof(puscon_dentry));

	if (!dentry)
		return NULL;

	if (!name) {
		name = &slash_name;
		dname = dentry->d_iname;
	} else if (name->len > DNAME_INLINE_LEN-1) {
		dname = malloc(name->len);
		if (!dname) {
			free(dentry);
			return NULL;
		}
	} else {
		dname = dentry->d_iname;
	}

	dentry->d_name.len = name->len;
	dentry->d_name.hash = name->hash;
	memcpy(dname, name->name, name->len);
	dname[name->len] = 0;

	dentry->d_name.name = dname;

	dentry->d_ref = 1;
	dentry->d_inode = NULL;
	dentry->d_parent = dentry;
	dentry->d_sb = sb;
	dentry->d_op = NULL;
	INIT_HLIST_HEAD(&dentry->d_children);
	INIT_HLIST_NODE(&dentry->d_sib);
	puscon_d_set_d_op(dentry, dentry->d_sb->s_d_op);

	if (dentry->d_op && dentry->d_op->d_init) {
		err = dentry->d_op->d_init(dentry);
		if (err) {
			if (dname_external(dentry))
				free((void*) dentry->d_name.name);
			free(dentry);
			return NULL;
		}
	}

	return dentry;
}

/**
 * d_alloc	-	allocate a dcache entry
 * @parent: parent of entry to allocate
 * @name: qstr of the name
 *
 * Allocates a dentry. It returns %NULL if there is insufficient memory
 * available. On a success the dentry is returned. The name passed in is
 * copied and the copy passed in may be reused after this call.
 */
puscon_dentry* puscon_d_alloc(puscon_dentry* parent, const puscon_qstr* name) {
	puscon_dentry *dentry = __d_alloc(parent->d_sb, name);
	if (!dentry)
		return NULL;

	dentry->d_parent = puscon_dget(parent);
	hlist_add_head(&dentry->d_sib, &parent->d_children);

	return dentry;
}

puscon_dentry* puscon_d_alloc_anon(puscon_super_block* sb) {
	return __d_alloc(sb, NULL);
}

puscon_dentry* puscon_d_alloc_name(puscon_dentry* parent, const char* name) {
	puscon_qstr q;

	q.name = name;
	q.len = strlen(name);
	return puscon_d_alloc(parent, &q);
}

void puscon_d_set_d_op(puscon_dentry* dentry, const puscon_dentry_operations* op) {
	if (dentry->d_op) {
		puscon_printk(KERN_WARNING "Warning: dentry->d_op alreay set.\n");
	}

	dentry->d_op = op;
}

static void __d_instantiate(puscon_dentry* dentry, puscon_inode* inode) {
	__d_set_inode_and_type(dentry, inode);
}

/**
 * d_instantiate - fill in inode information for a dentry
 * @entry: dentry to complete
 * @inode: inode to attach to this dentry
 *
 * Fill in inode information in the entry.
 *
 * This turns negative dentries into productive full members
 * of society.
 *
 * NOTE! This assumes that the inode count has been incremented
 * (or otherwise set) by the caller to indicate that it is now
 * in use by the dcache.
 */
void puscon_d_instantiate(puscon_dentry* dentry, puscon_inode* inode) {
	if (inode) {
		__d_instantiate(dentry, inode);
	}
}

puscon_dentry* puscon_d_make_root(puscon_inode* root_inode) {
	puscon_dentry *res = NULL;

	if (root_inode) {
		res = puscon_d_alloc_anon(root_inode->i_sb);
		if (res)
			puscon_d_instantiate(res, root_inode);
		else
			puscon_iput(root_inode);
	}

	return res;
}
