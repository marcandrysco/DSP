#ifndef FLOW_NODE_H
#define FLOW_NODE_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/*
 * structure prototypes
 */

struct dsp_flow_t;
struct dsp_sink_t;
struct dsp_source_t;
struct dsp_sync_t;

/*
 * node function declarations
 */

struct dsp_node_t *dsp_node_new(unsigned int incnt, unsigned int outcnt, dsp_flow_f func, void *arg);
void dsp_node_delete(struct dsp_node_t *node);

unsigned int dsp_node_incnt(struct dsp_node_t *node);
unsigned int dsp_node_outcnt(struct dsp_node_t *node);

struct dsp_sink_t *dsp_node_sink(struct dsp_node_t *node, unsigned int idx);
struct dsp_source_t *dsp_node_source(struct dsp_node_t *node, unsigned int idx);

void dsp_node_reset(struct dsp_node_t *node);
void dsp_node_conf(struct dsp_node_t *node, dsp_flow_f func, void *arg);
void dsp_node_resize(struct dsp_node_t *node, unsigned int incnt, unsigned int outcnt);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
