#ifndef IO_PLAY_H
#define IO_PLAY_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Playback callback function.
 *   @buf: The buffer array.
 *   @len: The buffer length.
 *   @arg: The callback argument.
 */

typedef void (*dsp_play_f)(double **buf, unsigned int len, void *arg);


/*
 * structure prototypes
 */

struct dsp_play_t;

/*
 * play function declarations
 */

struct dsp_play_t *dsp_play_new(unsigned int outcnt, unsigned int len);
void dsp_play_delete(struct dsp_play_t *play);

void dsp_play_conf(struct dsp_play_t *play, dsp_play_f func, void *arg);

void dsp_play_proc(struct dsp_play_t *play, double **buf, unsigned int len);
void dsp_play_prep(struct dsp_play_t *play);
void dsp_play_drop(struct dsp_play_t *play);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
