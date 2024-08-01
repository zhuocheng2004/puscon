
#include <string.h>

#include <puscon/err.h>
#include <puscon/fs.h>
#include <puscon/namei.h>
#include <puscon/uaccess.h>

#include "mount.h"


#define EMBEDDED_NAME_MAX		(PATH_MAX - offsetof(puscon_filename, iname))


puscon_filename* puscon_getname_flags(puscon_context* context, const char __user* filename, int flags) {
	puscon_filename *result;
	char *kname;
	int len;

	result = __getname();
	if (!result)
		return ERR_PTR(-ENOMEM);

	/*
	 * First, try to embed the struct filename inside the allocated struct
	 */
	kname = (char*) result->iname;
	result->name = kname;

	len = puscon_copy_from_user(context, kname, filename, EMBEDDED_NAME_MAX);

	/*
	 * Handle both empty path and copy failure in one go.
	 */
	if (len <= 0) {
		if (len < 0) {
			__putname(result);
			return ERR_PTR(len);
		}
		/* The empty path is special. */
		if (!(flags & LOOKUP_EMPTY)) {
			__putname(result);
			return ERR_PTR(-ENOENT);
		}
	}

	/*
	 * Uh-oh. We have a name that's approaching PATH_MAX. Allocate a
	 * separate struct filename, and re-do the copy from
	 * userland.
	 */
	if (len == EMBEDDED_NAME_MAX) {
		const size_t size = offsetof(puscon_filename, iname[1]);
		kname = (char*) result;

		result = puscon_kzalloc(size);
		if (!result) {
			__putname(kname);
			return ERR_PTR(-ENOMEM);
		}
		result->name = kname;
		len = puscon_copy_from_user(context, kname, filename, PATH_MAX);
		if (len < 0) {
			__putname(kname);
			puscon_kfree(result);
			return ERR_PTR(len);
		}
		/* The empty path is special. */
		if (!len && !(flags & LOOKUP_EMPTY)) {
			__putname(kname);
			puscon_kfree(result);
			return ERR_PTR(-ENOENT);
		}
		if (len == PATH_MAX) {
			__putname(kname);
			puscon_kfree(result);
			return ERR_PTR(-ENAMETOOLONG);
		}
	}

	result->refcnt = 1;
	result->uptr = filename;
	return result;
}

puscon_filename* puscon_getname(puscon_context* context, const char __user* filename) {
	return puscon_getname_flags(context, filename, 0);
}

puscon_filename* puscon_getname_kernel(const char* filename) {
	puscon_filename *result;
	int len = strlen(filename) + 1;

	result = __getname();
	if (!result)
		return ERR_PTR(-ENOMEM);

	if (len <= EMBEDDED_NAME_MAX) {
		result->name = (char*) result->iname;
	} else if (len <= PATH_MAX) {
		const size_t size = offsetof(puscon_filename, iname[1]);
		puscon_filename *tmp;

		tmp = puscon_kmalloc(size);
		if (!tmp) {
			__putname(result);
			return ERR_PTR(-ENOMEM);
		}
		tmp->name = (char*) result;
		result = tmp;
	} else {
		__putname(result);
		return ERR_PTR(-ENAMETOOLONG);
	}
	memcpy((char*) result->name, filename, len);
	result->uptr = NULL;
	result->refcnt = 1;

	return result;
}

void puscon_putname(puscon_filename* name) {
	if (IS_ERR(name))
		return;

	if (!name->refcnt) {
		puscon_printk(KERN_WARNING "Warning: putname() called on empty refcnt.\n");
		return;
	}

	if (--name->refcnt)
		return;

	if (name->name != name->iname) {
		__putname(name->name);
		puscon_kfree(name);
	} else {
		__putname(name);
	}
}


static puscon_dentry* filename_create(int dfd, puscon_filename* name, puscon_path* path, unsigned int lookup_flags) {
	puscon_dentry *dentry = ERR_PTR(-EEXIST);
	puscon_qstr last;
	int type;
	int error;

	// TODO
	return NULL;
}


/**
 * vfs_mkdir - create directory
 * @dir:	inode of the parent directory
 * @dentry:	dentry of the child directory
 * @mode:	mode of the child directory
 * 
 * Create a directory.
 */
int puscon_vfs_mkdir(puscon_inode* dir, puscon_dentry* dentry, mode_t mode) {
	int error;

	if (!dir->i_op->mkdir)
		return -EPERM;

	error = dir->i_op->mkdir(dir, dentry, mode);

	return error;
}

int puscon_do_mkdirat(int dfd, puscon_filename* name, mode_t mode) {
	return 0;
}


long puscon_sys_mkdir(const char* pathname, mode_t mode) {
	return 0;
}
