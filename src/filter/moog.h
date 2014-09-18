#ifndef FILTER_MOOG_H
#define FILTER_MOOG_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Moog resonant filter constant.
 *   @a, b, w, g: The polynomial, exponential, and gain constants.
 */

struct dsp_moog_t {
	double a, b, w, g;
};


/**
 * Calculate the RC constant.
 *   @rate: The rate.
 *   @freq: The frequency.
 *   @res: The resanonce constant in the range zero to four.
 *   &returns: The constant.
 */

static inline struct dsp_moog_t dsp_moog_init(unsigned int rate, double freq, double res)
{
	double dt, k, w;
	struct dsp_moog_t c;

	w = freq * 2.0 * M_PI;
	dt = 1.0 / (double)rate;
	k = pow(res, 0.25) * sqrt(2.0) / 2.0 * w * dt;

	c.a = 2.0 * exp(k) * cos(k) + 2.0 * exp(-k) * cos(k);
	c.b = 2.0 + exp(2.0*k) + exp(-2.0*k) + 2.0 * cos(2.0*k);
	c.w = exp(-w * dt);
	c.g = (1.0 - c.w * (c.a - c.w * (c.b - c.w * (c.a - c.w)))) / (1.0 + res);

	return c;
}

/**
 * Compute the next value with the moog resonant filter.
 *   @x: The input value.
 *   @c: The filter constant.
 *   @data: The historical data.
 *   &returns: The output value.
 */

static inline double dsp_moog_next(double x, struct dsp_moog_t c, struct dsp_data_4 *data)
{
	double y;

	y = c.g * x + c.w * (c.a * data->y[0] - c.w * (c.b * data->y[1] - c.w * (c.a * data->y[2] - c.w * data->y[3])));
	dsp_data_4_inc(data, x, y);

	return y;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
