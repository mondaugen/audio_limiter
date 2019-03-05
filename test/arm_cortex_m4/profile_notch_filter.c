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

/* Naive implementation of the filtering operation */
__attribute__((optimize("-O3")))
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

/* Unrolled implementation of the filtering operation */
__attribute__((optimize("-O3")))
void
dc_notch_filter_1_pole_tick_unrolled4(
struct dc_notch_filter_1_pole *d,
float *buf)
{
    unsigned int buffer_size = d->buffer_size,
                 blk_cnt = buffer_size >> 2U;
    float        b0 = d->b[0],
                 b1 = d->b[1],
                 a1 = d->a1,
                 d_1 = d->d_1,
                 xn0,xn1,xn2,xn3,
                 yn0,yn1,yn2,yn3;
    while (blk_cnt-- > 0) {
        /*
        Direct form II transposed:
        y[n] + y[n-1]*a1 = x[n]*b0 + x[n-1]*b1 ->
        y[n] = x[n]*b0 + x[n-1]*b1 - y[n-1]*a1 ->
        y[n] = x[n]*b0 + d_1
        */
        xn0 = *buf;
        xn1 = *(buf+1);
        xn2 = *(buf+2);
        xn3 = *(buf+3);

        yn0 = xn0 * b0;
        yn0 += d_1;
        d_1 = b1 * xn0;
        d_1 -= a1 * yn0;

        yn1 = xn1 * b0;
        yn1 += d_1;
        d_1 = b1 * xn1;
        d_1 -= a1 * yn1;

        yn2 = xn2 * b0;
        yn2 += d_1;
        d_1 = b1 * xn2;
        d_1 -= a1 * yn2;

        yn3 = xn3 * b0;
        yn3 += d_1;
        d_1 = b1 * xn3;
        d_1 -= a1 * yn3;

        *buf = yn0;
        *(buf+1) = yn1;
        *(buf+2) = yn2;
        *(buf+3) = yn3;
        buf+=4;
    }
    blk_cnt = buffer_size & 3U;
    while (blk_cnt-- > 0) {
        xn0 = *buf;
        yn0 = xn0 * b0;
        yn0 += d_1;
        d_1 = b1 * xn0;
        d_1 -= a1 * yn0;
        *buf = yn0;
        buf++;
    }
    d->d_1 = d_1;
}

/*
Unrolled implementation of the filtering operation, but like cmsis. This is
ever so slightly faster than our unrolled function.
*/
__attribute__((optimize("-O3")))
void
dc_notch_filter_1_pole_tick_unrolled4_cmsis(
struct dc_notch_filter_1_pole *d,
float *buf)
{
    unsigned int buffer_size = d->buffer_size,
                 blk_cnt = buffer_size >> 2U;
    float        b0 = d->b[0],
                 b1 = d->b[1],
                 a1 = d->a1,
                 d1 = d->d_1,
                 p0,p1,p3,
                 A1,
                 xn1,xn2,xn3,xn4,
                 acc1,acc2,acc3,acc4;
    while (blk_cnt-- > 0) {
        /*
        Direct form II transposed:
        y[n] + y[n-1]*a1 = x[n]*b0 + x[n-1]*b1 ->
        y[n] = x[n]*b0 + x[n-1]*b1 - y[n-1]*a1 ->
        y[n] = x[n]*b0 + d_1
        */
         xn1 = buf[0];
         xn2 = buf[1];
         xn3 = buf[2];
         xn4 = buf[3];

         p0 = b0 * xn1;
         p1 = b1 * xn1;
         acc1 = p0 + d1;
         p0 = b0 * xn2;
         p3 = a1 * acc1;
         A1 = p1 - p3;
         d1 = A1;

         p1 = b1 * xn2;
         acc2 = p0 + d1;
         p0 = b0 * xn3;
         p3 = a1 * acc2;
         A1 = p1 - p3;
         d1 = A1;

         p1 = b1 * xn3;
         acc3 = p0 + d1;
         p0 = b0 * xn4;
         p3 = a1 * acc3;
         A1 = p1 - p3;
         d1 = A1;

         acc4 = p0 + d1;
         p1 = b1 * xn4;
         p3 = a1 * acc4;
         A1 = p1 - p3;
         d1 = A1;

         buf[0] = acc1;
         buf[1] = acc2;
         buf[2] = acc3;
         buf[3] = acc4;
		 buf += 4;
    }
    blk_cnt = buffer_size & 3U;
    while (blk_cnt-- > 0) {
         xn1 = *buf;

         p0 = b0 * xn1;
         p1 = b1 * xn1;
         acc1 = p0 + d1;
         p3 = a1 * acc1;
         A1 = p1 - p3;
         d1 = A1;

         *buf++ = acc1;
    }
    d->d_1 = d1;
}

