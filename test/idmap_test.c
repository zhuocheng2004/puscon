
#include <assert.h>
#include <stdio.h>

#include <puscon/util.h>

int test_simple() {
	puscon_idmap idmap;
	int err, id;

	err = puscon_idmap_init(&idmap, 5);
	if (err) {
		puts("Failed to init idmap.");
		return err;
	}

	assert(idmap.capacity_shift == 5);
	assert(idmap.capacity == 32);

	id = puscon_idmap_alloc(&idmap);
	assert(id == 0);
	id = puscon_idmap_alloc(&idmap);
	assert(id == 1);
	id = puscon_idmap_alloc(&idmap);
	assert(id == 2);

	err = puscon_idmap_free(&idmap, 1);
	assert(err == 0);
	id = puscon_idmap_alloc(&idmap);
	assert(id == 3);

	puscon_idmap_destroy(&idmap);

	return 0;
}

int test_wrap() {
	puscon_idmap idmap;
	int err, id;

	err = puscon_idmap_init(&idmap, 5);
	if (err) {
		puts("Failed to init idmap.");
		return err;
	}

	for (int i = 0; i < 32; i++) {
		id = puscon_idmap_alloc(&idmap);
		assert(id == i);
	}

	/* should have no free ids */
	id = puscon_idmap_alloc(&idmap);
	assert(id < 0);

	err = puscon_idmap_free(&idmap, 2);
	assert(err == 0);
	err = puscon_idmap_free(&idmap, 3);
	assert(err == 0);
	err = puscon_idmap_free(&idmap, 5);
	assert(err == 0);
	err = puscon_idmap_free(&idmap, 7);
	assert(err == 0);

	id = puscon_idmap_alloc(&idmap);
	assert(id == 2);
	id = puscon_idmap_alloc(&idmap);
	assert(id == 3);
	id = puscon_idmap_alloc(&idmap);
	assert(id == 5);
	id = puscon_idmap_alloc(&idmap);
	assert(id == 7);
	id = puscon_idmap_alloc(&idmap);
	assert(id < 0);

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
