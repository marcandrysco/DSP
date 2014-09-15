#ifndef FILTER_SPARSE_H
#define FILTER_SPARSE_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */


/**
 * Infinite impulse response filter.
 *   @ring: The ring buffer.
 *   @npts: The number of points.
 *   @pt: The point set.
 */

struct dsp_filter_sparse_t {
	struct dsp_ring_t *ring;

	unsigned int npts;
	struct dsp_pt_t pt[];
};


/*
 * sparse function declarations
 */

struct dsp_filter_sparse_t *dsp_filter_sparse_empty(unsigned int npts, unsigned int len);
void dsp_filter_sparse_delete(struct dsp_filter_sparse_t *sparse);


/**
 * Process the parse filter as an FIR filter.
 *   @sparse: The sparse filter.
 *   @x: The input.
 *   &returns: The output.
 */

static inline double dsp_filter_sparse_fir(struct dsp_filter_sparse_t *sparse, double x)
{
	double y;
	unsigned int i, npts;
	struct dsp_pt_t *pt;
	
	y = x;
	pt = sparse->pt;
	npts = sparse->npts;

	for(i = 0; i < npts; i++)
		y += pt[i].a * dsp_ring_get(sparse->ring, -pt[i].n);

	dsp_ring_put(sparse->ring, x);

	return y;
}

/**
 * Process the parse filter as an IIR filter.
 *   @sparse: The sparse filter.
 *   @x: The input.
 *   &returns: The output.
 */

static inline double dsp_filter_sparse_iir(struct dsp_filter_sparse_t *sparse, double x)
{
	unsigned int i, npts;
	struct dsp_pt_t *pt;
	
	pt = sparse->pt;
	npts = sparse->npts;

	for(i = 0; i < npts; i++)
		x += pt[i].a * dsp_ring_get(sparse->ring, -pt[i].n);

	dsp_ring_put(sparse->ring, x);

	return x;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
