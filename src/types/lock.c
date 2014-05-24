#include "../common.h"
#include "lock.h"


/**
 * Initialize a lock. 
 *  @lock: The lock.
 */

_export
void dsp_lock_init(struct dsp_lock_t *lock)
{
	lock->mutex = thread_mutex_new(NULL);
	lock->lock[0] = thread_rwlock_new(NULL);
	lock->lock[1] = thread_rwlock_new(NULL);
}

/**
 * Create a lock.
 *   &returns: The lock.
 */

_export
struct dsp_lock_t dsp_lock_gen()
{
	struct dsp_lock_t lock;

	lock.mutex = thread_mutex_new(NULL);
	lock.lock[0] = thread_rwlock_new(NULL);
	lock.lock[1] = thread_rwlock_new(NULL);

	return lock;
}

/**
 * Destroy a lock. 
 *  @lock: The lock.
 */

_export
void dsp_lock_destroy(struct dsp_lock_t *lock)
{
	thread_mutex_delete(&lock->mutex);
	thread_rwlock_delete(&lock->lock[0]);
	thread_rwlock_delete(&lock->lock[1]);
}


/**
 * Lock for reading.
 *   @lock: The lock.
 *   &returns: The locked index.
 */

_export
uint8_t dsp_lock_rdlock(struct dsp_lock_t *lock)
{
	uint8_t idx;

	for(idx = 1; ; idx ^= 1) {
		if(thread_rwlock_tryrdlock(&lock->lock[idx]))
			break;
	}

	return idx;
}

/**
 * Unlock from reading.
 *   @lock: The lock.
 *   @idx: The locked index.
 */

_export
void dsp_lock_rdunlock(struct dsp_lock_t *lock, uint8_t idx)
{
	thread_rwlock_rdunlock(&lock->lock[idx]);
}


/**
 * Lock for writing. The second lock is held after this operation.
 *   @lock: The lock.
 */

_export
void dsp_lock_wrlock(struct dsp_lock_t *lock)
{
	thread_mutex_lock(&lock->mutex);
	thread_rwlock_wrlock(&lock->lock[0]);
}

/**
 * Swap the write locks. The second lock is released and the first lock is
 * held after this operation.
 *   @lock: The lock.
 */

_export
void dsp_lock_wrswap(struct dsp_lock_t *lock)
{
	thread_rwlock_wrunlock(&lock->lock[0]);
	thread_rwlock_wrlock(&lock->lock[1]);
}

/**
 * Unlock the write lock.
 *   @lock: The lock.
 */

_export
void dsp_lock_wrunlock(struct dsp_lock_t *lock)
{
	thread_rwlock_wrunlock(&lock->lock[1]);
	thread_mutex_unlock(&lock->mutex);
}
