#ifndef BUF_H
#define BUF_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Zero floating point data.
 *   @dest: The destination buffer.
 *   @len: The length.
 */

static inline void dsp_buf_zero(double *restrict dest, unsigned int len)
{
	mem_set(dest, 0x00, len * sizeof(double));
}

/**
 * Copy floating point data.
 *   @dest: The destination buffer.
 *   @src: The source buffer.
 *   @len: The length.
 */

static inline void dsp_buf_copy(double *restrict dest, const double *restrict src, unsigned int len)
{
	mem_copy(dest, src, len * sizeof(double));
}

/**
 * Add floating point data.
 *   @dest: The destination buffer.
 *   @src: The source buffer.
 *   @len: The length.
 */

static inline void dsp_buf_add(double *restrict dest, const double *restrict src, unsigned int len)
{
	while(len-- > 0)
		*dest++ += *src++;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
