#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "dc_notch_filter_test_data.h"

struct dc_notch_filter_1_pole {
    /* the a1 coefficient, we assume a0 is 1 */
    float a1;
    /* The b0 and b1 coefficient */
    float b[2];
    /* the previous delay line values */
    float d_1;
    /* the number of values this can process */
    unsigned int buffer_size;
};

struct dc_notch_filter_1_pole_init {
    /* The "radius" of the pole. Needs to be between 0 and 1. Values closer to 1
    make a sharper notch. */
    float r;
    unsigned int buffer_size;
};

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

/* Naive implementation of the filtering operation, a faster version can override this */
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

#define TOL 1e-4

int chk_correct(float *a, const float *b, unsigned int len, float *max_diff, unsigned int *idx_md)
{
    int correct = 1;
    float diff;
    *max_diff = 0;
    unsigned int idx = 0;
    while ((len-- > 0) && (correct != 0)) {
        diff = fabs(*a++ - *b++);
        correct &= diff < TOL;
        if (diff > *max_diff) { *max_diff = diff; *idx_md = idx; }
        idx++;
    }
    return correct;
}

static float ans1[NVALS];
static float ans2[NVALS];

__attribute__((optimize("-O0")))
int main (void)
{
    float biggest_diff;
    unsigned int idx_biggest_diff;
    memcpy(ans1,x,sizeof(float)*NVALS);
    memcpy(ans2,x,sizeof(float)*NVALS);
    struct dc_notch_filter_1_pole_init di = {
        .r = r,
        .buffer_size = NVALS
    };
    struct dc_notch_filter_1_pole *d;
    while (1) {
        d = dc_notch_filter_1_pole_new(&di);
        assert(d);
        dc_notch_filter_1_pole_tick(d,ans1);
        assert(chk_correct(ans1,y,NVALS,&biggest_diff,&idx_biggest_diff) == 1);
        memcpy(ans1,x,sizeof(float)*NVALS);
        dc_notch_filter_1_pole_free(d);
    }
    return 0;
}
        

        
