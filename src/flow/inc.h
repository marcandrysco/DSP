#ifndef FLOW_INC_H
#define FLOW_INC_H

/*
 * common headers
 */

#include "../types/array.h"
#include "../types/lock.h"
#include "../types/sync.h"


/**
 * Flow structure.
 *   @nbuf: The nuber of buffers, the buffer length.
 *   @buf: The buffer array.
 *   @lock: The lock.
 *   @sync: The synchronized sources.
 *   @queue: The queued nodes.
 *   @avail: The available buffers.
 */

struct dsp_flow_t {
	unsigned int nbuf, buflen;
	void *buf;

	struct dsp_lock_t lock;

	struct dsp_array_t sync;

	struct dsp_node_t *queue;
	struct dsp_flow_buf_t *avail;
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
 *   @source: The array of sources.
 *   @cnt: The accumulation count.
 *   @accum: The accumulation buffer.
 */

struct dsp_sink_t {
	struct dsp_node_t *node;

	struct dsp_array_t source;

	unsigned int cnt;
	struct dsp_flow_buf_t *accum;
};

/**
 * Source structure.
 *   @node: The node.
 *   @sink: The array of sinks.
 *   @buf: The associated buffer.
 */

struct dsp_source_t {
	struct dsp_node_t *node;

	struct dsp_array_t sink;

	struct dsp_flow_buf_t *buf;
};

#endif