/* Unrolled implementation of the filtering operation */
__attribute__((optimize("-O3")))
void
dc_notch_filter_1_pole_tick_unrolled8(
struct dc_notch_filter_1_pole *d,
float *buf)
{
    unsigned int buffer_size = d->buffer_size,
                 blk_cnt = buffer_size >> 3U;
    float        b0 = d->b[0],
                 b1 = d->b[1],
                 a1 = d->a1,
                 d_1 = d->d_1,
                 xn0,xn1,xn2,xn3,xn4,xn5,xn6,xn7,
                 yn0,yn1,yn2,yn3,yn4,yn5,yn6,yn7;
    while (blk_cnt-- > 0) {
        /*
        Direct form II transposed:
        y[n] + y[n-1]*a1 = x[n]*b0 + x[n-1]*b1 ->
        y[n] = x[n]*b0 + x[n-1]*b1 - y[n-1]*a1 ->
        y[n] = x[n]*b0 + d_1
        */
        xn0 = *buf;
        xn1 = *(buf+1);
        xn2 = *(buf+2);
        xn3 = *(buf+3);
        xn4 = *(buf+4);
        xn5 = *(buf+5);
        xn6 = *(buf+6);
        xn7 = *(buf+7);

        yn0 = xn0 * b0;
        yn0 += d_1;
        d_1 = b1 * xn0;
        d_1 -= a1 * yn0;

        yn1 = xn1 * b0;
        yn1 += d_1;
        d_1 = b1 * xn1;
        d_1 -= a1 * yn1;

        yn2 = xn2 * b0;
        yn2 += d_1;
        d_1 = b1 * xn2;
        d_1 -= a1 * yn2;

        yn3 = xn3 * b0;
        yn3 += d_1;
        d_1 = b1 * xn3;
        d_1 -= a1 * yn3;

        yn4 = xn4 * b0;
        yn4 += d_1;
        d_1 = b1 * xn4;
        d_1 -= a1 * yn4;

        yn5 = xn5 * b0;
        yn5 += d_1;
        d_1 = b1 * xn5;
        d_1 -= a1 * yn5;

        yn6 = xn6 * b0;
        yn6 += d_1;
        d_1 = b1 * xn6;
        d_1 -= a1 * yn6;

        yn7 = xn7 * b0;
        yn7 += d_1;
        d_1 = b1 * xn7;
        d_1 -= a1 * yn7;

        *buf = yn0;
        *(buf+1) = yn1;
        *(buf+2) = yn2;
        *(buf+3) = yn3;
        *(buf+4) = yn4;
        *(buf+5) = yn5;
        *(buf+6) = yn6;
        *(buf+7) = yn7;
        buf+=8;
    }
    blk_cnt = buffer_size & 7U;
    while (blk_cnt-- > 0) {
        xn0 = *buf;
        yn0 = xn0 * b0;
        yn0 += d_1;
        d_1 = b1 * xn0;
        d_1 -= a1 * yn0;
        *buf = yn0;
        buf++;
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
static float ans3[NVALS];

__attribute__((optimize("-O0")))
int main (void)
{
    float biggest_diff;
    unsigned int idx_biggest_diff;
    memcpy(ans1,x,sizeof(float)*NVALS);
    memcpy(ans2,x,sizeof(float)*NVALS);
    memcpy(ans3,x,sizeof(float)*NVALS);
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

        d = dc_notch_filter_1_pole_new(&di);
        assert(d);
        dc_notch_filter_1_pole_tick_unrolled4(d,ans2);
        assert(chk_correct(ans2,y,NVALS,&biggest_diff,&idx_biggest_diff) == 1);
        memcpy(ans2,x,sizeof(float)*NVALS);
        dc_notch_filter_1_pole_free(d);

        d = dc_notch_filter_1_pole_new(&di);
        assert(d);
        dc_notch_filter_1_pole_tick_unrolled4_cmsis(d,ans3);
        assert(chk_correct(ans3,y,NVALS,&biggest_diff,&idx_biggest_diff) == 1);
        memcpy(ans3,x,sizeof(float)*NVALS);
        dc_notch_filter_1_pole_free(d);
    }
    return 0;
}
