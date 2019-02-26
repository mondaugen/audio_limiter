/*
The limiter uses a special kind of one-pole filter to extend the attenuation curve.
It is a one-pole filter with no input (we just supply the previous value).
Also, its coefficients never change. So in addition to unrolling, we want to try
precomputing powers of the a1 coefficient and using those instead of recursively
computing.
*/

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "one_pole_filter.h"
#include "filter_exp_test_data.h"

__attribute__((optimize("-O3")))
void
one_pole_unroll_4(
/* output */
float *y,
/* the previous delay line value, on exit contains the last delay line value */
float *pd1,
/* the a1 coefficient */
float a1,
/* the length of y */
uint32_t len)
{
    /* temporary variables */
    float d1 = *pd1, y0, y1, y2, y3;
    uint32_t blk_cnt = len >> 2U;
    while (blk_cnt > 0) {
        y0 = d1;
        d1 = y0 * a1;
        y1 = d1;
        d1 = y1 * a1;
        y2 = d1;
        d1 = y2 * a1;
        y3 = d1;
        d1 = y3 * a1;
        *(y) = y0;
        *(y+1) = y1;
        *(y+2) = y2;
        *(y+3) = y3;
        y += 4;
        blk_cnt--;
    }
    blk_cnt = len & (4U - 1);
    while (blk_cnt > 0) {
        y0 = d1;
        d1 = y0 * a1;
        *y = y0;
        y++;
        blk_cnt--;
    }
    *pd1 = d1;
}

/* This guy's the fastest */
__attribute__((optimize("-O3")))
void
one_pole_unroll_8(
/* output */
float *y,
/* the previous delay line value, on exit contains the last delay line value */
float *pd1,
/* the a1 coefficient */
float a1,
/* the length of y */
uint32_t len)
{
    /* temporary variables */
    float d1 = *pd1, y0, y1, y2, y3, y4, y5, y6, y7;
    uint32_t blk_cnt = len >> 3U;
    while (blk_cnt > 0) {
        y0 = d1;
        d1 = y0 * a1;
        y1 = d1;
        d1 = y1 * a1;
        y2 = d1;
        d1 = y2 * a1;
        y3 = d1;
        d1 = y3 * a1;
        y4 = d1;
        d1 = y4 * a1;
        y5 = d1;
        d1 = y5 * a1;
        y6 = d1;
        d1 = y6 * a1;
        y7 = d1;
        d1 = y7 * a1;
        *(y) = y0;
        *(y+1) = y1;
        *(y+2) = y2;
        *(y+3) = y3;
        *(y+4) = y4;
        *(y+5) = y5;
        *(y+6) = y6;
        *(y+7) = y7;
        y += 8;
        blk_cnt--;
    }
    blk_cnt = len & (8U - 1);
    while (blk_cnt > 0) {
        y0 = d1;
        d1 = y0 * a1;
        *y = y0;
        y++;
        blk_cnt--;
    }
    *pd1 = d1;
}


/*
Like one_pole_unroll_4 but we use pecomputed powers of a1.
UPDATE: This is not any faster than one_pole_unroll_4.
*/
__attribute__((optimize("-O3")))
void
one_pole_unroll_4_powers(
/* output */
float *y,
/* the previous delay line value, on exit contains the last delay line value */
float *pd1,
/* the precomputed a coefficients, {a^1,a^2,a^3,a^4} */
float *a,
/* the length of y */
uint32_t len)
{
    /* temporary variables */
    float d1 = *pd1, y0, y1, y2, y3,
          a1_1 = a[0], a1_2 = a[1], a1_3 = a[2], a1_4 = a[3];
    uint32_t blk_cnt = len >> 2U;
    while (blk_cnt > 0) {
        y0 = d1;
        y1 = d1 * a1_1;
        y2 = d1 * a1_2;
        y3 = d1 * a1_3;
        d1 = d1 * a1_4;
        *(y) = y0;
        *(y+1) = y1;
        *(y+2) = y2;
        *(y+3) = y3;
        y += 4;
        blk_cnt--;
    }
    blk_cnt = len & (2U - 1);
    while (blk_cnt > 0) {
        y0 = d1;
        d1 = y0 * a1_1;
        *y = y0;
        y++;
        blk_cnt--;
    }
    *pd1 = d1;
}

#define TOL 1e-6

int chk_correct(float *a, float *b, uint32_t len)
{
    int correct = 1;
    while ((len-- > 0) && (correct != 0)) {
        correct &= fabs(*a++ - *b++) < TOL;
    }
    return correct;
}

int main (void)
{
    float ans1[LEN_YN_TRUE],
          ans2[LEN_YN_TRUE],
          ans3[LEN_YN_TRUE],
          x[LEN_YN_TRUE],
          d1 = 1,
          a1=0.99,
          a[] = {a1,a1*a1,a1*a1*a1,a1*a1*a1*a1};
    struct one_pole_filter_init opf_init = { .a = a1 };
    struct one_pole_filter *opf = one_pole_filter_new(&opf_init);
    memset(x,0,sizeof(float)*LEN_YN_TRUE);
    while (1) {
        one_pole_filter_tick(opf,x,ans3,LEN_YN_TRUE,&d1);
        //assert(chk_correct(ans3,yn_true,LEN_YN_TRUE) > 0);
        d1 = 1;
        one_pole_unroll_4(ans1,&d1,a1,LEN_YN_TRUE);
        assert(chk_correct(ans1,yn_true,LEN_YN_TRUE) > 0);
        d1 = 1;
        one_pole_unroll_8(ans2,&d1,a1,LEN_YN_TRUE);
        assert(chk_correct(ans2,yn_true,LEN_YN_TRUE) > 0);
        d1 = 1;
    }
    return 0;
}
