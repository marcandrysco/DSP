#ifndef ARRAY_H
#define ARRAY_H

/*
 * Start Header Creation: dsp.h
 */

/* %dsp.h% */

/*
 * structure prototypes
 */

struct dsp_sync_t;


/**
 * Array structure.
 *   @lock: Data lock.
 *   @nbytes: Size of each data element.
 *   @data: The internal data.
 *   @len: The lengths.
 */

struct dsp_array_t {
	struct dsp_lock_t lock;
	unsigned nbytes;

	void *data[2];
	unsigned int len[2];
};


/*
 * array function declarations
 */

struct dsp_array_t dsp_array_empty();
void dsp_array_destroy(struct dsp_array_t *arr);

void dsp_array_get(struct dsp_array_t *arr, void *restrict ptr, unsigned int idx, uint8_t sel);
void *dsp_array_getptr(struct dsp_array_t *arr, unsigned int idx, uint8_t sel);
void dsp_array_add(struct dsp_array_t *arr, const void *restrict ptr, struct dsp_sync_t *sync);
void dsp_array_addptr(struct dsp_array_t *arr, void *ptr, struct dsp_sync_t *sync);
bool dsp_array_rem(struct dsp_array_t *arr, const void *restrict ptr, struct dsp_sync_t *sync);
void dsp_array_remptr(struct dsp_array_t *arr, void *ptr, struct dsp_sync_t *sync);


/*
 * external function declarations
 */

uint8_t dsp_lock_rdlock(struct dsp_lock_t *lock);
void dsp_lock_rdunlock(struct dsp_lock_t *lock, uint8_t idx);


/**
 * Lock the array data for reading.
 *   @arr: The array.
 *   @data: Optional. A reference to store the data pointer.
 *   @len: Optional. A reference to store the data length.
 *   &returns: The locked selector.
 */

static inline uint8_t dsp_array_lock(struct dsp_array_t *arr, void **data, unsigned int *len)
{
	uint8_t sel;

	sel = dsp_lock_rdlock(&arr->lock);

	if(data != NULL)
		*data = arr->data[sel];

	if(len != NULL)
		*len = arr->len[sel];

	return sel;
}

/**
 * Unlock the array data.
 *   @arr: The array.
 *   @data: The array data.
 */

static inline void dsp_array_unlock(struct dsp_array_t *arr, uint8_t sel)
{
	dsp_lock_rdunlock(&arr->lock, sel);
}

/* %~dsp.h% */

/*
 * End Header Creation: dsp.h
 */

#endif
