/*
The native implementation of the limiter's filter. Doesn't use external
libraries. Just a one-pole filter.
*/
#include "limiter_ir_af.h"
#include "one_pole_filter.h"

static void
filter_free(void *f)
{
    one_pole_filter_free((struct one_pole_filter*)f);
}

/*
Note that this just checks if it could make a filter from the arguments, but if
you've passed in more filter coefficients, you might be surprised at the
behaviour of the filter.
*/
static int
filter_w_ir_init_arg_chk(struct filter_w_ir_init *args)
{
    if (!args) { return -1; }
    if (!args->a) { return -2; }
    if (args->len_a < 1) { return -3; }
    return 0;
}

int
filter_w_ir_init_filter(
    struct filter_w_ir_filter *filter,
    struct filter_w_ir_init *init)
{
    int ret;
    if ((ret = filter_w_ir_init_arg_chk(init)) != 0) { return ret; }
    struct one_pole_filter_init opfi = { .a = init->a[0] };
    filter->filter_aux = (void*)one_pole_filter_new(&opfi);
    if (!filter->filter_aux) { return -1; }
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
    struct one_pole_filter *f = f_imp->filter_aux;
    int ret;
    if ((ret = one_pole_filter_tick(f,x,y,len,y_prev)) != 0) { return ret; }
    return 0;
}
