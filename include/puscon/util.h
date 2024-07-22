#ifndef PUSCON_UTIL_H
#define PUSCON_UTIL_H

#include <puscon/types.h>

static const u8 MAX_CAPACITY_SHIFT = 30;

typedef struct puscon_idmap {
	/* id will be in [0, 2^capacity_shift-1] */
	u8		capacity_shift;
	u32		capacity;

	u32		pos;

	/* each entry marks whether the id is free */
	u8*		free_map;

	/* array of pointers to objects */
	void**		ptrs;
} puscon_idmap;

int puscon_idmap_init(puscon_idmap* idmap, u8 capacity_shift);
void puscon_idmap_destroy(puscon_idmap *idmap);
s32 puscon_idmap_alloc(puscon_idmap* idmap);
int puscon_idmap_free(puscon_idmap* idmap, u32 id);

#endif
