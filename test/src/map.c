#include "common.h"


/**
 * Channel mapping test.
 *   &returns: True of success, false on failure.
 */

bool test_map()
{
	struct dsp_map_t *map;
	unsigned int *row, ref0[3] = { 3, 4, UINT_MAX}, ref1[4] = { 0, 1, 3, UINT_MAX }, ref2[4] = { 0, 1, 2, UINT_MAX };

	printf("map... ");

	map = dsp_map_new(3, 5);

	dsp_map_add(map, 2, 0);
	dsp_map_add(map, 1, 3);
	dsp_map_add(map, 0, 4);
	dsp_map_add(map, 1, 4);
	dsp_map_add(map, 0, 2);
	dsp_map_add(map, 2, 1);
	dsp_map_add(map, 1, 0);
	dsp_map_add(map, 1, 1);
	dsp_map_add(map, 2, 2);
	dsp_map_remove(map, 1, 4);
	dsp_map_remove(map, 0, 2);
	dsp_map_add(map, 0, 3);

	row = dsp_map_row(map, 0);
	if(!mem_isequal(row, ref0, sizeof(ref0)))
		printf("failed\n"), sys_exit(1);

	row = dsp_map_row(map, 1);
	if(!mem_isequal(row, ref1, sizeof(ref1)))
		printf("failed\n"), sys_exit(1);

	row = dsp_map_row(map, 2);
	if(!mem_isequal(row, ref2, sizeof(ref2)))
		printf("failed\n"), sys_exit(1);

	dsp_map_delete(map);

	printf("okay\n");

	return true;
}
