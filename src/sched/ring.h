#ifndef SCHED_RING_H
#define SCHED_RING_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Ring scheduler.
 *   @init: The initializer.
 *   @lock: The lock.
 *   @idx, len, size: The index, length, and size.
 *   @data: The data array.
 */

struct dsp_sched_ring_t {
	init_f init;
	struct thread_mutex_t lock;

	unsigned int idx, len, size;
	uint8_t data[];
};


/*
 * ring scheduler function declarations
 */

struct dsp_sched_ring_t *dsp_sched_ring_new(init_f init, unsigned int len, unsigned int size);
void dsp_sched_ring_delete(struct dsp_sched_ring_t *sched);

void dsp_sched_ring_resize(struct dsp_sched_ring_t **sched, unsigned int size);
void dsp_sched_ring_relen(struct dsp_sched_ring_t **sched, unsigned int len);


/**
 * Retrieve a reference from the scheduler.
 *   @sched: The scheduler.
 *   @idx: The index.
 *   &returns: The reference.
 */

static inline void *dsp_sched_ring_idx(struct dsp_sched_ring_t *sched, unsigned int idx)
{
	return sched->data + idx * sched->size;
}

/**
 * Retrieve the as an offset from the most recent reference.
 *   @sched: The scheduler.
 *   @idx: The index.
 */

static inline void *dsp_sched_ring_get(struct dsp_sched_ring_t *sched, unsigned int idx)
{
	return dsp_sched_ring_idx(sched, dsp_mod(sched->idx - idx, sched->len));
}

/**
 * Increment the ring scheduler.
 *   @sched: The scheduler.
 */

static inline void *dsp_sched_ring_inc(struct dsp_sched_ring_t *sched)
{
	sched->idx = (sched->idx + 1) % sched->len;

	return dsp_sched_ring_idx(sched, sched->idx);
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
