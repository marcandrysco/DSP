#include "../common.h"
#include "ring.h"


/*
 * local function declarations
 */

static void refresh(struct dsp_sched_ring_t *sched);


/**
 * Create a ring scheduler.
 *   @len: The length.
 *   @size: The size.
 *   @init: The initializer.
 *   &returns: The scheduler.
 */

_export
struct dsp_sched_ring_t *dsp_sched_ring_new(init_f init, unsigned int len, unsigned int size)
{
	struct dsp_sched_ring_t *sched;

	sched = mem_alloc(sizeof(struct dsp_sched_ring_t) + len * size);
	sched->init = init;
	sched->lock = thread_mutex_new(NULL);
	sched->idx = 0;
	sched->len = len;
	sched->size = size;

	refresh(sched);

	return sched;
}

/**
 * Delete a ring scheduler.
 *   @sched: The scheduler.
 */

_export
void dsp_sched_ring_delete(struct dsp_sched_ring_t *sched)
{
	mem_free(sched);
}


/**
 * Resize the ring scheduler.
 *   @sched: The scheduler reference.
 *   @size: The new size.
 */

_export
void dsp_sched_ring_resize(struct dsp_sched_ring_t **sched, unsigned int size)
{
	thread_mutex_lock(&(*sched)->lock);

	*sched = mem_realloc(*sched, sizeof(struct dsp_sched_ring_t) + (*sched)->len * size);
	(*sched)->size = size;

	refresh(*sched);

	thread_mutex_unlock(&(*sched)->lock);
}

/**
 * Resize the length of the ring scheduler.
 *   @sched: The scheduler reference.
 *   @len: The new length.
 */

_export
void dsp_sched_ring_relen(struct dsp_sched_ring_t **sched, unsigned int len)
{
	thread_mutex_lock(&(*sched)->lock);

	*sched = mem_realloc(*sched, sizeof(struct dsp_sched_ring_t) + len * (*sched)->size);
	(*sched)->len = len;

	refresh(*sched);

	thread_mutex_unlock(&(*sched)->lock);
}


/**
 * Refresh the scheduler, initializing all reference.
 *   @sched: The scheduler.
 */

static void refresh(struct dsp_sched_ring_t *sched)
{
	unsigned int i;

	for(i = 0; i < sched->idx; i++)
		sched->init(dsp_sched_ring_idx(sched, i));
}
