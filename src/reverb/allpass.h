#ifndef REVERB_ALLPASS_H
#define REVERB_ALLPASS_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * All-pass reverberator.
 *   @gain: The gain.
 *   @delay: The delay.
 */

struct dsp_reverb_allpass_t {
	double gain;
	unsigned int delay;
};

/**
 * All-pass reverberator with low-pass feedback.
 *   @gain: The gain.
 *   @delay: The delay.
 *   @rc, mix: The RC constant and feedback mix
 */

struct dsp_reverb_allpass_lpf_t {
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

static inline struct dsp_reverb_allpass_t dsp_reverb_allpass_new(double gain, unsigned int delay)
{
	return (struct dsp_reverb_allpass_t){ gain, delay };
}

/**
 * Create an all-pass reverberator with low-pass feedback.
 *   @gain: The gain.
 *   @delay: The delay.
 *   @rc: The RC constant.
 *   @mix: The feedback mix.
 *   &returns: The reverberator.
 */

static inline struct dsp_reverb_allpass_lpf_t dsp_reverb_allpass_lpf_new(double gain, unsigned int delay, unsigned int rate, double freq, double mix)
{
	double tau, dt;

	dt = 1.0 / rate;
	tau = 1.0 / (2.0 * M_PI * freq);

	return (struct dsp_reverb_allpass_lpf_t){ gain, delay, tau / (tau + dt), mix };
}


/**
 * Process an all-pass reverberator.
 *   @x: The input.
 *   @ring: The ring buffer.
 *   @allpass: The all-pass reverberator.
 *   &returns: The next output.
 */

static inline double dsp_reverb_allpass(double x, struct dsp_ring_t *ring, struct dsp_reverb_allpass_t allpass)
{
	double v, y;

	v = dsp_ring_get(ring, -allpass.delay);
	x -= allpass.gain * v;
	y = v + allpass.gain * x;
	dsp_ring_put(ring, x);

	return y;
}

/**
 * Process an all-pass reverberator.
 *   @ring: The ring buffer.
 *   @x: The input.
 *   @allpass: The all-pass, low-pass feedback reverberator.
 *   &returns: The next output.
 */

static inline double dsp_reverb_allpass_lpf(double x, struct dsp_ring_t *ring, struct dsp_reverb_allpass_lpf_t allpass)
{
	double v, y;

	v = dsp_ring_get(ring, -allpass.delay);
	x -= allpass.gain * v;
	y = v + allpass.gain * x;
	x = (1.0 - allpass.mix) * x + allpass.mix * dsp_lpf_proc(x, dsp_ring_get(ring, -1), allpass.rc);
	dsp_ring_put(ring, x);

	return y;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
