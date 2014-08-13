#ifndef MAP_H
#define MAP_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/*
 * map function declarations
 */

struct dsp_map_t *dsp_map_new(unsigned int in, unsigned int out);
void dsp_map_delete(struct dsp_map_t *map);
void dsp_map_clear(struct dsp_map_t *map);

unsigned int *dsp_map_row(struct dsp_map_t *map, unsigned int in);

void dsp_map_add(struct dsp_map_t *map, unsigned int in, unsigned int out);
void dsp_map_remove(struct dsp_map_t *map, unsigned int in, unsigned int out);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
