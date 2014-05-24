#include "common.h"
#include "conv.h"


_export
unsigned int dsp_int2len(unsigned int inrate, unsigned int outrate, unsigned int len, unsigned int nchan)
{
	return ceil((double)len * (double)outrate / (double)inrate / (double)nchan);
}

/**
 * Convert interlaced data to an array.
 *   @in: The input data.
 *   @inrate: The input rate.
 *   @out: The output data.
 *   @outrate: The output rate.
 *   @len: The input data length.
 *   @nchan: The number of channels.
 *   &returns: THe output data length.
 */

_export
unsigned int dsp_int2arr(double *in, unsigned int inrate, double **out, unsigned int outrate, unsigned int len, unsigned int nchan)
{
	//double idx;
	unsigned int i, outlen;

	outlen = dsp_int2len(inrate, outrate, len, nchan);
	for(i = 0; i < outlen; i++) {
		//idx = i /
	}

	return outlen;
}
