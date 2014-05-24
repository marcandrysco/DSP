#ifndef FILTER_DEFS_H
#define FILTER_DEFS_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * One data point store structure.
 *   @x: The input history.
 *   @y: The output history.
 */

struct dsp_data_1 {
	double x;
	double y;
};

/**
 * Two data point store structure.
 *   @x: The input history.
 *   @y: The output history.
 */

struct dsp_data_2 {
	double x[2];
	double y[2];
};


/**
 * Two data point store creator.
 *   &returns: The one point data.
 */

static inline struct dsp_data_1 dsp_data_1()
{
	return (struct dsp_data_1){ 0.0, 0.0 };
}

/**
 * Increment a one point data store.
 *   &returns: The one point data.
 *   @x: The input value.
 *   @y: The output value.
 */

static inline void dsp_data_1_inc(struct dsp_data_1 *data, double x, double y)
{
	data->x = x;
	data->y = y;
}


/**
 * Two data point store creator.
 *   &returns: The two point data.
 */

static inline struct dsp_data_2 dsp_data_2()
{
	return (struct dsp_data_2){ { 0.0, 0.0 }, { 0.0, 0.0 } };
}

/**
 * Increment a two point data store.
 *   &returns: The two point data.
 *   @x: The input value.
 *   @y: The output value.
 */

static inline void dsp_data_2_inc(struct dsp_data_2 *data, double x, double y)
{
	data->x[1] = data->x[0];
	data->x[0] = x;

	data->y[1] = data->y[0];
	data->y[0] = y;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
