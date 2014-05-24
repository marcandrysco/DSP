#ifndef CONV_H
#define CONV_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/*
 * conversion function declarations
 */

unsigned int dsp_int2len(unsigned int inrate, unsigned int outrate, unsigned int len, unsigned int nchan);
void dsp_conf_int2arr(double *in, unsigned int inrate, double **out, unsigned int outrate, unsigned int nchan);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
