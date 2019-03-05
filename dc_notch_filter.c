#include <stdlib.h>
#include "dc_notch_filter.h"
#include "dc_notch_filter_private.h"

/* Returns non-zero on failure. */
static int
init_arg_chk(struct dc_notch_filter_1_pole_init *i)
{
    if ((i->r < 0.) || (i->r >= 1.)) { return -1; }
    return 0;
}

void
dc_notch_filter_1_pole_free(struct dc_notch_filter_1_pole *d)
{
    if (d) { free(d); }
}

struct dc_notch_filter_1_pole *
dc_notch_filter_1_pole_new(
struct dc_notch_filter_1_pole_init *i)
{
    struct dc_notch_filter_1_pole *ret = calloc(1,sizeof(struct dc_notch_filter_1_pole));
    if (!ret) { goto fail; }
    *ret = (struct dc_notch_filter_1_pole) {
        .a1 = -i->r,
        .b = {i->r,-i->r},
        .d_1 = 0,
        .buffer_size = i->buffer_size
    };
    return ret;
fail:
    dc_notch_filter_1_pole_free(ret);
    return NULL;
}

/* Naive implementation of the filtering operation */
__attribute__((weak,optimize("-O3")))
void
dc_notch_filter_1_pole_tick(
struct dc_notch_filter_1_pole *d,
float *buf)
{
    unsigned int buffer_size = d->buffer_size;
    float b0 = d->b[0], b1 = d->b[1], a1 = d->a1, d_1 = d->d_1, xn, yn;
    while (buffer_size-- > 0) {
        /*
        Direct form II transposed:
        y[n] + y[n-1]*a1 = x[n]*b0 + x[n-1]*b1 ->
        y[n] = x[n]*b0 + x[n-1]*b1 - y[n-1]*a1 ->
        y[n] = x[n]*b0 + d_1
        */
        xn = *buf;
        yn = xn * b0 + d_1;
        d_1 = b1 * xn - a1 * yn;
        *buf++ = yn;
    }
    d->d_1 = d_1;
}

