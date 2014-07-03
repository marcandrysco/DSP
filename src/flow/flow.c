#include "../common.h"
#include "flow.h"
#include "inc.h"


/*
 * local function declarations
 */

static void proc_node(struct dsp_node_t *node, unsigned int len, uint8_t sel);

static void flow_realloc(struct dsp_flow_t *flow);
static void flow_reset(struct dsp_flow_t *flow);

static struct dsp_flow_buf_t *bufget(struct dsp_flow_t *flow, unsigned int idx);
static struct dsp_flow_buf_t *bufnext(struct dsp_flow_t *flow);
static void bufdel(struct dsp_flow_t *flow, struct dsp_flow_buf_t *buf);
static void bufzero(struct dsp_flow_buf_t *buf, unsigned int len);
static void bufcopy(struct dsp_flow_buf_t *dest, struct dsp_flow_buf_t *src, unsigned int len);
static void bufadd(struct dsp_flow_buf_t *dest, struct dsp_flow_buf_t *src, unsigned int len);


/**
 * Create a new flow.
 *   @nbuf: The number of buffers.
 *   @buflen: The length of the buffers.
 *   &returns: The flow.
 */

_export
struct dsp_flow_t *dsp_flow_new()
{
	struct dsp_flow_t *flow;

	flow = mem_alloc(sizeof(struct dsp_flow_t));
	flow->nbuf = 0;
	flow->buflen = 0;
	flow->buf = NULL;
	flow_reset(flow);

	flow->lock = dsp_lock_gen();
	flow->sync = dsp_array_empty(sizeof(void *));

	return flow;
}

/**
 * Delete a flow.
 *   @flow: The flow.
 */

_export
void dsp_flow_delete(struct dsp_flow_t *flow)
{
	dsp_lock_destroy(&flow->lock);
	dsp_array_destroy(&flow->sync);
	mem_delete(flow->buf);
	mem_free(flow);
}


/**
 * Configure the flow.
 *   @nbuf: The number of buffer.
 *   @buflen: The buffer length.
 */

_export
void dsp_flow_conf(struct dsp_flow_t *flow, unsigned int nbuf, unsigned int buflen)
{
	flow->nbuf = nbuf;
	flow->buflen = buflen;

	flow_realloc(flow);
}

/**
 * Retrieve the number of buffers.
 *   @flow: The flow.
 *   &returns: The buffer count;
 */

_export
unsigned int dsp_flow_nbuf_get(struct dsp_flow_t *flow)
{
	return flow->nbuf;
}

/**
 * Set the number of buffers.
 *   @flow: The flow.
 *   @nbuf: The number of buffers.
 */

_export
void dsp_flow_nbuf_set(struct dsp_flow_t *flow, unsigned int nbuf)
{
	flow->nbuf = nbuf;

	flow_realloc(flow);
}

/**
 * Retrieve the length of the buffers
 *   @flow: The flow.
 *   &returns: The buffer length;
 */

_export
unsigned int dsp_flow_buflen_get(struct dsp_flow_t *flow)
{
	return flow->buflen;
}

/**
 * Set the buffer length.
 *   @flow: The flow.
 *   @buflen: The buffer length.
 */

_export
void dsp_flow_buflen_set(struct dsp_flow_t *flow, unsigned int buflen)
{
	flow->buflen = buflen;

	flow_realloc(flow);
}


/**
 * Process data through a flow.
 *   @flow: The flow.
 *   @len: The length.
 */

_export
void dsp_flow_proc(struct dsp_flow_t *flow, unsigned int len)
{
	uint8_t sel;
	struct dsp_node_t **sync;
	unsigned int i, ii, cnt;

	flow_reset(flow);

	sel = dsp_lock_rdlock(&flow->lock);

	sync = (struct dsp_node_t **)flow->sync.data[sel];
	cnt = flow->sync.len[sel];

	for(i = 0; i < cnt; i++) {
		struct dsp_node_t *node = sync[i];

		for(ii = 0; ii < node->incnt; ii++) {
			struct dsp_sink_t *sink = node->sink[ii];

			if(sink->source.len[sel] > 0)
				continue;

			bufzero(sink->accum = bufnext(flow), len);
			node->cnt++;
		}

		if(node->cnt == node->incnt)
			proc_node(node, len, sel);

		while(flow->queue != NULL) {
			node = flow->queue;
			flow->queue = node->next;

			proc_node(node, len, sel);
		}
	}

	dsp_lock_rdunlock(&flow->lock, sel);
}

/**
 * Process a flow node.
 *   @node: The node.
 *   @len: The length.
 *   @sel: The lock selector.
 */

