
#include <stdlib.h>
#include <string.h>

#include <puscon/err.h>
#include <puscon/fs.h>
#include <puscon/fs_context.h>


/* Free a superblock that has never been seen by anyone */
static void destroy_unused_super(puscon_super_block* s) {
	if (!s)
		return;
	free(s);
}

/**
 *	alloc_super	-	create new superblock
 *	@type:	filesystem type superblock should belong to
 *
 *	Allocates and initializes a new &struct super_block.  alloc_super()
 *	returns a pointer new superblock or %NULL if allocation had failed.
 */
static puscon_super_block* alloc_super(puscon_file_system_type* type) {
	puscon_super_block *s = calloc(1, sizeof(puscon_super_block));
	static const puscon_super_operations default_op;

	if (!s)
		return NULL;

	INIT_LIST_HEAD(&s->s_mounts);

	INIT_HLIST_NODE(&s->s_instances);
	INIT_LIST_HEAD(&s->s_inodes);

	s->s_count = 1;
	s->s_op = &default_op;

	return s;
}

/**
 * sget_fc - Find or create a superblock
 * @fc:	Filesystem context.
 * @test: Comparison callback
 * @set: Setup callback
 *
 * Create a new superblock or find an existing one.
 *
 * The @test callback is used to find a matching existing superblock.
 * Whether or not the requested parameters in @fc are taken into account
 * is specific to the @test callback that is used. They may even be
 * completely ignored.
 *
 * Return: On success, an extant or newly created superblock is
 *         returned. On failure an error pointer is returned.
 */
puscon_super_block* puscon_sget_fc(struct puscon_fs_context* fc,
	int (*test) (puscon_super_block*, struct puscon_fs_context*),
	int (*set) (puscon_super_block*, struct puscon_fs_context*)) {
	puscon_super_block *s = NULL;
	puscon_super_block *old;
	int err;

	if (test) {
		hlist_for_each_entry(old, &fc->fs_type->fs_supers, s_instances) {
			if (test(old, fc))
				return ERR_PTR(-EBUSY);
		}
	}

	s = alloc_super(fc->fs_type);

	err = set(s, fc);
	if (err) {
		destroy_unused_super(s);
		return ERR_PTR(err);
	}
	s->s_type = fc->fs_type;
	strncpy(s->s_id, s->s_type->name, sizeof(s->s_id));
	/*
	 * Make the superblock visible on @super_blocks and @fs_supers.
	 */
	list_add_tail(&s->s_list, &fc->context->super_blocks);
	hlist_add_head(&s->s_instances, &s->s_type->fs_supers);
	puscon_get_filesystem(fc->context, s->s_type);
	return s;
}

int puscon_set_anon_super(puscon_super_block* sb, void* data) {
	return 0; // TODO
}

void puscon_kill_litter_super(puscon_super_block* sb) {
	; // TODO
}

int puscon_set_anon_super_fc(puscon_super_block* sb, puscon_fs_context* fc) {
	return puscon_set_anon_super(sb, NULL);
}

static int vfs_get_super(puscon_fs_context* fc,
	int (*test) (puscon_super_block*, puscon_fs_context*),
	int (*fill_super) (puscon_super_block* sb, puscon_fs_context* fc)) {
	puscon_super_block *sb;
	int err;

	sb = puscon_sget_fc(fc, test, puscon_set_anon_super_fc);
	if (IS_ERR(sb))
		return PTR_ERR(sb);

	if (!sb->s_root) {
		err = fill_super(sb, fc);
		if (err)
			goto error;
	}

	fc->root = puscon_dget(sb->s_root);
	return 0;

error:
	return err;
}

int puscon_get_tree_nodev(puscon_fs_context* fc,
	int (*fill_super) (struct puscon_super_block* sb, puscon_fs_context* fc)) {
		return vfs_get_super(fc, NULL, fill_super);
}


/**
 * vfs_get_tree - Get the mountable root
 * @fc: The superblock configuration context.
 *
 * The filesystem is invoked to get or create a superblock which can then later
 * be used for mounting.  The filesystem places a pointer to the root to be
 * used for mounting in @fc->root.
 */
int puscon_vfs_get_tree(puscon_fs_context* fc) {
	puscon_super_block *sb;
	int error;

	if (fc->root)
		return -EBUSY;

	/* Get the mountable root in fc->root, with a ref on the root and a ref
	 * on the superblock.
	 */
	error = fc->ops->get_tree(fc);
	if (error < 0)
		return error;

	if (!fc->root) {
		puscon_printk(KERN_ERR "Filesystem %s get_tree() didn't set fc->root\n", 
			fc->fs_type->name);
		return -ENODEV;
	}

	sb = fc->root->d_sb;

	return 0;
}
