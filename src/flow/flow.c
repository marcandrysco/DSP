#include "../common.h"
#include "flow.h"
#include "inc.h"
#include "node.h"


/*
 * local function declarations
 */

static void proc_node(struct dsp_node_t *node, unsigned int len, uint8_t sel);

static void flow_commit(void *arg);
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

	flow->list = avltree_empty(compare_ptr, delete_noop);
	flow->node[0] = flow->node[1] = NULL;
	flow->len[0] = flow->len[1] = 0;

	flow->upsource = avltree_empty(compare_ptr, delete_noop);
	flow->upsink = avltree_empty(compare_ptr, delete_noop);

	return flow;
}

/**
 * Delete a flow.
 *   @flow: The flow.
 */

_export
void dsp_flow_delete(struct dsp_flow_t *flow)
{
	avltree_destroy(&flow->upsource);
	avltree_destroy(&flow->upsink);
	avltree_destroy(&flow->list);
	mem_delete(flow->node[0]);
	dsp_lock_destroy(&flow->lock);
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

	sync = flow->node[sel];
	cnt = flow->len[sel];

	for(i = 0; i < cnt; i++) {
		struct dsp_node_t *node = sync[i];

		for(ii = 0; ii < node->incnt; ii++) {
			struct dsp_sink_t *sink = node->sink[ii];

			if(sink->len[sel] > 0)
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
		struct dsp_sink_t **list = source->sink[sel];
		struct dsp_flow_buf_t *avail = source->buf;
		unsigned int ii, cnt = source->len[sel];

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
			if(sink->cnt == sink->len[sel]) {
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
 * Synchronize a node to a flow.
 *   @flow: The flow.
 *   @node: The node.
 *   @sync: The synchronization structure.
 */

_export
void dsp_flow_sync(struct dsp_flow_t *flow, struct dsp_node_t *node, struct dsp_sync_t *sync)
{
	if(sync == NULL) {
		struct dsp_sync_t sync;

		if(node->flow != NULL)
			throw("Node already synchronized.");

		sync = dsp_sync_empty();
		dsp_flow_sync(flow, node, &sync);
		dsp_sync_commit(&sync);
	}
	else {
		if(dsp_sync_add(sync, flow, flow_commit))
			dsp_lock_wrlock(&flow->lock);

		node->cnt = 0;
		node->flow = flow;

		avltree_insert(&flow->list, node, node);
	}
}

/**
 * Desynchronize a node from a flow.
 *   @node: The node.
 *   @flow: The flow.
 *   @sync: The synchronization structure.
 */

_export
void dsp_flow_desync(struct dsp_flow_t *flow, struct dsp_node_t *node, struct dsp_sync_t *sync)
{
	if(sync == NULL) {
		struct dsp_sync_t sync;
		unsigned int i;

		if(node->flow == NULL)
			throw("Node not synchronized.");

		for(i = 0; i < node->incnt; i++) {
			if(node->sink[i]->len[0] != 0)
				throw("Cannot desync attached node.");
		}

		for(i = 0; i < node->outcnt; i++) {
			if(node->source[i]->len[0] != 0)
				throw("Cannot desync attached node.");
		}

		sync = dsp_sync_empty();
		dsp_flow_desync(flow, node, &sync);
		dsp_sync_commit(&sync);
	}
	else {
		if(dsp_sync_add(sync, flow, flow_commit))
			dsp_lock_wrlock(&flow->lock);

		avltree_purge(&flow->list, node);
		node->flow = NULL;
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
		struct dsp_flow_t *flow = source->node->flow;

		if(flow != NULL) {
			if(dsp_sync_add(sync, flow, flow_commit))
				dsp_lock_wrlock(&flow->lock);

			if(!avltree_lookup(&flow->upsource, source))
				avltree_insert(&flow->upsource, source, source);

			if(!avltree_lookup(&flow->upsink, sink))
				avltree_insert(&flow->upsink, sink, sink);
		}

		avltree_insert(&source->list, sink, sink);
		avltree_insert(&sink->list, source, source);
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
		struct dsp_flow_t *flow = source->node->flow;

		if(dsp_sync_add(sync, flow, flow_commit))
			dsp_lock_wrlock(&flow->lock);

		if(!avltree_lookup(&flow->upsource, source))
			avltree_insert(&flow->upsource, source, source);

		if(!avltree_lookup(&flow->upsink, sink))
			avltree_insert(&flow->upsink, sink, sink);

		avltree_purge(&source->list, sink);
		avltree_purge(&sink->list, source);
	}
}


/**
 * Commit a flow on the lock.
 *   @arg: The argument.
 */

static void flow_commit(void *arg)
{
	struct dsp_flow_t *flow = arg;
	unsigned int i;
	struct dsp_node_t *node;
	struct avltree_iter_t iter;
	struct dsp_source_t *source;
	struct dsp_sink_t *sink;

	flow->len[0] = flow->list.count;
	if(flow->len[0] > 0) {
		flow->node[0] = mem_alloc(sizeof(void *) * flow->len[0]);

		iter = avltree_iter_begin(&flow->list);
		for(i = 0; (node = avltree_iter_next(&iter)) != NULL; i++)
			flow->node[0][i] = node;
	}
	else
		flow->node[0] = NULL;

	iter = avltree_iter_begin(&flow->upsource);
	while((source = avltree_iter_next(&iter)) != NULL) {
		struct avltree_iter_t iter;

		source->len[0] = source->list.count;
		if(source->len[0] > 0) {
			source->sink[0] = mem_alloc(sizeof(void *) * source->len[0]);

			iter = avltree_iter_begin(&source->list);
			for(i = 0; (sink = avltree_iter_next(&iter)) != NULL; i++)
				source->sink[0][i] = sink;
		}
		else
			source->sink[0] = NULL;
	}

	iter = avltree_iter_begin(&flow->upsink);
	while((sink = avltree_iter_next(&iter)) != NULL) {
		struct avltree_iter_t iter;

		sink->len[0] = sink->list.count;
		if(sink->len[0] > 0) {
			sink->source[0] = mem_alloc(sizeof(void *) * sink->len[0]);

			iter = avltree_iter_begin(&sink->list);
			for(i = 0; (source = avltree_iter_next(&iter)) != NULL; i++)
				sink->source[0][i] = source;
		}
		else
			sink->source[0] = NULL;
	}

	dsp_lock_wrswap(&flow->lock);

	mem_delete(flow->node[1]);
	flow->len[1] = flow->len[0];
	flow->node[1] = flow->node[0];

	iter = avltree_iter_begin(&flow->upsource);
	while((source = avltree_iter_next(&iter)) != NULL) {
		mem_delete(source->sink[1]);
		source->len[1] = source->len[0];
		source->sink[1] = source->sink[0];
	}

	iter = avltree_iter_begin(&flow->upsink);
	while((sink = avltree_iter_next(&iter)) != NULL) {
		mem_delete(sink->source[1]);
		sink->len[1] = sink->len[0];
		sink->source[1] = sink->source[0];
	}

	dsp_lock_wrunlock(&flow->lock);
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
