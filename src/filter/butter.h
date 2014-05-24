#ifndef FILTER_BUTTER_H
#define FILTER_BUTTER_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Butterworth 2nd order filter constant.
 *   @val: The constant value.
 */

struct dsp_butter_2 {
	double a, b1, b2;
};


/**
 * Calculate the constant for a 2nd-order butterworth filter.
 *   @rate: The data rate.
 *   @freq: The frequency.
 *   &returns: The constant.
 */

static inline struct dsp_butter_2 dsp_butter_2(unsigned int rate, double freq)
{
	double a, b1, b2, c;

	c = freq / (rate * sqrt(2));
	b1 = -2 * exp(-c) * cos(c);
	b2 = exp(-2 * c);
	a = 1 + b1 + b2;

	return (struct dsp_butter_2){ a, b1, b2 };;
}

/**
 * Process a 2nd-order low-pass butterworth filter.
 *   @x: The input value.
 *   @c: The constant.
 *   @data: The two point data store.
 *   &returns: The output.
 */

static inline double dsp_butter2_low(double x, struct dsp_butter_2 c, struct dsp_data_2 *data)
{
	double y;

	y = c.a * x - c.b1 * data->y[0] - c.b2 * data->y[1];
	dsp_data_2_inc(data, x, y);

	return y;
}

/**
 * Process a 2nd-order high-pass butterworth filter.
 *   @x: The input value.
 *   @c: The constant.
 *   @data: The two point data store.
 *   &returns: The output.
 */

static inline double dsp_butter2_high(double x, struct dsp_butter_2 c, struct dsp_data_2 *data)
{
	double y;

	y = x + -2 * data->x[0] + data->x[1] - c.b1 * data->y[0] - c.b2 * data->y[1];
	dsp_data_2_inc(data, x, y);

	return y;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
