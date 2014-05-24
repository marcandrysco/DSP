#ifndef SYNC_H
#define SYNC_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Synchronization callback.
 *   @ref: The reference.
 */

typedef void (*dsp_sync_f)(void *ref);

/**
 * Synchronization structure.
 *   @tree: The internal tree.
 */

struct dsp_sync_t {
	struct avltree_t tree;
};


/*
 * synchronization function declarations
 */

struct dsp_sync_t dsp_sync_empty();
void dsp_sync_commit(struct dsp_sync_t *sync);
void dsp_sync_discard(struct dsp_sync_t *sync);

bool dsp_sync_add(struct dsp_sync_t *sync, void *ref, dsp_sync_f func);
void dsp_sync_remove(struct dsp_sync_t *sync, void *ref);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
