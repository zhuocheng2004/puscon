#ifndef PUSCON_DCACHE_H
#define PUSCON_DCACHE_H

#include <puscon/types.h>

struct puscon_inode;
struct puscon_super_block;


#define IS_ROOT(x) ((x) == (x)->d_parent)

/*
 * "quick string" -- eases parameter passing, but more importantly
 * saves "metadata" about the string (ie length and the hash).
 *
 * hash comes first so it snuggles against d_parent in the
 * dentry.
 */
typedef struct puscon_qstr {
	union {
		struct {
			u32 hash; u32 len;
		};
		u64 hash_len;
	};
	const char *name;
} puscon_qstr;

#define QSTR_INIT(n,l) { { { .len = l } }, .name = n }

extern const puscon_qstr empty_name;
extern const puscon_qstr slash_name;
extern const puscon_qstr dotdot_name;

#define DNAME_INLINE_LEN	40

typedef struct puscon_dentry {
	struct puscon_dentry*	d_parent;	/* parent directory */
	puscon_qstr		d_name;
	struct puscon_inode*	d_inode;	/* Where the name belongs to - NULL is
	                                         * negative */

	char			d_iname[DNAME_INLINE_LEN];	/* small names */

	const struct puscon_dentry_operations*	d_op;
	struct puscon_super_block*	d_sb;	/* The root of the dentry tree */

	hlist_node		d_sib;		/* child of parent list */
	hlist_head		d_children;	/* out children */

	int			d_ref;
} puscon_dentry;

typedef struct puscon_dentry_operations {
	int		(*d_compare) (const puscon_dentry*, unsigned int, const char*, puscon_qstr*);
	int		(*d_delete) (const puscon_dentry*);
	int		(*d_init) (puscon_dentry*);
	void		(*d_release) (puscon_dentry*);
	void		(*d_iput) (puscon_dentry*, struct puscon_inode*);
	void		*(*d_dname) (puscon_dentry*, char*, int);
} puscon_dentry_operations;

/*
 * These are the low-level FS interfaces to the dcache..
 */
void puscon_d_instantiate(puscon_dentry*, struct puscon_inode*);
void puscon_d_instantiate_new(puscon_dentry*, struct puscon_inode*);
void puscon___d_drop(puscon_dentry* dentry);
void puscon_d_drop(puscon_dentry* dentry);
void puscon_d_delete(puscon_dentry* dentry);
void puscon_d_set_d_op(puscon_dentry* dentry, const puscon_dentry_operations* op);

/* allocate/de-allocate */
puscon_dentry* puscon_d_alloc(puscon_dentry*, const puscon_qstr*);
puscon_dentry* puscon_d_alloc_anon(struct puscon_super_block*);


/* only used at mount-time */
puscon_dentry* puscon_d_make_root(struct puscon_inode*);


/**
 * dget - get a reference to a dentry
 * @dentry: dentry to get a reference to
 */
static inline puscon_dentry* puscon_dget(puscon_dentry* dentry) {
	if (dentry)
		dentry->d_ref++;
	return dentry;
}

puscon_dentry* puscon_dget_parent(puscon_dentry* dentry);

void puscon_dput(puscon_dentry*);

#endif
