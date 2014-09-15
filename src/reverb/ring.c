#include "../common.h"
#include "ring.h"


/**
 * Create a new ring buffer.
 *   @len: The length.
 *   &returns: The ring buffer.
 */

_export
struct dsp_ring_t *dsp_ring_new(unsigned int len)
{
	unsigned int i;
	struct dsp_ring_t *ring;

	ring = mem_alloc(sizeof(struct dsp_ring_t) + len * sizeof(double));
	ring->idx = 0;
	ring->len = len;

	for(i = 0; i < len; i++)
		ring->arr[i] = 0.0;

	return ring;
}

/**
 * Delete a ring buffer.
 *   @ring: The ring buffer.
 */

_export
void dsp_ring_delete(struct dsp_ring_t *ring)
{
	mem_free(ring);
}
