/*
Implementation of the limiter's filter optimized a bit for ARM cortex-m4.
Doesn't use external libraries.
A weird kind of 1 pole filter:
It doesn't consider input values, only the last delay line value.
Its coefficients cannot change after filter initialization.
*/
#include "limiter_ir_af.h"

/* This guy's the fastest */
__attribute__((optimize("-O3")))
static void
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
    filter->filter_aux = (void*)op
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
    int ret;
    one_pole_unroll_8(y,y_prev,f->a,len);
    return 0;
}
