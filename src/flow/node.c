#include "../common.h"
#include "node.h"
#include "inc.h"
#include "flow.h"


/*
 * local function declarations
 */

static struct dsp_sink_t *sink_new(struct dsp_node_t *node);
static void sink_delete(struct dsp_sink_t *sink);

static struct dsp_source_t *source_new(struct dsp_node_t *node);
static void source_delete(struct dsp_source_t *source);


/**
 * Create a new node.
 *   @incnt: The input count.
 *   @outcnt: The output count.
 *   @func: The callback function.
 *   @arg: The callback argument.
 *   &returns: The node.
 */

_export
struct dsp_node_t *dsp_node_new(unsigned int incnt, unsigned int outcnt, dsp_flow_f func, void *arg)
{
	struct dsp_node_t *node;
	unsigned int i;

	node = mem_alloc(sizeof(struct dsp_node_t));
	node->flow = NULL;
	node->incnt = incnt;
	node->outcnt = outcnt;
	node->func = func;
	node->arg = arg;

	node->sink = mem_alloc(incnt * sizeof(void *));
	for(i = 0; i < incnt; i++)
		node->sink[i] = sink_new(node);

	node->source = mem_alloc(outcnt * sizeof(void *));
	for(i = 0; i < outcnt; i++)
		node->source[i] = source_new(node);

	return node;
}

/**
 * Delete a node.
 *   @node: The node.
 */

_export
void dsp_node_delete(struct dsp_node_t *node)
{
	unsigned int i;

	for(i = 0; i < node->incnt; i++)
		sink_delete(node->sink[i]);

	for(i = 0; i < node->outcnt; i++)
		source_delete(node->source[i]);

	mem_free(node->sink);
	mem_free(node->source);
	mem_free(node);
}


/**
 * Retrieve the number of input channels.
 *   @node: The node.
 *   &returns: The number of input channels.
 */

_export
unsigned int dsp_node_incnt(struct dsp_node_t *node)
{
	return node->incnt;
}

/**
 * Retrieve the number of output channels.
 *   @node: The node.
 *   &returns: The number of output channels.
 */

_export
unsigned int dsp_node_outcnt(struct dsp_node_t *node)
{
	return node->outcnt;
}


/**
 * Retrieve the sink from the node.
 *   @node: The node.
 *   @idx: The sink index.
 *   &returns: The sink.
 */

_export
struct dsp_sink_t *dsp_node_sink(struct dsp_node_t *node, unsigned int idx)
{
	return node->sink[idx];
}

/**
 * Retrieve the source from the node.
 *   @node: The node.
 *   @idx: The source index.
 *   &returns: The source.
 */

_export
struct dsp_source_t *dsp_node_source(struct dsp_node_t *node, unsigned int idx)
{
	return node->source[idx];
}


/**
 * Configure the node callback.
 *   @node: The node.
 *   @func: The callback function.
 *   @arg: The callback argument.
 */

_export
void dsp_node_conf(struct dsp_node_t *node, dsp_flow_f func, void *arg)
{
	node->func = func;
	node->arg = arg;
}

/**
 * Resize the node.
 *   @node: The node.
 *   @incnt: The input count.
 *   @outcnt: The output count.
 */

_export
void dsp_node_resize(struct dsp_node_t *node, unsigned int incnt, unsigned int outcnt)
{
	unsigned int i;

	for(i = incnt; i < node->incnt; i++)
		sink_delete(node->sink[i]);

	for(i = outcnt; i < node->outcnt; i++)
		source_delete(node->source[i]);

	node->sink = mem_realloc(node->sink, incnt * sizeof(void *));
	node->source = mem_realloc(node->source, outcnt * sizeof(void *));

	for(i = node->incnt; i < incnt; i++)
		node->sink[i] = sink_new(node);

	for(i = node->outcnt; i < outcnt; i++)
		node->source[i] = source_new(node);

	node->incnt = incnt;
	node->outcnt = outcnt;
}


/**
 * Reset the node, detaching all nodes.
 *   @node: The node.
 */

_export
void dsp_node_reset(struct dsp_node_t *node)
{
	unsigned int i;
	struct dsp_source_t *source;
	struct dsp_sink_t *sink;

	for(i = 0; i < node->outcnt; i++) {
		source = node->source[i];

		while(source->len[0] > 0)
			dsp_flow_detach(source, source->sink[0][0], NULL);
	}

	for(i = 0; i < node->incnt; i++) {
		sink = node->sink[i];

		while(sink->len[0] > 0)
			dsp_flow_detach(sink->source[0][0], sink, NULL);
	}
}


/**
 * Create a new sink.
 *   @node: The node.
 *   &returns: The sink.
 */

static struct dsp_sink_t *sink_new(struct dsp_node_t *node)
{
	struct dsp_sink_t *sink;

	sink = mem_alloc(sizeof(struct dsp_sink_t));
	sink->node = node;
	sink->cnt = 0;
	sink->accum = 0;
	sink->list = avltree_empty(compare_ptr, delete_noop);
	sink->source[0] = sink->source[1] = NULL;
	sink->len[0] = sink->len[1] = 0;

	return sink;
}

/**
 * Destroy the sink.
 *   @sink: The sink.
 */

static void sink_delete(struct dsp_sink_t *sink)
{
	avltree_destroy(&sink->list);
	mem_delete(sink->source[0]);
	mem_free(sink);
}


/**
 * Create a new source.
 *   @node: The node.
 *   &returns: The source.
 */

static struct dsp_source_t *source_new(struct dsp_node_t *node)
{
	struct dsp_source_t *source;

	source = mem_alloc(sizeof(struct dsp_source_t));
	source->node = node;
	source->node = node;
	source->list = avltree_empty(compare_ptr, delete_noop);
	source->sink[0] = source->sink[1] = NULL;
	source->len[0] = source->len[1] = 0;

	return source;
}

/**
 * Destroy the source.
 *   @source: The source.
 */

static void source_delete(struct dsp_source_t *source)
{
	avltree_destroy(&source->list);
	mem_delete(source->sink[0]);
	mem_free(source);
}
