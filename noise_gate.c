/*
A simple noise gate.
When the average signal value goes below a threshold, the gain is ramped to 0.
When it exceeds this threshold, and a gain < 1 was already being applied, it is
ramped to 1.
*/

#include <stdlib.h>
#include <math.h>
#include "noise_gate.h"
#include "float_buf.h"

/* compute x[n] *= y[n] for n = 0 to N-1 */
extern void
mul_f32_vecs(float *x, const float *y, unsigned int N);

struct noise_gate {
    struct noise_gate_init config;
    float avg_amp;
    float ramp_inc;
    float cur_ramp_inc;
    float sum_err;
    float ramp;
    /* A buffer of size buffer_size + config.lookahead so the signal can be
    stored and delayed by lookahead. */
    struct float_buf *lookahead_buf;
    /* A buffer storing the computed ramp values by looking in the lookahead
    buffer but making a ramp that will be applied to the current buffer. */
    float *ramp_buf;
};

void
noise_gate_free(struct noise_gate *ng)
{
    if (!ng) { return; }
    if (ng->lookahead_buf) { float_buf_free(ng->lookahead_buf); }
    if (ng->ramp_buf) { free(ng->ramp_buf); }
    free(ng);
}

int
ng_chk_args(const struct noise_gate_init *init)
{
    /* noise threshold only meaningful if non-negative */
    if (init->noise_thresh < 0) { return -1; }
    /* averaging coefficients outside (0,1] are not valid and if avg_alpha were
    0, the average would stay at 0, so this is also disallowed */
    if ((init->avg_alpha > 1) || (init->avg_alpha <= 0)) { return -2; }
    /* negative ramp times invalid, and check for underflow */
    if ((init->ramp_time <= 0) || (1./init->ramp_time == 0)) { return -3; }
    return 0;
}

struct noise_gate *
noise_gate_new(const struct noise_gate_init *init)
{
    int err, n;
    if ((err = ng_chk_args(init))) { goto fail; }
    struct noise_gate *ret = calloc(1,sizeof(struct noise_gate));
    if (!ret) { return NULL; }
    ret->lookahead_buf = float_buf_new(init->buffer_size + init->lookahead);
    if (!ret->lookahead_buf) { goto fail; }
    ret->ramp_buf = malloc(sizeof(float)*init->buffer_size);
    if (!ret->ramp_buf) { goto fail; }
    /* Push in values so that incoming signal is delayed by "lookahead" */
    float initval = 0;
    for (n = 0; n < (init->buffer_size + init->lookahead); n++) {
        float_buf_push_copy(ret->lookahead_buf,1,&initval);
    }
    ret->config = *init;
    ret->ramp_inc = 1./init->ramp_time;
    ret->ramp = 1;
    /* all other fields initialized to 0 */
    return ret;
fail:
    noise_gate_free(ret);
    return NULL;
}

static unsigned int
ng_buffer_size(struct noise_gate *ng)
{
    return ng->config.buffer_size;
}

struct ng_compute_ramp_aux {
    struct noise_gate *ng;
    float *ramp_buf_ptr;
};

static void
ng_compute_ramp(float *seg, unsigned int len, void *aux_)
{
    struct ng_compute_ramp_aux *aux = aux_;
    const float noise_thresh = aux->ng->config.noise_thresh,
                avg_alpha = aux->ng->config.avg_alpha,
                comp_avg_alpha = 1 - avg_alpha,
                ramp_inc = aux->ng->ramp_inc;
    float avg_amp = aux->ng->avg_amp,
          cur_ramp_inc = aux->ng->cur_ramp_inc,
          sum_err = aux->ng->sum_err,
          ramp = aux->ng->ramp,
          *ramp_buf_ptr = aux->ramp_buf_ptr,
          tmp, inc;
    unsigned int lookahead = aux->ng->config.lookahead, n;
    for (n = 0; n < len; n++) {
        avg_amp = fabsf(*seg++)*avg_alpha + comp_avg_alpha * avg_amp;
        if ((avg_amp < noise_thresh) && (ramp == 1)) {
            /* ramp should decrease in value */
            cur_ramp_inc = -ramp_inc;
        } else if ((avg_amp >= noise_thresh) && (ramp < 1)) {
            /* ramp should increase in value */
            cur_ramp_inc = ramp_inc;
        }
        /* increment with compensation */
        tmp = ramp;
        inc = cur_ramp_inc + sum_err;
        ramp = tmp + inc;
        sum_err = (tmp - ramp) + inc;
        if (ramp <= 0) { cur_ramp_inc = 0; ramp = 0; }
        if (ramp >= 1) { cur_ramp_inc = 0; ramp = 1; }
        *ramp_buf_ptr = ramp;
        ramp_buf_ptr++;
    }
    aux->ng->avg_amp = avg_amp;
    aux->ng->cur_ramp_inc = cur_ramp_inc;
    aux->ng->sum_err = sum_err;
    aux->ng->ramp = ramp;
    aux->ramp_buf_ptr = ramp_buf_ptr;
}

/* 
Applies a noise gate to the contents of x.
There will be an output delay of ng->lookahead.
x is processed in place.
*/
int
noise_gate_tick(struct noise_gate *ng, float *x)
{
    /* shift in samples from x */
    float_buf_shift_in(ng->lookahead_buf,x,ng_buffer_size(ng));
    /* compute the ramp from the contents of the lookahead buffer */
    struct ng_compute_ramp_aux aux1 = { .ng = ng, .ramp_buf_ptr = ng->ramp_buf };
    float_buf_process_region(
        ng->lookahead_buf,
        0,
        ng_buffer_size(ng),
        ng_compute_ramp,
        &aux1);
    float_buf_memcpy(
        ng->lookahead_buf,
        0,
        ng_buffer_size(ng),
        x);
    mul_f32_vecs(x,ng->ramp_buf,ng_buffer_size(ng));
    return 0;
}
