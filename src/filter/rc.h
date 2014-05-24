#ifndef FILTER_RC_H
#define FILTER_RC_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * RC filter constant.
 *   @val: The constant value.
 */

struct dsp_rc_1 {
	double val;
};

/**
 * RC band filter constant.
 *   @low, high: Loc-pass and high-pass components.
 */

struct dsp_rc_2 {
	double low, high;
};


/**
 * Calculate the RC constant.
 *   @rate: The rate.
 *   @freq: The frequency.
 *   &returns: The constant.
 */

static inline struct dsp_rc_1 dsp_rc_1(unsigned int rate, double freq)
{
	double tau, dt;

	dt = 1.0 / rate;
	tau = 1.0 / (2.0 * M_PI * freq);

	return (struct dsp_rc_1){ tau / (tau + dt) };
}

/**
 * Calculate the RC constant for a band filter.
 *   @rate: The rate.
 *   @low: The low frequency.
 *   @high: The high frequency.
 *   &returns: The constant.
 */

static inline struct dsp_rc_2 dsp_rc_2(unsigned int rate, double low, double high)
{
	return (struct dsp_rc_2){ dsp_rc_1(rate, high).val, dsp_rc_1(rate, low).val };
}


/**
 * Process a low-pass RC filter.
 *   @x: The input value.
 *   @c: The constant.
 *   @data: The one-point data store.
 *   &returns: The output.
 */

static inline double dsp_rc_low(double x, struct dsp_rc_1 c, struct dsp_data_1 *data)
{
	double y;

	y = (1.0 - c.val) * x + c.val * data->y;
	dsp_data_1_inc(data, x, y);

	return y;
}

/**
 * Process a high-pass RC filter.
 *   @x: The input value.
 *   @c: The constant.
 *   @data: The one-point data store.
 *   &returns: The output.
 */

static inline double dsp_rc_high(double x, struct dsp_rc_1 c, struct dsp_data_1 *data)
{
	double y;

	y = c.val * (x + data->y - data->x);
	dsp_data_1_inc(data, x, y);

	return y;
}

/**
 * Process a band-pass RC filter.
 *   @x: The input value.
 *   @c: The constant.
 *   @data: The two-point data store.
 *   &returns: The output.
 */

static inline double dsp_rc_pass(double x, struct dsp_rc_2 c, struct dsp_data_2 *data)
{
	double y;

	y = c.high * (1.0 - c.low) * x + c.low * (c.low - 1.0) * data->x[0]
		+ (c.low + c.high) * data->y[0] - c.low * c.high * data->y[1];
	dsp_data_2_inc(data, x, y);

	return y;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
