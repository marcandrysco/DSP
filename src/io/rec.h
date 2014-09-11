#ifndef IO_REC_H
#define IO_REC_H

/*
 * headers
 */


/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Recording callback function.
 *   @buf: The buffer array.
 *   @len: The buffer length.
 *   @arg: The callback argument.
 */

typedef void (*dsp_rec_f)(double **buf, unsigned int len, void *arg);


/*
 * structure prototypes
 */

struct dsp_rec_t;

/*
 * recorder function declarations
 */

struct dsp_rec_t *dsp_rec_new(unsigned int nchan, unsigned int len);
void dsp_rec_delete(struct dsp_rec_t *rec);

void dsp_rec_conf(struct dsp_rec_t *rec, dsp_rec_f func, void *arg);

void dsp_rec_proc(struct dsp_rec_t *rec, double **buf, unsigned int len);
void dsp_rec_flush(struct dsp_rec_t *rec);

/* %~dsp.h% */

/*
 * End Header Creation: snd.h
 */

#endif
