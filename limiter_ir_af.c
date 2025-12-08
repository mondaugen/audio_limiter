/*
A limiter whose attenuation function's decay is described by an IIR filter.
*/

/* in order to enable the definition of qsort_r */
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "limiter_ir_af.h"
#include "float_buf.h"
#include "ringbuffer.h"

#ifndef MAX
#define MAX(x,y) (((x)>(y))?(x):(y))
#endif

/* computes y[n] += x[n] * a */
extern void
acc_with_scale(float *y,const float *x, float a, unsigned int len);
/* compute y[n+m] *= 1 - x[n] for n in frames and m in channels */
void
mul_one_minus_vec(void *y, float *x, unsigned int len, unsigned int n_channels);
/* clamp values to upper bound */
extern void clamp_ab( float *seg, unsigned int len, void *aux);
/* return the maximum absolute value in the array (infinity norm) */
extern float vf32_max_abs(float *vf, unsigned int len);
/* return 1 if the max absolute value in array a is greater than the max absolute value in array b (comparison of infinity norms) */
extern int vf32_max_abs_a_gt_max_abs_b(const float *a, const float *b, unsigned int len);

static unsigned int
argmax(const float *array, unsigned int len)
{
    float max = array[0];
    unsigned int maxi = 0,
                 n = 1;
    while (n < len) {
        if (array[n] > max) {
            maxi = n;
            max = array[n];
        }
        n++;
    }
    return maxi;
}

struct filter_w_ir {
    struct filter_w_ir_init config;
    /* The index of the maxmimum value in the stored IR */
    unsigned int ir_argmax;
    unsigned int filter_order;
    struct filter_w_ir_filter_imp filter;
};

static struct filter_w_ir_filter_imp *
filter_w_ir_filter_imp_ptr(struct filter_w_ir *f)
{
    return &f->filter;
}

void
filter_w_ir_free(struct filter_w_ir *f)
{
    if (!f) { return; }
    if (f->filter.free_filter_aux) {
        f->filter.free_filter_aux(f->filter.filter_aux);
    }
    free(f);
}

static int
filter_w_ir_arg_chk(const struct filter_w_ir_init * i)
{
    if ((i->len_b > 0) && (i->b == NULL)) { return -1; }
    if ((i->len_a > 0) && (i->a == NULL)) { return -2; }
    if ((i->len_ir > 0) && (i->ir == NULL)) { return -3; }
    return 0;
}

struct filter_w_ir *
filter_w_ir_new(const struct filter_w_ir_init *fwirinit)
{
    if (filter_w_ir_arg_chk(fwirinit) != 0) { return NULL; }
    struct filter_w_ir *ret = calloc(1,sizeof(struct filter_w_ir));
    if (!ret) { return NULL; }
    ret->config = *fwirinit;
    if (filter_w_ir_init_filter(&ret->filter,fwirinit) != 0) {
        free(ret);
        return NULL;
    }
    ret->ir_argmax = argmax(fwirinit->ir,fwirinit->len_ir);
    ret->filter_order = MAX(fwirinit->len_b,fwirinit->len_a);
    return ret;
}

struct limiter_ir_af {
    struct limiter_ir_af_init config;
    unsigned int ramp_up;
    unsigned int lookahead_buf_size;
    /* a buffer_size worth of zeros, used as the input to the attenuation
    function's filter. */
    float *zeros;
    /* Where the filter's past_x values are stored so we can force the filter to
    have them when it filters */
    float *past_x;
    /* Where the filter's past_y values are stored */
    float *past_y;
    struct rngbuf    *lookahead_buf;
    struct float_buf *attenuation_buf;
};

static int
limiter_ir_af_arg_chk(const struct limiter_ir_af_init *i)
{
    if (i->buffer_size < 1) { return -1; }
    if (i->fwir == NULL) { return -2; }
    if (i->threshold < 0) { return -3; }
    if (i->fwir->ir_argmax < 1) { return -4; }
    if ((i->fwir->ir_argmax + i->buffer_size) > i->fwir->config.len_ir) { return -5; }
    if ((i->fwir->config.ir[i->fwir->ir_argmax] != 1.)) { return -6; }
    if (i->n_channels<1) { return -7; }
    return 0;
}

static unsigned int
lia_buffer_size(struct limiter_ir_af *lia)
{
    return lia->config.buffer_size;
}

