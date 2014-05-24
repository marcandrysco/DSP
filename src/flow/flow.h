#ifndef FLOW_FLOW_H
#define FLOW_FLOW_H

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
 * flow function declarations
 */

struct dsp_flow_t *dsp_flow_new();
void dsp_flow_delete(struct dsp_flow_t *flow);

void dsp_flow_conf(struct dsp_flow_t *flow, unsigned int nbuf, unsigned int buflen);
unsigned int dsp_flow_nbuf_get(struct dsp_flow_t *flow);
void dsp_flow_nbuf_set(struct dsp_flow_t *flow, unsigned int nbuf);
unsigned int dsp_flow_buflen_get(struct dsp_flow_t *flow);
void dsp_flow_buflen_set(struct dsp_flow_t *flow, unsigned int buflen);

void dsp_flow_proc(struct dsp_flow_t *flow, unsigned int len);

void dsp_flow_attach(struct dsp_source_t *source, struct dsp_sink_t *sink, struct dsp_sync_t *sync);
void dsp_flow_detach(struct dsp_source_t *source, struct dsp_sink_t *sink, struct dsp_sync_t *sync);

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

void dsp_node_sync(struct dsp_node_t *node, struct dsp_flow_t *flow, struct dsp_sync_t *sync);
void dsp_node_desync(struct dsp_node_t *node, struct dsp_sync_t *sync);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
