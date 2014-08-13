#include "common.h"
#include "map.h"


/**
 * Channel map structure.
 */

struct dsp_map_t {
	unsigned int in, out;

	unsigned int arr[];
};


unsigned int *getref(struct dsp_map_t *map, unsigned int in, unsigned int idx)
{
	return &map->arr[in * (map->out + 1) + idx];
}


/**
 * Create an empty map.
 *   @in: The number of inputs.
 *   @out: The number of outputs.
 *   &returns: The map.
 */

_export
struct dsp_map_t *dsp_map_new(unsigned int in, unsigned int out)
{
	struct dsp_map_t *map;

	map = mem_alloc(sizeof(struct dsp_map_t) + in * (out + 1) * sizeof(unsigned int));
	map->in = in;
	map->out = out;
	dsp_map_clear(map);

	return map;
}

/**
 * Delete the map.
 *   @map: The map.
 */

_export
void dsp_map_delete(struct dsp_map_t *map)
{
	mem_free(map);
}


/**
 * Clear a map.
 *   @map: The map.
 */

_export
void dsp_map_clear(struct dsp_map_t *map)
{
	unsigned int i, ii;

	for(i = 0; i < map->in; i++)
		for(ii = 0; ii < map->out + 1; ii++)
		*getref(map, i, 0) = UINT_MAX;
}


/**
 * Retrieve the output row from the map.
 *   @map: The map.
 *   @in: The input.
 *   &returns: The output row.
 */

_export
unsigned int *dsp_map_row(struct dsp_map_t *map, unsigned int in)
{
	return getref(map, in, 0);
}


/**
 * Map an output for a given input.
 *   @map: The map.
 *   @in: The input.
 *   @out: The output.
 */

_export
void dsp_map_add(struct dsp_map_t *map, unsigned int in, unsigned int out)
{
	unsigned int i, ii;

	if((in >= map->in) || (out >= map->out))
		throw("Invalid mapping.");

	for(i = 0; i < map->out; i++) {
		if(*getref(map, in, i) >= out)
			break;
	}

	if(*getref(map, in, i) == out)
		throw("Mapping already exists.");

	for(ii = map->out; ii > i; ii--)
		*getref(map, in, ii) = *getref(map, in, ii - 1);

	*getref(map, in, i) = out;
}

/**
 * Unmap an output for a given input.
 *   @map: The map.
 *   @in: The input.
 *   @out: The output.
 */

_export
void dsp_map_remove(struct dsp_map_t *map, unsigned int in, unsigned int out)
{
	unsigned int i;

	if((in >= map->in) || (out >= map->out))
		throw("Invalid mapping.");

	for(i = 0; i < map->out; i++) {
		if(*getref(map, in, i) == out)
			break;
	}

	if(i == map->out)
		throw("No such mapping.");

	do
		*getref(map, in, i) = *getref(map, in, i + 1);
	while(*getref(map, in, i++) != UINT_MAX);
}
