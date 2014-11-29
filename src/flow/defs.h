#ifndef FLOW_DEFS_H
#define FLOW_DEFS_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Data flow callback function.
 *   @buf: The set of buffers.
 *   @len: The buffer length.
 *   @arg: The argument.
 */

typedef void (*dsp_flow_f)(double **buf, unsigned int len, void *arg);


/**
 * Flow structure.
 *   @nbuf: The nuber of buffers, the buffer length.
 *   @buf: The buffer array.
 *   @lock: The lock.
 *   @list: The node list.
 *   @node: The list of synchronized nodes.
 *   @len: The node list lengths.
 *   @queue: The queued nodes.
 *   @avail: The available buffers.
 *   @upsource, upsink: The source and sink update trees.
 */

struct dsp_flow_t {
	unsigned int nbuf, buflen;
	void *buf;

	struct dsp_lock_t lock;

	struct avltree_t list;
	struct dsp_node_t **node[2];
	unsigned int len[2];

	struct dsp_node_t *queue;
	struct dsp_flow_buf_t *avail;

	struct avltree_t upsource, upsink;
};

/**
 * Flow buffer structure.
 *   @next: The next buffer.
 *   @arr: The data array.
 */

struct dsp_flow_buf_t {
	struct dsp_flow_buf_t *next;

	double arr[];
};


/**
 * Flow node structure.
 *   @flow: The flow.
 *   @incnt, outcnt: The input and output count.
 *   @func: The callback function.
 *   @arg: The callback argument.
 *   @cnt: The accumulation count.
 *   @source: The set of sources.
 *   @sink: The set of sinks.
 *   @next: The next queued node.
 */

struct dsp_node_t {
	struct dsp_flow_t *flow;
	unsigned int incnt, outcnt;

	dsp_flow_f func;
	void *arg;

	unsigned int cnt;
	struct dsp_source_t **source;
	struct dsp_sink_t **sink;

	struct dsp_node_t *next;
};

/**
 * Sink structure.
 *   @node: The node.
 *   @list: The list of sources.
 *   @source: The array of sources.
 *   @lenes: The array lengths.
 *   @cnt: The accumulation count.
 *   @accum: The accumulation buffer.
 */

struct dsp_sink_t {
	struct dsp_node_t *node;


	struct avltree_t list;
	struct dsp_source_t **source[2];
	unsigned int len[2];

	unsigned int cnt;
	struct dsp_flow_buf_t *accum;
};

/**
 * Source structure.
 *   @node: The node.
 *   @list: The list of sinks.
 *   @sink: The array of sinks.
 *   @len: The array lengths.
 *   @buf: The associated buffer.
 */

struct dsp_source_t {
	struct dsp_node_t *node;

	struct avltree_t list;
	struct dsp_sink_t **sink[2];
	unsigned int len[2];

	struct dsp_flow_buf_t *buf;
};

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
