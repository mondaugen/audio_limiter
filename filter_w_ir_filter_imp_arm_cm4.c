/*
Implementation of the limiter's filter optimized a bit for ARM cortex-m4.
Doesn't use external libraries.
A weird kind of 1 pole filter:
It doesn't consider input values, only the last delay line value.
Its coefficients cannot change after filter initialization.
*/
#include <stdlib.h>
#include <stdint.h>
#include "limiter_ir_af.h"

/*
This is actually the fastest. The "Duff's device" at the end is really
overkill, you gain 4 cycles in the worst case over just having a while loop, but
I've already written it, so why not use it?
*/
__attribute__((optimize("-O3")))
void
one_pole_unroll_4_duff(
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
    switch (blk_cnt) {
    case 3:
            y0 = d1;
            d1 = y0 * a1;
            *y = y0;
            y++;
    case 2:
            y0 = d1;
            d1 = y0 * a1;
            *y = y0;
            y++;
    case 1:
            y0 = d1;
            d1 = y0 * a1;
            *y = y0;
            y++;
    case 0:
            *pd1 = d1;
    }
}

struct one_pole { float a; };

static void
filter_free(void *f)
{
    free(f);
}

/*
Note that this just checks if it could make a filter from the arguments, but if
you've passed in more filter coefficients, you might be surprised at the
behaviour of the filter.
*/
static int
filter_w_ir_init_arg_chk(const struct filter_w_ir_init *args)
{
    if (!args) { return -1; }
    if (!args->a) { return -2; }
    if (args->len_a < 1) { return -3; }
    return 0;
}

int
filter_w_ir_init_filter(
    struct filter_w_ir_filter_imp *filter,
    const struct filter_w_ir_init *init)
{
    int ret;
    if ((ret = filter_w_ir_init_arg_chk(init)) != 0) { return ret; }
    struct one_pole *op = calloc(1,sizeof(struct one_pole));
    if (!op) { return -1; }
    op->a = init->a[0];
    filter->filter_aux = (void*)op;
    filter->free_filter_aux = filter_free;
    return 0;
}

/*
Note that this ignores x_prev because the implementation doesn't require it.
*/
int
filter_w_ir_filter_imp_tick(
    struct filter_w_ir_filter_imp *f_imp,
    float *x,
    float *y,
    unsigned int len,
    float *x_prev,
    float *y_prev)
{
    struct one_pole *f = f_imp->filter_aux;
    one_pole_unroll_4_duff(y,y_prev,f->a,len);
    return 0;
}
