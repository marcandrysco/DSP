#ifndef DEFS_H
#define DEFS_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/**
 * Non-blocking reader lock structure.
 *   @mutex: The writer mutex.
 *   @lock: The set of locks.
 */

struct dsp_lock_t {
	struct thread_mutex_t mutex;
	struct thread_rwlock_t lock[2];
};


/**
 * Perform a modulus returning an always positive result.
 *   @n: Tne input number.
 *   @m: The modulus.
 *   &returns; The modified number.
 */

static inline unsigned int dsp_mod(int n, unsigned int m)
{
	return ((n % m) + m) % m;
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
