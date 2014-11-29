#ifndef SCHED_LIVE_H
#define SCHED_LIVE_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Live scheduler.
 *   @len, size: The list length and element size.
 *   @head, tail: The head and tail instances,
 *   @data: The data.
 */

struct dsp_sched_live_t {
	unsigned int len, size;
	unsigned int rd, wr;
	struct dsp_sched_inst_t *head, *tail;

	uint8_t data[];
};

/**
 * Live scheduled instance.
 *   @active: The lock and active flag.
 *   @data: The data.
 */

struct dsp_sched_inst_t {
	uint8_t lock, active;

	uint8_t data[];
};


/*
 * live scheduler function declarations
 */

struct dsp_sched_live_t *dsp_sched_live_new(unsigned int len, unsigned int size);
void dsp_sched_live_delete(struct dsp_sched_live_t *live);

bool dsp_sched_live_add(struct dsp_sched_live_t *live, const void *data);
bool dsp_sched_live_remove(struct dsp_sched_live_t *live, void *data);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
