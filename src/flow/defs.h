#ifndef FLOW_DEFS_H
#define FLOW_DEFS_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Data flow callback function.
 *   @buf: The set of buffers.
 *   @len: The buffer length.
 *   @arg: The argument.
 */

typedef void (*dsp_flow_f)(double **buf, unsigned int len, void *arg);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
