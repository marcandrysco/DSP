#include "../common.h"
#include "array.h"
#include "lock.h"
#include "sync.h"


/*
 * local function declarations
 */

static void sync_commit(void *arg);


/**
 * Create an empty array.
 *   @nbytes: Width of each data element.
 *   @delete: The deletion callback.
 *   &returns: The array.
 */

_export
struct dsp_array_t dsp_array_empty(unsigned int nbytes)
{
	return (struct dsp_array_t){ dsp_lock_gen(), nbytes, { NULL, NULL }, { 0, 0 } };
}

/**
 * Destroy an array.
 *   @arr: The array.
 */

_export
void dsp_array_destroy(struct dsp_array_t *arr)
{
	mem_delete(arr->data[0]);
	mem_delete(arr->data[1]);
}



_export
void dsp_array_get(struct dsp_array_t *arr, void *restrict ptr, unsigned int idx, uint8_t sel)
{
	mem_copy(ptr, arr->data[sel] + arr->nbytes * idx, arr->nbytes);
}

void *dsp_array_getptr(struct dsp_array_t *arr, unsigned int idx, uint8_t sel)
{
	void *ptr;

	if(idx >= arr->len[sel])
		return NULL;

	dsp_array_get(arr, &ptr, idx, sel);

	return ptr;
}

/**
 * Add to an array.
 *   @arr: The array.
 *   @ptr: The data pointer.
 *   @sync: The synchronization structure.
 */

_export
void dsp_array_add(struct dsp_array_t *arr, const void *restrict ptr, struct dsp_sync_t *sync)
{
	if(sync == NULL) {
		struct dsp_sync_t sync;

		sync = dsp_sync_empty();
		dsp_array_add(arr, ptr, &sync);
		dsp_sync_commit(&sync);
	}
	else {
		unsigned int nbytes = arr->nbytes;

		if(dsp_sync_add(sync, arr, sync_commit))
			dsp_lock_wrlock(&arr->lock);

		if(arr->data[0] == NULL) {
			arr->len[0] = 1;
			arr->data[0] = mem_alloc(nbytes);
		}
		else {
			arr->len[0]++;
			arr->data[0] = mem_realloc(arr->data[0], arr->len[0] * nbytes);
		}

		mem_copy(arr->data[0] + nbytes * (arr->len[0] - 1), ptr, nbytes);
	}
}

/**
 * Add a pointer to the array.
 *   @arr: The array
 *   @ptr: The pointer.
 *   @sync: The synchronization structure.
 */

_export
void dsp_array_addptr(struct dsp_array_t *arr, void *ptr, struct dsp_sync_t *sync)
{
	dsp_array_add(arr, &ptr, sync);
}

/**
 * Remove from an array.
 *   @arr: The array.
 *   @ptr: The data pointer.
 *   @sync: The synchronization structure.
 */

_export
bool dsp_array_rem(struct dsp_array_t *arr, const void *restrict ptr, struct dsp_sync_t *sync)
{
	if(sync == NULL) {
		bool res;
		struct dsp_sync_t sync;

		sync = dsp_sync_empty();
		res = dsp_array_rem(arr, ptr, &sync);
		dsp_sync_commit(&sync);

		return res;
	}
	else {
		unsigned int n, nbytes = arr->nbytes;

		if(dsp_sync_add(sync, arr, sync_commit))
			dsp_lock_wrlock(&arr->lock);

		for(n = 0; n < arr->len[0]; n++) {
			if(mem_isequal(arr->data[0] + nbytes * n, ptr, nbytes))
				break;
		}

		if(n == arr->len[0])
			return false;

		mem_move(arr->data[0] + n * nbytes, arr->data[0] + (n + 1) * nbytes, (arr->len[0] - n - 1) * nbytes);

		arr->len[0]--;

		if(arr->len[0] == 0) {
			mem_free(arr->data[0]);
			arr->data[0] = NULL;
		}
		else
			arr->data[0] = mem_realloc(arr->data[0], arr->len[0] * nbytes);

		return true;
	}
}

/**
 * Remove a pointer from the array.
 *   @arr: The array
 *   @ptr: The pointer.
 *   @sync: The synchronization structure.
 */

_export
void dsp_array_remptr(struct dsp_array_t *arr, void *ptr, struct dsp_sync_t *sync)
{
	dsp_array_rem(arr, &ptr, sync);
}


/**
 * Commit the array for synchronizations.
 *   @arg: The argument.
 */

static void sync_commit(void *arg)
{
	struct dsp_array_t *arr = arg;

	dsp_lock_wrswap(&arr->lock);

	mem_delete(arr->data[1]);

	arr->len[1] = arr->len[0];
	if(arr->data[0] != NULL)
		arr->data[1] = mem_dup(arr->data[0], arr->nbytes * arr->len[1]);
	else
		arr->data[1] = NULL;

	dsp_lock_wrunlock(&arr->lock);
}
