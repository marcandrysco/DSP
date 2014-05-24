#ifndef LOCK_H
#define LOCK_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/*
 * lock function declarations
 */

struct dsp_lock_t dsp_lock_gen();
void dsp_lock_destroy(struct dsp_lock_t *lock);

uint8_t dsp_lock_rdlock(struct dsp_lock_t *lock);
void dsp_lock_rdunlock(struct dsp_lock_t *lock, uint8_t idx);

void dsp_lock_wrlock(struct dsp_lock_t *lock);
void dsp_lock_wrswap(struct dsp_lock_t *lock);
void dsp_lock_wrunlock(struct dsp_lock_t *lock);

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
