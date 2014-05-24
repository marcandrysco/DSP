#include "common.h"
#include "algo.h"


/**
 * Compute a set of nearest coprime values.
 *   @pt: The point array.
 *   @npts: The number of points.
 */

_export
void dsp_coprimes(unsigned int *pt, unsigned int npts)
{
	unsigned int i, j;

	for(i = 1; i < npts; i++) {
		for(j = 0; j < i; j++) {
			if(m_gcd(pt[i], pt[j]) == 1)
				continue;

			j = 0;
			do
				pt[i]++;
			while(m_gcd(pt[i], pt[j]) != 1);
		}
	}
}
