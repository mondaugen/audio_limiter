/* A ring buffer */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuffer.h"

#ifndef MIN
#define MIN(x,y) (((x)<(y))?(x):(y))
#endif

struct rngbuf {
    /* The index in the array of the first of the contiguous region of values */
    unsigned int head_idx;
    /* The index in the array one past the last of the contiguous region of
    values */
    unsigned int tail_idx;
    /* The size in bytes of the memory region where data is stored. */
    unsigned int size;
    /* A mask that makes wrapping the index quick */
    unsigned int size_mask;
    /* The memory where the data is stored */
    char *data;
};

void
rngbuf_free(struct rngbuf *rb)
{
    free(rb);
}

/*
Capacity is the minimum number of values you want to be able to store in the
ring buffer. Its actual capacity will probably be greater.
*/
struct rngbuf *
rngbuf_new(unsigned int capacity)
{
    if (capacity == 0) { return NULL; }
    unsigned int size = 1;
    /*
    We need a slot for a sentinel between head and tail. This is so that the
    empty condition is head_idx == tail_idx and the full condition is tail_idx
    == wrap(head_idx - 1,0,size)
    */
    capacity += 1;
    /* size is made to be power of 2 so we can wrap the indices quickly */
    while (size < capacity) { size <<= 1; }
    struct rngbuf *ret = calloc(1,sizeof(struct rngbuf)+size);
    ret->data = (char *)(ret+1);
    ret->size = size;
    ret->size_mask = size - 1;
    return ret;
}

/* The size of the stuff currently stored in the ring buffer */
unsigned int
rngbuf_contents_size(struct rngbuf *rb)
{
    return (rb->tail_idx - rb->head_idx) & rb->size_mask;
}

/* The size of the space available in the ring buffer */
unsigned int
rngbuf_available_capacity(struct rngbuf *rb)
{
    return (rb->head_idx - 1 - rb->tail_idx) & rb->size_mask;
}

/*
Advance the head of the ring buffer. This effectively discards the first n
values and makes the beginning of the ring buffer n values later than
previously. If the capacity of the ring buffer is less than n, no advance can be
made and -1 is returned. Otherwise 0 is returned.
*/
int
rngbuf_advance_head(struct rngbuf *rb, unsigned int n)
{
    if (rngbuf_contents_size(rb) < n) { return -1; }
    rb->head_idx = (rb->head_idx + n) & rb->size_mask;
    return 0;
}

/* Put the n values in data at the end of the ring buffer and advance the tail
index. If there's not enough capacity available, returns -1, otherwise 0. data
should not overlap with a region in the ring buffer (like memcpy) */
int
rngbuf_push_copy(struct rngbuf *rb, const char *data, unsigned int n)
{
    if (rngbuf_available_capacity(rb) < n) { return -1; }
    unsigned int first_region_size = MIN(rb->size - rb->tail_idx,n),
                 second_region_size = n - first_region_size;
    memcpy(rb->data+rb->tail_idx,data,first_region_size);
    memcpy(rb->data,data+first_region_size,second_region_size);
    rb->tail_idx = (rb->tail_idx + n) & rb->size_mask;
    return 0;
}

/* Advance the head n values and then push n values in */
int
rngbuf_shift_in(
    struct rngbuf *rb,
    const char *data,
    unsigned int n)
{
    if (rngbuf_advance_head(rb,n)) { return -1; }
    if (rngbuf_push_copy(rb,data,n)) { return -2; }
    return 0;
}

int
rngbuf_get_slice(
    struct rngbuf *rb,
    struct rngbuf_slice *rb_slice,
    unsigned int start,
    unsigned int length)
{
    /*
    Start must be in [0,rngbuf_contents_size(rb)-1]
    Length must be in [0,rngbuf_contents_size(rb)-start]
    */
    unsigned int contents_size = rngbuf_contents_size(rb),
                 start_idx;
    if (start >= contents_size) { return -1; }
    if (length > (contents_size-start)) { return -2; }
    start_idx = (start + rb->head_idx) & rb->size_mask;
    rb_slice->first_region_size = MIN(rb->size - start_idx,length);
    rb_slice->second_region_size = length - rb_slice->first_region_size;
    rb_slice->first_region = rb->data + start_idx;
    rb_slice->second_region = (rb_slice->second_region_size > 0) ? rb->data : NULL;
    return 0;
}

/*
Copy contents out of ring buffer. Destination should not overlap with the
ring buffer.
*/
int
rngbuf_memcpy(
    struct rngbuf *rb,
    unsigned int start,
    unsigned int length,
    char *dest)
{
    struct rngbuf_slice rbs;
    int ret;
    if ((ret = rngbuf_get_slice(
        rb,
        &rbs,
        start,
        length)) != 0) {
        return ret;
    }
    memcpy(dest,rbs.first_region,rbs.first_region_size);
    dest += rbs.first_region_size;
    memcpy(dest,rbs.second_region,rbs.second_region_size);
    return 0;
}
    
