#ifndef REVERB_COMB_H
#define REVERB_COMB_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * All-pass reverberator.
 *   @gain: The gain.
 *   @delay: The delay.
 */

struct dsp_reverb_comb_t {
	double gain;
	unsigned int delay;
};

/**
 * All-pass reverberator with low-pass feedback.
 *   @gain: The gain.
 *   @delay: The delay.
 *   @rc, mix: The RC constant and feedback mix
 */

struct dsp_reverb_comb_lpf_t {
	double gain;
	unsigned int delay;

	double rc, mix;
};


/**
 * Create an all-pass reverberator.
 *   @gain: The gain.
 *   @delay: The delay.
 *   &returns: The reverberator.
 */

static inline struct dsp_reverb_comb_t dsp_reverb_comb_new(double gain, unsigned int delay)
{
	return (struct dsp_reverb_comb_t){ gain, delay };
}

/**
 * Create an all-pass reverberator with low-pass feedback.
 *   @gain: The gain.
 *   @delay: The delay.
 *   @rc: The RC constant.
 *   @mix: The feedback mix.
 *   &returns: The reverberator.
 */

static inline struct dsp_reverb_comb_lpf_t dsp_reverb_comb_lpf_new(double gain, unsigned int delay, unsigned int rate, double freq, double mix)
{
	double tau, dt;

	dt = 1.0 / rate;
	tau = 1.0 / (2.0 * M_PI * freq);

	return (struct dsp_reverb_comb_lpf_t){ gain, delay, tau / (tau + dt), mix };
}


/**
 * Process an all-pass reverberator.
 *   @ring: The ring buffer.
 *   @x: The input.
 *   @comb: The all-pass reverberator.
 *   &returns: The next output.
 */

static inline double dsp_reverb_comb(struct dsp_ring_t *ring, double x, struct dsp_reverb_comb_t comb)
{
	x += -comb.gain * dsp_ring_get(ring, -comb.delay);
	dsp_ring_put(ring, x);

	return x;
}

/**
 * Process an all-pass reverberator.
 *   @ring: The ring buffer.
 *   @x: The input.
 *   @comb: The all-pass, low-pass feedback reverberator.
 *   &returns: The next output.
 */

static inline double dsp_reverb_comb_lpf(struct dsp_ring_t *ring, double x, double g, unsigned int d, struct dsp_reverb_comb_lpf_t comb)
{
	double v;

	x += -comb.gain * dsp_ring_get(ring, -comb.delay);
	v = (1.0 - comb.mix) * x + comb.mix * dsp_lpf_proc(x, dsp_ring_get(ring, -1), comb.rc);
	dsp_ring_put(ring, v);

	return x;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
