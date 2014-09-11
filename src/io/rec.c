#include "../common.h"
#include "rec.h"


/**
 * Recorder structure.
 *   @func: the callback function.
 *   @arg: the callback argument.
 *   @thread: the thread.
 *   @sync: The synchorization variable.
 *   @lock: The synchronizaiton lock.
 *   @sel: the current buffer to process.
 *   @i, len: The current index and processed length.
 *   @incnt, buflen: The input channel count and total buffer length.
 *   @buf: the buffer.
 */

struct dsp_rec_t {
	dsp_rec_f func;
	void *arg;

	struct thread_t *thread;
	struct thread_cond_t sync;
	struct thread_mutex_t lock;

	uint8_t sel;
	unsigned int i, len;
	unsigned int incnt, buflen;

	double buf[];
};


/*
 * local function declarations
 */

static void *thread_proc(void *arg);


/**
 * Create a new recorder.
 *   @cnt: The number of channels to record.
 *   @len: The buffer length.
 *   &returns: The recorder.
 */

_export
struct dsp_rec_t *dsp_rec_new(unsigned int cnt, unsigned int len)
{
	struct dsp_rec_t *rec;

	rec = mem_alloc(sizeof(struct dsp_rec_t) + 2 * len * cnt * sizeof(double));
	rec->i = 0;
	rec->buflen = len;
	rec->incnt = cnt;
	rec->func = NULL;
	rec->arg = NULL;
	rec->sync = thread_cond_new(NULL);
	rec->lock = thread_mutex_new(NULL);
	rec->thread = thread_new(thread_proc, rec, NULL);

	return rec;
}

/**
 * Delete a sound recorder.
 *   @rec: The recorder.
 */

_export
void dsp_rec_delete(struct dsp_rec_t *rec)
{
	rec->buflen = 0;

	thread_mutex_lock(&rec->lock);
	thread_cond_signal(&rec->sync);
	thread_mutex_unlock(&rec->lock);

	thread_join(rec->thread);
	thread_cond_delete(&rec->sync);
	thread_mutex_delete(&rec->lock);
	mem_free(rec);
}


/**
 * Configure the recorder.
 *   @rec: The recorder.
 *   @func: The callback function.
 *   @arg: The callback argument.
 */

_export
void dsp_rec_conf(struct dsp_rec_t *rec, dsp_rec_f func, void *arg)
{
	rec->func = func;
	rec->arg = arg;
}


/**
 * Process a set of data.
 *   @rec: The recorder.
 *   @buf: The input buffer.
 *   @len: The length of the input.
 */

_export
void dsp_rec_proc(struct dsp_rec_t *rec, double **buf, unsigned int len)
{
	unsigned int i, ii, idx, buflen = rec->buflen, cnt = rec->incnt;

	for(i = 0, idx = rec->i; len > 0; i++, idx++, len--) {
		if((idx > 0) && (idx % buflen) == 0) {
			idx %= 2*buflen;

			thread_mutex_lock(&rec->lock);

			rec->i = idx;
			rec->len = buflen;
			rec->sel = 1 ^ (idx / buflen);

			thread_cond_signal(&rec->sync);
			thread_mutex_unlock(&rec->lock);
		}

		for(ii = 0; ii < cnt; ii++)
			rec->buf[2*ii*buflen + idx] = buf[ii][i];
	}

	rec->i = idx;
}

/**
 * Flush any remaining, if any, data in the recorder. The return is blocked
 * until the data has been processed by the callback.
 *   @rec: The recorder.
 */

_export
void dsp_rec_flush(struct dsp_rec_t *rec)
{
	unsigned int len;

	thread_mutex_lock(&rec->lock);

	len = rec->i % rec->buflen;
	if(len > 0) {
		rec->len = len;
		rec->sel = rec->i / rec->buflen;

		thread_cond_signal(&rec->sync);
		thread_cond_wait(&rec->sync, &rec->lock);

		rec->i = 0;
	}

	thread_mutex_unlock(&rec->lock);
}


/**
 * Processing thread.
 *   @arg: The recoder argument.
 *   &returns: Always 'NULL'.
 */

static void *thread_proc(void *arg)
{
	unsigned int i, len;
	struct dsp_rec_t *rec = arg;
	double *buf[2][rec->incnt];

	for(i = 0; i < rec->incnt; i++) {
		buf[0][i] = &rec->buf[(2*i + 0)*rec->buflen];
		buf[1][i] = &rec->buf[(2*i + 1)*rec->buflen];
	}

	thread_mutex_lock(&rec->lock);

	while(rec->buflen > 0) {
		thread_cond_wait(&rec->sync, &rec->lock);
		len = rec->buflen;

		if(len == 0)
			break;

		if(rec->func != NULL)
			rec->func(buf[rec->sel], rec->len, rec->arg);

		thread_cond_signal(&rec->sync);
	}

	thread_mutex_unlock(&rec->lock);

	return NULL;
}
