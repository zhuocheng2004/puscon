
#include <assert.h>
#include <stdio.h>

#include <puscon/util.h>

int test_simple() {
	puscon_idmap idmap;

	if (puscon_idmap_init(&idmap, 5)) {
		fprintf(stderr, "Failed to init idmap.");
		return 1;
	}

	assert(idmap.capacity_shift == 5);
	assert(idmap.capacity == 32);
	assert(puscon_idmap_occupied(&idmap) == 0);
;
	assert(puscon_idmap_alloc(&idmap) == 0);
	assert(puscon_idmap_alloc(&idmap) == 1);
	assert(puscon_idmap_alloc(&idmap) == 2);

	assert(puscon_idmap_occupied(&idmap) == 3);

	assert(puscon_idmap_free(&idmap, 1) == 0);
	assert(puscon_idmap_alloc(&idmap) == 3);

	assert(puscon_idmap_free(&idmap, 0) == 0);
	//assert(puscon_idmap_free(&idmap, 1) == 0);
	assert(puscon_idmap_free(&idmap, 2) == 0);
	assert(puscon_idmap_free(&idmap, 3) == 0);
	
	assert(puscon_idmap_occupied(&idmap) == 0);

	puscon_idmap_destroy(&idmap);

	return 0;
}

int test_wrap() {
	puscon_idmap idmap;

	if (puscon_idmap_init(&idmap, 5)) {
		fprintf(stderr, "Failed to init idmap.");
		return 1;
	}

	for (int i = 0; i < 32; i++) {
		assert(puscon_idmap_alloc(&idmap) == i);
	}

	/* should have no free ids */
	assert(puscon_idmap_alloc(&idmap) < 0);

	assert(puscon_idmap_free(&idmap, 2) == 0);
	assert(puscon_idmap_free(&idmap, 3) == 0);
	assert(puscon_idmap_free(&idmap, 5) == 0);
	assert(puscon_idmap_free(&idmap, 7) == 0);

	assert(puscon_idmap_alloc(&idmap) == 2);
	assert(puscon_idmap_alloc(&idmap) == 3);
	assert(puscon_idmap_alloc(&idmap) == 5);
	assert(puscon_idmap_alloc(&idmap) == 7);
	assert(puscon_idmap_alloc(&idmap) < 0);

	puscon_idmap_destroy(&idmap);

	return 0;
}

int main() {
	int err;

	err = test_simple();
	if (err)
		return err;
	
	err = test_wrap();
	if (err)
		return err;

	return 0;
}
