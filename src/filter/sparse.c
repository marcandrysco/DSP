#include "../common.h"
#include "sparse.h"
#include "../reverb/ring.h"


/**
 * Create an empty sparse filter.
 *   @npts: The number of points.
 *   @len: The length.
 *   &returns: The sparse filter.
 */

_export
struct dsp_filter_sparse_t *dsp_filter_sparse_empty(unsigned int npts, unsigned int len)
{
	unsigned int i;
	struct dsp_filter_sparse_t *sparse;

	sparse = mem_alloc(sizeof(struct dsp_filter_sparse_t) + npts * sizeof(struct dsp_pt_t));
	sparse->ring = dsp_ring_new(len);
	sparse->npts = npts;

	for(i = 0; i < npts; i++)
		sparse->pt[i] = (struct dsp_pt_t){ 0, 0.0 };

	return sparse;
}

/**
 * Delete a sparse filter.
 *   @sparse: The sparse filter.
 */

_export
void dsp_filter_sparse_delete(struct dsp_filter_sparse_t *sparse)
{
	dsp_ring_delete(sparse->ring);
	mem_free(sparse);
}
