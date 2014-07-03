#ifndef REVERB_DEFS_H
#define REVERB_DEFS_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Ring buffer struture.
 *   @idx: The current index.
 *   @len: The length.
 *   @arr: The data array.
 */

struct dsp_ring_t {
	unsigned int idx, len;

	double arr[];
};


/**
 * Retrieve a value from the ring buffer. Typically, you want to use a
 * negative index to look backward into the buffer.
 *   @ring: The ring buffer.
 *   @idx: The index.
 *   &returns: The value.
 */

static inline double dsp_ring_get(struct dsp_ring_t *ring, int idx)
{
	return ring->arr[dsp_mod(ring->idx + idx, ring->len)];
}

/**
 * Retrieve the last value from the ring buffer.
 *   @ring: The ring buffer.
 *   &returns: The value.
 */

static inline double dsp_ring_last(struct dsp_ring_t *ring)
{
	return dsp_ring_get(ring, 0);
}

/**
 * Write a value to the ring buffer.
 *   @ring: The ring buffer.
 *   @val: The value.
 */

static inline void dsp_ring_put(struct dsp_ring_t *ring, double val)
{
	ring->arr[ring->idx] = val;
	ring->idx = dsp_mod(ring->idx + 1, ring->len);
}

/**
 * Process a single data point, retrieving the last output and providing the
 * most recent input.
 *   @ring: The ring.
 *   @in: The most recent input.
 *   &returns: The last value in the ring.
 */

static inline double dsp_ring_proc(struct dsp_ring_t *ring, double in)
{
	double out = dsp_ring_last(ring);

	dsp_ring_put(ring, in);

	return out;
}


/**
 * Process low-pass filter feedback loop.
 *   @x: The input.
 *   @prev: The previous value.
 *   @rc: The RC constant.
 *   &returns: The next value.
 */

static inline double dsp_lpf_proc(double x, double prev, double rc)
{
	return (1.0 - rc) * x + rc * prev;
}


/*
 * ring buffer function declarations
 */

struct dsp_ring_t *dsp_ring_new(unsigned int len);
void dsp_ring_delete(struct dsp_ring_t *ring);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
