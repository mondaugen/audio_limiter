/* Low level limiter routines, native implementation */
#include <stdint.h>
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
mul_one_minus_vec(float *y, float *x, unsigned int len)
{
    while (len-- > 0) {
        *y++ *= 1 - *x++;
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
