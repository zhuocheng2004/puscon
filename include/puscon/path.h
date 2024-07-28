#ifndef PUSCON_PATH_H
#define PUSCON_PATH_H

struct puscon_dentry;
struct puscon_vfsmount;

typedef struct puscon_path {
	struct puscon_vfsmount*	mnt;
	struct puscon_dentry*	dentry;
} puscon_path;

void puscon_path_get(const puscon_path*);
void puscon_path_put(const puscon_path*);

static inline int puscon_path_equal(const puscon_path* path1, const puscon_path* path2) {
	return path1->mnt == path2->mnt && path1->dentry == path2->dentry;
}

static inline void puscon_path_put_init(puscon_path* path) {
	puscon_path_put(path);
	*path = (puscon_path) { };
}

#endif