void
limiter_ir_af_free(struct limiter_ir_af *lia)
{
    if (!lia) { return; }
    if (lia->lookahead_buf) { rngbuf_free(lia->lookahead_buf); }
    if (lia->attenuation_buf) { float_buf_free(lia->attenuation_buf); }
    free(lia);
}

static int
push_inital_zeros_to_buf(struct rngbuf *rb, unsigned int n)
{
    unsigned char zero[rngbuf_item_size(rb)*n];
    memset(zero,0,rngbuf_item_size(rb)*n);
    int ret;
    return rngbuf_push_copy(rb, zero, n);
}

struct limiter_ir_af *
limiter_ir_af_new(struct limiter_ir_af_init *i)
{
    if (limiter_ir_af_arg_chk(i)) { return NULL; }
    struct limiter_ir_af *ret = NULL;
    unsigned int ramp_up = i->fwir->ir_argmax,
                 lookahead_buf_size = i->buffer_size + ramp_up; 
    ret = calloc(
        1,
        sizeof(struct limiter_ir_af)
            + sizeof(float)*(i->buffer_size
                + 2*i->fwir->filter_order));
    if (!ret) { goto fail; }
    ret->zeros = (float*)(ret+1);
    ret->past_x = ret->zeros + i->buffer_size;
    ret->past_y = ret->past_x + i->fwir->filter_order;
    ret->lookahead_buf = rngbuf_new(lookahead_buf_size,sizeof(float)*i->n_channels);
    if (!ret->lookahead_buf) { goto fail; }
    ret->attenuation_buf = float_buf_new(lookahead_buf_size);
    if (!ret->attenuation_buf) { goto fail; }
    if (push_inital_zeros_to_buf(ret->lookahead_buf,lookahead_buf_size) != 0) {
        goto fail;
    }
    if (push_inital_zeros_to_buf((struct rngbuf*)ret->attenuation_buf,lookahead_buf_size) != 0) {
        goto fail;
    }
    ret->config = *i;
    ret->ramp_up = ramp_up;
    ret->lookahead_buf_size = lookahead_buf_size;
    return ret;
fail:
    limiter_ir_af_free(ret);
    return NULL;
}

static unsigned int
lia_filter_order(struct limiter_ir_af *lia)
{
    return lia->config.fwir->filter_order;
}

static const float *
lia_ir(struct limiter_ir_af *lia)
{
    return lia->config.fwir->config.ir;
}

struct fb_filter_region_aux {
    struct filter_w_ir_filter_imp *f_imp;
    float *input_vals;
    float *past_x;
    float *past_y;
};

static void
fb_filter_region(
    float *seg,
    unsigned int len,
    void *aux)
{
    struct fb_filter_region_aux *ffrb = aux;
    (void)filter_w_ir_filter_imp_tick(
        ffrb->f_imp,
        ffrb->input_vals,
        seg,
        len,
        ffrb->past_x,
        ffrb->past_y);
    ffrb->input_vals += len;
}

struct atn_fun_updater_aux { struct limiter_ir_af *lia; };

/* Sort descending */
static int
fval_where_val_cmp_dec(const void *a_, const void *b_, void *aux)
{
    struct atn_fun_updater_aux *afu = aux;
    const struct rngbuf_where_val *a = a_, *b = b_;
    return vf32_max_abs_a_gt_max_abs_b(b->item,
                                       a->item,
                                       afu->lia->config.n_channels)
            - vf32_max_abs_a_gt_max_abs_b(a->item,
                                          b->item,
                                       afu->lia->config.n_channels);
}

struct sum_ir_into_atn_buf_aux {
    const float *ir;
    float scale;
};

static void
sum_ir_into_atn_buf(
    float *seg,
    unsigned int len,
    void *aux_)
{
    struct sum_ir_into_atn_buf_aux *aux = aux_;
    acc_with_scale(seg,aux->ir,aux->scale,len);
    /* Advance IR pointer so next region multiplied with the rest of the IR */
    aux->ir += len;
}

/* passed as chk argument to rngbuf_where_values */
static int
la_buf_peak_finder(struct rngbuf *rb,
void *item,
void *aux_)
{
    struct atn_fun_updater_aux *aux = aux_;
    return vf32_max_abs(item, aux->lia->config.n_channels) > aux->lia->config.threshold;
}

