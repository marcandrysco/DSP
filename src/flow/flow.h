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
struct dsp_node_t;
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

void dsp_flow_sync(struct dsp_flow_t *flow, struct dsp_node_t *node, struct dsp_sync_t *sync);
void dsp_flow_desync(struct dsp_flow_t *flow, struct dsp_node_t *node, struct dsp_sync_t *sync);

void dsp_flow_attach(struct dsp_source_t *source, struct dsp_sink_t *sink, struct dsp_sync_t *sync);
void dsp_flow_detach(struct dsp_source_t *source, struct dsp_sink_t *sink, struct dsp_sync_t *sync);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
