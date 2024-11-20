
#include <stdio.h>
#include <stdlib.h>

#include <puscon/puscon.h>
#include <puscon/util.h>

int puscon_idmap_init(puscon_idmap* idmap, u8 capacity_shift) {
	if (!idmap) {
		puscon_log(LOG_ERR "Error: idmap is NULL\n");
		return 1;
	}

	if (capacity_shift > MAX_CAPACITY_SHIFT) {
		puscon_log(LOG_ERR "Error: idmap initialized with too large capacity_shift %d (MAX: %d)\n", capacity_shift, MAX_CAPACITY_SHIFT);
		return 1;
	}
	idmap->capacity_shift = capacity_shift;
	int capacity = 1 << capacity_shift;
	idmap->capacity = capacity;

	idmap->free_map = calloc(capacity, 1);
	if (!idmap->free_map) {
		puscon_log(LOG_ERR "Error: failed to allocate idmap.free_map (capacity_shift=%d)\n", capacity_shift);
		return 1;
	}
	
	idmap->ptrs = calloc(capacity, sizeof(void*));
	if (!idmap->ptrs) {
		puscon_log(LOG_ERR "Error: failed to allocate idmap.ptrs (capacity_shift=%d)\n", capacity_shift);
		return 1;
	}

	idmap->pos = 0;

	return 0;
}

void puscon_idmap_destroy(puscon_idmap* idmap) {
	if (idmap->free_map)
		free(idmap->free_map);
	if (idmap->ptrs)
		free(idmap->ptrs);
}

s32 puscon_idmap_alloc(puscon_idmap* idmap) {
	if (!idmap || !idmap->free_map) {
		puscon_log(LOG_ERR "Error: bad idmap\n");
		return -1;
	}

	u32 capacity = idmap->capacity;
	u32 mask = capacity - 1;
	u8* free_map = idmap->free_map;
	u32 pos = idmap->pos;

	for (int i = 0; i < capacity; i++, pos++) {
		if (pos >= capacity)
			pos &= mask;
		if (!free_map[pos]) {
			/* found free id */
			idmap->free_map[pos] = 1;
			idmap->pos = pos + 1;
			return pos;
		}
	}

	/* no free id */
	return -1;
}

int puscon_idmap_free(puscon_idmap* idmap, u32 id) {
	if (!idmap || !idmap->free_map) {
		puscon_log(LOG_ERR "Error: bad idmap\n");
		return -1;
	}

	if (id >= idmap->capacity) {
		puscon_log(LOG_ERR "Error: id >= capacity [%u]\n", idmap->capacity);
		return -1;
	}

	if (!idmap->free_map[id]) {
		puscon_log(LOG_WARNING "Warning: freeing unused id %u\n", id);
	}

	idmap->free_map[id] = 0;

	return 0;
}

int puscon_idmap_occupied(puscon_idmap* idmap) {
	if (!idmap || !idmap->free_map) {
		puscon_log(LOG_ERR "Error: bad idmap\n");
		return -1;
	}

	u32 capacity = idmap->capacity;
	u8* free_map = idmap->free_map;

	s32 cnt = 0;
	for (int i = 0; i < capacity; i++) {
		if (free_map[i])
			cnt++;
	}

	return cnt;
}