/* passed as fun argument to rngbuf_where_values */
static void
atn_fun_updater(struct rngbuf *rb,
                struct rngbuf_where_val *vals,
                unsigned int nvals,
                void *aux_)
{
    struct atn_fun_updater_aux *aux = aux_;
    /* Sort values in descending order of magnitude */
    qsort_r(vals,nvals,sizeof(struct rngbuf_where_val),fval_where_val_cmp_dec,aux);
    unsigned int n;
    for (n = 0; n < nvals; n++) {
        float atn_amt = 1. - aux->lia->config.threshold / vf32_max_abs(vals[n].item,aux->lia->config.n_channels),
              cur_atn;
        /* TODO DEBUG trap here? */
        float_buf_lookup(aux->lia->attenuation_buf, vals[n].n, &cur_atn);
        if (cur_atn >= atn_amt) {
            /* no need to attenuate, we're attenuating enough already */
            continue;
        }
        atn_amt = atn_amt - cur_atn;
        struct sum_ir_into_atn_buf_aux aux2 = {
            .ir = lia_ir(aux->lia),
            .scale = atn_amt
        };
        float_buf_process_region(
            aux->lia->attenuation_buf,
            vals[n].n - aux->lia->ramp_up,
            aux->lia->lookahead_buf_size - vals[n].n + aux->lia->ramp_up,
            sum_ir_into_atn_buf,
            &aux2);
    }
}

struct scale_out_buf_aux { void *out_buf; unsigned int n_channels; };

static void
scale_out_buf(
    float *seg,
    unsigned int len,
    void *aux_)
{
    struct scale_out_buf_aux *aux = aux_;
    mul_one_minus_vec(aux->out_buf,seg,len,aux->n_channels);
    /* Advance out_buf pointer so that we multiply the rest when calling on the
    next region.  */
    aux->out_buf += len*aux->n_channels;
} 

/*
Limits x according to the configuration of lia.
There will be an output delay of lia->ramp_up.
x is processed in place.
*/
int
limiter_ir_af_tick(struct limiter_ir_af *lia, float *x)
{
    /* Shift in samples from x */
    rngbuf_shift_in(lia->lookahead_buf,x,lia_buffer_size(lia));
    /*
    Shift in the attenuation function by computing the IR values based on the
    last known values of the attenuation function
    */
    memset(lia->past_x,0,sizeof(float)*lia_filter_order(lia));
    /* Shift over attenuation function */
    float_buf_shift_in(lia->attenuation_buf,lia->zeros,lia_buffer_size(lia));
    float_buf_memcpy(
        lia->attenuation_buf,
        lia->ramp_up-lia_filter_order(lia),
        lia_filter_order(lia),
        lia->past_y);
    struct fb_filter_region_aux filter_region_aux = {
        .f_imp = &lia->config.fwir->filter,
        .f_imp = filter_w_ir_filter_imp_ptr(lia->config.fwir),
        .input_vals = lia->zeros,
        .past_x = lia->past_x,
        .past_y = lia->past_y,
    };
    /* Compute the initial attenuation function by extending the IR of the filter */
    float_buf_process_region(
        lia->attenuation_buf,
        lia->ramp_up,
        lia_buffer_size(lia),
        fb_filter_region,
        &filter_region_aux);
    /*
    update attenuation function according to peak values in the future
    */
    struct atn_fun_updater_aux aux = { .lia = lia };
    rngbuf_where_values(
        lia->lookahead_buf,
        lia->ramp_up,
        lia_buffer_size(lia),
        la_buf_peak_finder,
        atn_fun_updater,
        &aux);
    /* Clamp attenuation buffer to 1 maximum (it should bever be negative) */
    struct clamp_ab_aux aux_clamp_ab = { .clamp_val = 1 };
    float_buf_process_region(
        lia->attenuation_buf,
        0,
        lia_buffer_size(lia),
        clamp_ab,
        &aux_clamp_ab);
    /*
    Multiply the lookahead buffer by the attenuation function.  What we actually
    do is extract the lookahead buffer to x (that will contain the output).
    Then we do the multiplication in place in x, that way we don't have to deal
    with mis-aligned lookahead_buf and attenuation_buf (this would make their
    multiplication complicated).
    */
    rngbuf_memcpy(
        lia->lookahead_buf,
        0,
        lia_buffer_size(lia),
        x);
    struct scale_out_buf_aux sob_aux = { .out_buf = x, .n_channels = lia->config.n_channels };
    float_buf_process_region(
        lia->attenuation_buf,
        0,
        lia_buffer_size(lia),
        scale_out_buf,
        &sob_aux);
    return 0;
} 