static void proc_node(struct dsp_node_t *node, unsigned int len, uint8_t sel)
{
	unsigned int i;
	unsigned int maxcnt = node->incnt > node->outcnt ? node->incnt : node->outcnt;

	{
		double *set[maxcnt];
		struct dsp_flow_buf_t *buf;

		for(i = 0; i < maxcnt; i++) {
			if(i < node->incnt)
				buf = node->sink[i]->accum;
			else
				buf = bufnext(node->flow);

			if(i < node->outcnt)
				node->source[i]->buf = buf;

			set[i] = buf->arr;
		}

		node->func(set, len, node->arg);
	}

	node->cnt = 0;

	for(i = 0; i < node->outcnt; i++) {
		struct dsp_source_t *source = node->source[i];
		struct dsp_sink_t **list = (struct dsp_sink_t **)source->sink.data[sel];
		struct dsp_flow_buf_t *avail = source->buf;
		unsigned int ii, cnt = source->sink.len[sel];

		for(ii = 0; ii < cnt; ii++) {
			struct dsp_sink_t *sink = list[ii];

			if(sink->cnt == 0) {
				if(avail != NULL) {
					sink->accum = avail;
					avail = NULL;
				}
				else
					bufcopy(sink->accum = bufnext(node->flow), source->buf, len);
			}
			else
				bufadd(sink->accum, source->buf, len);

			if(avail != NULL)
				bufdel(node->flow, avail);

			sink->cnt++;
			if(sink->cnt == sink->source.len[sel]) {
				struct dsp_node_t *node = sink->node;

				sink->cnt = 0;
				node->cnt++;
				if(node->cnt == node->incnt) {
					struct dsp_flow_t *flow = node->flow;

					node->next = flow->queue;
					flow->queue = node;
				}
			}
		}
	}
}


/**
 * Attach two nodes together.
 *   @source: The source.
 *   @sink: The sink.
 *   @sync: The synchronization structure.
 */

_export
void dsp_flow_attach(struct dsp_source_t *source, struct dsp_sink_t *sink, struct dsp_sync_t *sync)
{
	if(sync == NULL) {
		struct dsp_sync_t sync;

		sync = dsp_sync_empty();
		dsp_flow_attach(source, sink, &sync);
		dsp_sync_commit(&sync);
	}
	else {
		dsp_array_addptr(&source->sink, sink, sync);
		dsp_array_addptr(&sink->source, source, sync);
	}
}

/**
 * Detach nodes together.
 *   @source: The source.
 *   @sink: The sink.
 *   @sync: The synchronization structure.
 */

_export
void dsp_flow_detach(struct dsp_source_t *source, struct dsp_sink_t *sink, struct dsp_sync_t *sync)
{
	if(sync == NULL) {
		struct dsp_sync_t sync;

		sync = dsp_sync_empty();
		dsp_flow_detach(source, sink, &sync);
		dsp_sync_commit(&sync);
	}
	else {
		dsp_array_remptr(&source->sink, sink, sync);
		dsp_array_remptr(&sink->source, source, sync);
	}
}


/**
 * Reallocate teh buffers.
 *   @flow: The flow.
 */

static void flow_realloc(struct dsp_flow_t *flow)
{
	flow->buf = mem_realloc(flow->buf, flow->nbuf * (sizeof(struct dsp_flow_buf_t) + flow->buflen * sizeof(double)));

	flow_reset(flow);
}

/**
 * Reset the flow available buffer.
 *   @flow: The flow.
 */

static void flow_reset(struct dsp_flow_t *flow)
{
	unsigned int i;

	if(flow->nbuf > 0) {
		bufget(flow, flow->nbuf - 1)->next = NULL;
		for(i = flow->nbuf - 2; i != (unsigned int )-1; i--)
			bufget(flow, i)->next = bufget(flow, i+1);

		flow->avail = bufget(flow, 0);
	}
	else
		flow->avail = NULL;

	flow->queue = NULL;
}


/**
 * Retrieve the given buffer. 
 *   @flow: The flow.
 *   @idx: The buffer index.
 *   &returns: The buffer.
 */

static struct dsp_flow_buf_t *bufget(struct dsp_flow_t *flow, unsigned int idx)
{
	return flow->buf + idx * (sizeof(struct dsp_flow_buf_t) + flow->buflen * sizeof(double));
}

/**
 * Retrieve the next available buffer.
 *   @flow: The flow.
 *   &returns: The buffer.
 */

static struct dsp_flow_buf_t *bufnext(struct dsp_flow_t *flow)
{
	struct dsp_flow_buf_t *buf;

	buf = flow->avail;
	flow->avail = buf->next;

	return buf;
}

/**
 * Delete a buffer, adding it back to the available queue.
 *   @flow: The flow.
 *   @buf: The buffer.
 */

static void bufdel(struct dsp_flow_t *flow, struct dsp_flow_buf_t *buf)
{
	buf->next = flow->avail;
	flow->avail = buf;
}

/**
 * Zero out a buffer.
 *   @buf: Teh buffer.
 *   @len: The length to zero.
 */

static void bufzero(struct dsp_flow_buf_t *buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		buf->arr[i] = 0.0;
}

/**
 * Copy a buffer.
 *   @dest: The destination.
 *   @src: The source.
 *   @len: The length.
 */

static void bufcopy(struct dsp_flow_buf_t *dest, struct dsp_flow_buf_t *src, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		dest->arr[i] = src->arr[i];
}

/**
 * Add to a buffer.
 *   @dest: The destination.
 *   @src: The source.
 *   @len: The length.
 */

static void bufadd(struct dsp_flow_buf_t *dest, struct dsp_flow_buf_t *src, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++)
		dest->arr[i] += src->arr[i];
}
