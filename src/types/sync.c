#include "../common.h"
#include "sync.h"


/**
 * Instance structure.
 *   @ref: The reference.
 *   @func: The callback function.
 */

struct inst_t {
	void *ref;
	dsp_sync_f func;
};


/**
 * Initialize a synchronization structure.
 *   &returns: The synchronization structure.
 */

_export
struct dsp_sync_t dsp_sync_empty()
{
	return (struct dsp_sync_t){ avltree_empty(compare_ptr, mem_free) };
}

/**
 * Commit a synchronization structure, executing all callbacks.
 *   @sync: The synchronization structure.
 */

_export
void dsp_sync_commit(struct dsp_sync_t *sync)
{
	struct inst_t *inst;
	struct avltree_iter_t iter;

	avltree_iter_init(&iter, &sync->tree);
	while((inst = avltree_iter_next(&iter)) != NULL)
		inst->func(inst->ref);

	dsp_sync_discard(sync);
}

/**
 * Discard a synchronization structure without executing callbacks.
 *   @sync: The synchronization structure.
 */

_export
void dsp_sync_discard(struct dsp_sync_t *sync)
{
	avltree_destroy(&sync->tree);
}


/**
 * Add a callback to the synchronization structure.
 *   @sync: The synchronization structure.
 *   @ref: The reference.
 *   @func: The callback.
 *   &returns: True if added, false if already exists.
 */

_export
bool dsp_sync_add(struct dsp_sync_t *sync, void *ref, dsp_sync_f func)
{
	struct inst_t *inst;

	if(avltree_lookup(&sync->tree, ref) != NULL)
		return false;

	inst = mem_alloc(sizeof(struct inst_t));
	inst->ref = ref;
	inst->func = func;

	avltree_insert(&sync->tree, ref, inst);

	return true;
}

/**
 * Remove a callback from the synchronization structure.
 *   @sync: The synchronization structure.
 *   @ref: The reference.
 */

_export
void dsp_sync_remove(struct dsp_sync_t *sync, void *ref)
{
	avltree_purge(&sync->tree, ref);
}
