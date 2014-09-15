#ifndef REVERB_RING_H
#define REVERB_RING_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

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
