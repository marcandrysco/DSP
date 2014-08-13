#include "common.h"


/*
 * test function declarations
 */

bool test_map();


/**
 * Coprime algorithm test.
 *   &returns: True of success, false on failure.
 */

bool test_coprime()
{
	unsigned int i, pt[6];

	printf("coprime algorithm... ");

	for(i = 0; i < 6; i++)
		pt[i] = 2;

	dsp_coprimes(pt, 6);

	if((pt[0] != 2) || (pt[1] != 3) || (pt[2] != 5) || (pt[3] != 7) || (pt[4] != 11) || (pt[5] != 13))
		printf("failed\n"), sys_exit(1);

	pt[0] = 3;
	pt[1] = 14;
	pt[2] = 800;
	pt[3] = 1400;
	pt[4] = 1435;
	dsp_coprimes(pt, 5);

	if((pt[0] != 3) || (pt[1] != 14) || (pt[2] != 803) || (pt[3] != 1403) || (pt[4] != 1439))
		printf("failed\n"), sys_exit(1);

	printf("okay\n");

	return true;
}


/**
 * Non-blocking array test.
 *   &returns: True of success, false on failure.
 */

bool test_array()
{
	struct dsp_array_t arr;

	printf("array... ");

	{
		arr = dsp_array_empty(sizeof(void *));
		dsp_array_destroy(&arr);
	}

	{
		void **data;
		uint8_t sel;
		unsigned int len;

		arr = dsp_array_empty(sizeof(void *));

		dsp_array_add(&arr, mem_getref(void *, (void *)1), NULL);
		dsp_array_add(&arr, mem_getref(void *, (void *)2), NULL);

		sel = dsp_array_lock(&arr, (void **)&data, &len);
		if((len != 2) || (data[0] != (void *)1) || (data[1] != (void *)2))
			printf("failed\n"), sys_exit(1);
		dsp_array_unlock(&arr, sel);

		dsp_array_add(&arr, mem_getref(void *, (void *)3), NULL);

		sel = dsp_array_lock(&arr, (void **)&data, &len);
		if((len != 3) || (data[0] != (void *)1) || (data[1] != (void *)2) || (data[2] != (void *)3))
			printf("failed\n"), sys_exit(1);
		dsp_array_unlock(&arr, sel);

		dsp_array_destroy(&arr);
	}

	printf("okay\n");

	return true;
}


/**
 * Sample rate conversion test.
 *   &returns: True of success, false on failure.
 */

bool test_conv()
{
	printf("conv... ");

	if(dsp_int2len(3, 2, 3, 1) != 2)
		printf("failed\n"), sys_exit(1);

	if(dsp_int2len(3, 5, 3, 1) != 5)
		printf("failed\n"), sys_exit(1);

	if(dsp_int2len(3, 2, 2, 1) != 2)
		printf("failed\n"), sys_exit(1);

	if(dsp_int2len(48000, 2, 24000, 1) != 1)
		printf("failed\n"), sys_exit(1);

	if(dsp_int2len(48000, 2, 24001, 1) != 2)
		printf("failed\n"), sys_exit(1);

	if(dsp_int2len(48000, 17828, 44100, 2) != 8190)
		printf("failed\n"), sys_exit(1);

	printf("okay\n");

	return true;
}


/**
 * Main entry point.
 *   @argc: The number of arguments.
 *   @argv: The argument array.
 *   &returns: The return status code.
 */

int main(int argc, char *argv[])
{
	bool suc = true;

	suc &= test_coprime();
	suc &= test_array();
	suc &= test_conv();
	suc &= test_map();

	return suc ? 0 : 1;
}
