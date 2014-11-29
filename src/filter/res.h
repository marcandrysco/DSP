#ifndef FILTER_RES_H
#define FILTER_RES_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Notch filter constant.
 *   @a, b, c, d: Filter constants.
 */

struct dsp_notch_t {
	double a, b, c, d, g;
};


/**
 * Calculate the filter constant.
 *   @rate: The rate.
 *   @freq: The frequency.
 *   @k: The resanonce constant in the range zero to one.
 *   &returns: The constant.
 */

static inline struct dsp_notch_t dsp_notch_init(unsigned int rate, double freq, double k)
{
	double w, dt;
	struct dsp_notch_t c;

	w = 2.0 * M_PI * freq;
	dt = 1.0 / (double)rate;

	c.a = -2.0 * exp(w*dt);
	c.b = exp(2.0*w*dt);
	c.c = -2.0*exp(w*dt*(k-1.0)/(k+1.0)) * cos(2*w*dt*sqrt(k)/(k+1.0));
	c.d = exp(2.0*w*dt*(k-1.0)/(k+1.0));
	c.g = (1.0 + c.c + c.d) / (1.0 + c.a + c.b);
	printf("%f\n%f\n%f\n%f\n%f\n", c.a, c.b, c.c, c.d, c.g);
	printf("%f vs %f\n", c.c+c.d, c.a+c.b);

	return c;
}

/**
 * Compute the next value with the notch filter with resonance.
 *   @x: The input value.
 *   @c: The filter constant.
 *   @data: The historical data.
 *   &returns: The output value.
 */

static inline double dsp_notch_res(double x, struct dsp_notch_t c, struct dsp_data_2 *data)
{
	double y;

	static int i =0;
	y = c.g*(x + c.a*data->x[0] + c.b*data->x[1]) - c.c*data->y[0] - c.d*data->y[1];
	if(i++==1000)
		printf("%f -> %f + %f + %f = %f\n", x, c.g*x, c.g*c.a*data->x[0], c.g*c.b*data->x[1], c.g*x+ c.g*c.a*data->x[0]+ c.g*c.b*data->x[1]),
		printf("+ %f*%f + %f*%f = %f\n", -c.c,data->y[0], -c.d,data->y[1], y);
	dsp_data_2_inc(data, x, y);

	return y;
}

/**
 * Compute the next value with the notch filter by cutting.
 *   @x: The input value.
 *   @c: The filter constant.
 *   @data: The historical data.
 *   &returns: The output value.
 */

static inline double dsp_notch_cut(double x, struct dsp_notch_t c, struct dsp_data_2 *data)
{
	double y;

	static int i = 0;
	y = (x + c.c*data->x[0] + c.d*data->x[1])/c.g - c.a*data->y[0] - c.b*data->y[1];
	if(i++==1000)
		printf("%f -> %f + %f + %f = %f\n", x, x/c.g, c.c/c.g*data->x[0], c.d/c.g*data->x[1], x/c.g+ c.c/c.g*data->x[0]+ c.d/c.g*data->x[1]),
		printf("+ %f*%f + %f*%f = %f\n", -c.a,data->y[0], -c.b,data->y[1], y);
	dsp_data_2_inc(data, x, y);

	return y;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
