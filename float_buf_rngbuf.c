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
    return (struct float_buf *)rngbuf_new(size*sizeof(float));
}

int
float_buf_lookup(struct float_buf *fb, unsigned int n, float *dest)
{
    return rngbuf_memcpy(
        (struct rngbuf *)fb,
        n*sizeof(float),
        sizeof(float),
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
        start*sizeof(float),
        length*sizeof(float))) != 0) {
        return ret;
    }
    process((float*)rbs.first_region,rbs.first_region_size/sizeof(float),aux);
    process((float*)rbs.second_region,rbs.second_region_size/sizeof(float),aux);
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
    nvalues*sizeof(float));
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
    start*sizeof(float),
    length*sizeof(float),
    (char *)dest);
}

/*
Make an array of struct float_buf_where_val.
The value is included in the array if chk returns non-zero when called on the
value.
fun is then called on the array of values.
*/
int
float_buf_where_values(
    struct float_buf *fb,
    unsigned int start,
    unsigned int length,
    int (*chk)(float val, void *aux),
    void (*fun)(struct float_buf_where_val *v,
                unsigned int nvals,
                void *aux),
    void *aux)
{
    struct rngbuf_slice rbs;
    int ret;
    /* get view of all the values */
    if ((ret = 
    rngbuf_get_slice(
        (struct rngbuf *)fb,
        &rbs,
        sizeof(float)*start,
        sizeof(float)*length)) != 0) {
        return ret;
    }
    /* Count the number of times chk returns non-zero */
    unsigned int nwhere = 0, n, idx_accum = start;
    for (n = 0; n < (rbs.first_region_size/sizeof(float)); n++) {
        nwhere += chk(((float*)rbs.first_region)[n],aux) != 0 ? 1 : 0;
    }
    for (n = 0; n < (rbs.second_region_size/sizeof(float)); n++) {
        nwhere += chk(((float*)rbs.second_region)[n],aux) != 0 ? 1 : 0;
    }
    /* allocate space on stack for passed values */
    struct float_buf_where_val wherevals[nwhere];
    float val;
    nwhere = 0;
    /* copy in the values */
    for (n = 0; n < (rbs.first_region_size/sizeof(float)); n++) {
        val = ((float*)rbs.first_region)[n];
        if (chk(val,aux) != 0) {
            wherevals[nwhere].f = val;
            wherevals[nwhere].n = idx_accum;
            nwhere++;
        }
        idx_accum++;
    }
    for (n = 0; n < (rbs.second_region_size/sizeof(float)); n++) {
        val = ((float*)rbs.second_region)[n];
        if (chk(val,aux) != 0) {
            wherevals[nwhere].f = val;
            wherevals[nwhere].n = idx_accum;
            nwhere++;
        }
        idx_accum++;
    }
    /* pass to the function */
    fun(wherevals,nwhere,aux);
    return 0;
}

int
float_buf_push_copy(
struct float_buf *fb,
unsigned int n,
const float *values)
{
    return rngbuf_push_copy((struct rngbuf *)fb,
           (char *)values, n*sizeof(float));
}
