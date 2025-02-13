/* Low level limiter routines, native implementation */
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "limiter_ir_af.h"

/* For now this is just the same as ARM because we don't do anything specific to ARM. */

/* computes y[n] += x[n] * a */
__attribute__((optimize("-O3")))
void
acc_with_scale(
float *y,
const float *x,
float a,
unsigned int len)
{
    while (len-- > 0) {
        *y++ += *x++ * a;
    }
}

/* compute y[n] *= 1 - x[n] */
__attribute__((optimize("-O3")))
void
mul_one_minus_vec(void *_y, float *x, unsigned int len, unsigned int n_channels)
{
    float *y=_y;
    while (len-- > 0) {
        unsigned int n;
        for (n=0;n<n_channels;n++){
            *y++ *= 1 - *x;
        }
        x++;
    }
}

/* clamp values to upper bound */
extern void clamp_ab(
float *seg, unsigned int len, void *aux_)
{
    struct clamp_ab_aux *aux = aux_;
    float clamp_val = aux->clamp_val;
    while (len-- > 0) {
        *seg = *seg > clamp_val ? clamp_val : *seg;
        seg++;
    }
}

static inline float
max_f32(float a, float b)
{
    if (a > b) return a;
    return b;
}

float vf32_max_abs(float *vf, unsigned int len)
{
    switch (len) {
        case 1:
            return fabs(*vf);
        case 2:
            return max_f32(fabs(vf[0]),fabs(vf[1]));
        default:
    }            
    /* if len is 0, results undefined */
    float the_max = fabs(*vf);
    vf++;
    while (len > 1) {
        if (fabs(*vf) > the_max) { the_max = fabs(*vf); }
        len--;
        vf++;
    }
    return the_max;
}

int vf32_max_abs_a_gt_max_abs_b(float *a, float *b, unsigned int len)
{
    switch (len) {
        case 0:
            return 0;
        case 1:
            return fabs(*a) > fabs(*b);
        case 2:
            return max_f32(fabs(a[0]),fabs(a[1])) > max_f32(fabs(b[0]),fabs(b[1]));
        default:
            return vf32_max_abs(a,len) > vf32_max_abs(b,len);
    }
}
