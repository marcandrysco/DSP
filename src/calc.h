#ifndef DB_H
#define DB_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Convert amplitude to decibels.
 *   @amp: The amplitude value.
 *   &returns: The decibel value.
 */

static inline double dsp_amp2db(double amp)
{
	return 20.0 * log(amp) / log(10.0);
}

/**
 * Convert decibels to amplitude.
 *   @db: The decibel value.
 *   &returns: The amplitude value.
 */

static inline double dsp_db2amp(double db)
{
	return pow(10.0, db / 20.0);
}


/**
 * Clipping function.
 *   @val: The input value.
 *   &returns: The clipped value.
 */

static inline double dsp_clip(double val)
{
	return tanh(val);
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
