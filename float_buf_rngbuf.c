#include "float_buf.h"
#include "ringbuffer.h"

/* implementation of float_buf using ring buffer. */

void
float_buf_free(struct float_buf *fb)
{
    rngbuf_free((struct rngbuf *)fb);
}

struct float_buf *
float_buf_new(unsigned int size)
{
    return (struct float_buf *)rngbuf_new(size,sizeof(float));
}

int
float_buf_lookup(struct float_buf *fb, unsigned int n, float *dest)
{
    return rngbuf_memcpy(
        (struct rngbuf *)fb,
        n,
        1,
        (char *)dest);
}

int
float_buf_process_region(
    struct float_buf *fb,
    unsigned int start,
    unsigned int length,
    void (*process)(
        float *seg,
        unsigned int len,
        void *aux),
    void *aux)
{
    struct rngbuf_slice rbs;
    int ret;
    if ((ret = rngbuf_get_slice(
        (struct rngbuf *)fb,
        &rbs,
        start,
        length)) != 0) {
        return ret;
    }
    process((float*)rbs.first_region,rbs.first_region_size,aux);
    process((float*)rbs.second_region,rbs.second_region_size,aux);
    return 0;
}

int
float_buf_shift_in(
    struct float_buf *fb,
    const float *values,
    unsigned int nvalues)
{
    return rngbuf_shift_in(
    (struct rngbuf *)fb,
    (char *)values,
    nvalues);
}

/*
Copy values out of the float buffer from start and extending for length into
dest.
*/
int
float_buf_memcpy(
    struct float_buf *fb,
    unsigned int start,
    unsigned int length,
    float *dest)
{
    return rngbuf_memcpy(
    (struct rngbuf *)fb,
    start,
    length,
    (char *)dest);
}

int
float_buf_push_copy(
struct float_buf *fb,
unsigned int n,
const float *values)
{
    return rngbuf_push_copy((struct rngbuf *)fb,
           (char *)values, n);
}
