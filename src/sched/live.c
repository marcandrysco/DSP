#include "../common.h"
#include "live.h"



/*
 * local function declarations
 */

static struct dsp_sched_inst_t *inst_get(struct dsp_sched_live_t *live, unsigned int idx);


/**
 * Create a new live scheduler.
 *   @len: The list length.
 *   @size: The element size.
 */

_export
struct dsp_sched_live_t *dsp_sched_live_new(unsigned int len, unsigned int size)
{
	unsigned int i;
	struct dsp_sched_live_t *live;
	struct dsp_sched_inst_t *inst;

	live = mem_alloc(sizeof(struct dsp_sched_live_t) + len * (size + sizeof(struct dsp_sched_inst_t)));
	live->len = len;
	live->size = size;
	live->rd = live->wr = 0;

	for(i = 0; i < len; i++) {
		inst = inst_get(live, i);
		inst->active = 0;
		inst->lock = 0;
	}

	return live;
}

/**
 * Delete a live scheduler.
 *   @live: The live scheudler.
 */

_export
void dsp_sched_live_delete(struct dsp_sched_live_t *live)
{
	mem_free(live);
}


/**
 * Add data to a live scheduler.
 *   @live: The live scheduler.
 *   @data: The data.
 *   &returns: True if added, false if the current element is locked.
 */

_export
bool dsp_sched_live_add(struct dsp_sched_live_t *live, const void *data)
{
	struct dsp_sched_inst_t *inst;

	inst = inst_get(live, live->wr);
	if(!__sync_bool_compare_and_swap(&inst->lock, 0, 1))
		return false;

	inst->active = 1;
	mem_copy(inst->data, data, live->size);

	live->wr = (live->wr + 1) % live->len;

	__sync_synchronize();
	inst->lock = 0;

	return true;
}

/**
 * Remove data from a live scheduler.
 *   @live: The live scheduler.
 *   @data: The data.
 *   &returns: True if removed, false if the current element is locked or no
 *     available elements.
 */

_export
bool dsp_sched_live_remove(struct dsp_sched_live_t *live, void *data)
{
	uint8_t active;
	struct dsp_sched_inst_t *inst;

	inst = inst_get(live, live->rd);
	if(!__sync_bool_compare_and_swap(&inst->lock, 0, 1))
		return false;

	active = inst->active;
	if(inst->active) {
		inst->active = 0;
		mem_copy(data, inst->data, live->size);

		live->rd = (live->rd + 1) % live->len;
	}

	__sync_synchronize();
	inst->lock = 0;

	return active;
}


/**
 * Obtain an instance by index.
 *   @live: The live scheduler.
 *   @idx: The index.
 *   &returns: The instance.
 */

static struct dsp_sched_inst_t *inst_get(struct dsp_sched_live_t *live, unsigned int idx)
{
	return (struct dsp_sched_inst_t *)(live->data + idx * (live->size + sizeof(struct dsp_sched_inst_t)));
}
