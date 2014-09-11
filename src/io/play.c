#include "../common.h"
#include "play.h"


/**
 * Playback structure.
 *   @func: the callback function.
 *   @arg: the callback argument.
 *   @thread: the thread.
 *   @sync: The synchorization variable.
 *   @lock: The synchorization lock.
 *   @sel: the current buffer to process.
 *   @i: The current index.
 *   @outcnt, buflen: The output channel count and total buffer length.
 *   @buf: the buffer.
 */

struct dsp_play_t {
	dsp_play_f func;
	void *arg;

	struct thread_t *thread;
	struct thread_cond_t sync;
	struct thread_mutex_t lock;

	uint8_t sel;
	unsigned int i;
	unsigned int outcnt, buflen;

	double buf[];
};


/*
 * local function declarations
 */

static void *thread_proc(void *arg);


/**
 * Create a new playback helper.
 *   @outcnt: The number of output channels.
 *   @buflen: The length of the internal buffer.
 */

_export
struct dsp_play_t *dsp_play_new(unsigned int outcnt, unsigned int buflen)
{
	struct dsp_play_t *play;

	play = mem_alloc(sizeof(struct dsp_play_t) + 2 * outcnt * buflen * sizeof(double));
	play->i = 0;
	play->buflen = buflen;
	play->outcnt = outcnt;
	play->func = NULL;
	play->arg = NULL;
	play->sync = thread_cond_new(NULL);
	play->lock = thread_mutex_new(NULL);
	play->thread = thread_new(thread_proc, play, NULL);

	return play;
}

/**
 * Delete a playback helper.
 *   @play: The playback helper.
 */

_export
void dsp_play_delete(struct dsp_play_t *play)
{
	play->buflen = 0;

	thread_mutex_lock(&play->lock);
	thread_cond_signal(&play->sync);
	thread_mutex_unlock(&play->lock);

	thread_join(play->thread);
	thread_cond_delete(&play->sync);
	thread_mutex_delete(&play->lock);

	mem_free(play);
}


/**
 * Configure the playback.
 *   @play: The playback helper.
 *   @func: The callback function.
 *   @arg: The callback argument.
 */

_export
void dsp_play_conf(struct dsp_play_t *play, dsp_play_f func, void *arg)
{
	play->func = func;
	play->arg = arg;
}


/**
 * Process a set of data.
 *   @play: The playback helper.
 *   @buf: The output buffer.
 *   @len: The length of the input.
 */

_export
void dsp_play_proc(struct dsp_play_t *play, double **buf, unsigned int len)
{
	unsigned int i, ii, idx, buflen = play->buflen, outcnt = play->outcnt;

	for(i = 0, idx = play->i; len > 0; i++, idx++, len--) {
		if((idx > 0) && (idx % buflen) == 0) {
			idx %= 2*buflen;

			thread_mutex_lock(&play->lock);

			play->i = idx;
			play->sel = 1 ^ (idx / buflen);

			thread_cond_signal(&play->sync);
			thread_mutex_unlock(&play->lock);
		}

		for(ii = 0; ii < outcnt; ii++)
			buf[ii][i] += play->buf[2*ii*buflen + idx];
	}

	play->i = idx;
}

/**
 * Prepare the playback by filling the buffer. The function does not return
 * until the thread has finished filling the buffer.
 *   @play: The playback helper.
 */

_export
void dsp_play_prep(struct dsp_play_t *play)
{
	thread_mutex_lock(&play->lock);

	play->i = 0;

	play->sel = 0;
	thread_cond_signal(&play->sync);
	thread_cond_wait(&play->sync, &play->lock);

	play->sel = 1;
	thread_cond_signal(&play->sync);
	thread_cond_wait(&play->sync, &play->lock);

	thread_mutex_unlock(&play->lock);
}


/**
 * Processing thread.
 *   @arg: The playback argument.
 *   &returns: Always 'NULL'.
 */

static void *thread_proc(void *arg)
{
	unsigned int i, len;
	struct dsp_play_t *play = arg;
	double *buf[2][play->outcnt];

	for(i = 0; i < play->outcnt; i++) {
		buf[0][i] = &play->buf[(2*i + 0)*play->buflen];
		buf[1][i] = &play->buf[(2*i + 1)*play->buflen];
	}

	thread_mutex_lock(&play->lock);

	while(play->buflen > 0) {
		thread_cond_wait(&play->sync, &play->lock);
		len = play->buflen;

		if(len == 0)
			break;

		if(play->func != NULL)
			play->func(buf[play->sel], play->buflen, play->arg);

		thread_cond_signal(&play->sync);
	}

	thread_mutex_unlock(&play->lock);

	return NULL;
}
