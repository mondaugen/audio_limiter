#ifndef LIMITER_IR_AF_H
#define LIMITER_IR_AF_H 

/* A filter with a part of its IR stored with it. */
struct filter_w_ir_init {
    /* The feedforward coefficients */
    const float *b;
    /* The number of feedforward coefficients */
    unsigned int len_b;
    /* The feedback coefficients, these are a1,a2,... a0 is assumed to be 1 */
    const float *a;
    /* The number of feedback coefficients */
    unsigned int len_a;
    /* The precomputed IR */
    const float *ir;
    /* The number of samples of the IR that have been computed */
    unsigned int len_ir;
};

/* Auxilary structure for filter */
struct filter_w_ir_filter_imp {
    /* Stuff the filtering function might need (e.g., past filter states) */
    void *filter_aux;
    /* How to free this, can be NULL, in which case it is not called. */
    void (*free_filter_aux)(void *);
};

struct limiter_ir_af_init {
    /* The implementation of the filter with the IR stored. */
    struct filter_w_ir *fwir;
    /* The number of samples that are computed each call to the
    limiter_ir_af_tick function */
    unsigned int buffer_size;
    /* The threshold over which absolute signal values are limited */
    float threshold;
};

struct clamp_ab_aux {
    float clamp_val;
};

/*
Initialize the filtering routines.
Returns non-zero on error, 0 otherwise.
This needs to be defined somewhere in the application. It might be defined
differently depending on the DSP library used.
*/
int
filter_w_ir_init_filter(
    struct filter_w_ir_filter_imp *filter,
    const struct filter_w_ir_init *init);

/*
Filter x and put output in y. x and y should not overlap and should have
length at least buffer_size.
x_prev is {x[-M],x[-M+1],...,x[0]} where M is order of the filter.
y_prev is similar.
This needs to be defined somewhere and is what actually carries out the
filtering.
It needs to return non-zero on error.
After returning, x_prev and y_prev should contain the most current filter state.
*/
int
filter_w_ir_filter_imp_tick(
    struct filter_w_ir_filter_imp *f_imp,
    float *x,
    float *y,
    unsigned int len,
    float *x_prev,
    float *y_prev);

struct limiter_ir_af *
limiter_ir_af_new(struct limiter_ir_af_init *i);

void
limiter_ir_af_free(struct limiter_ir_af *lia);

int
limiter_ir_af_tick(struct limiter_ir_af *lia, float *x);

struct filter_w_ir *
filter_w_ir_new(const struct filter_w_ir_init *fwirinit);

#endif /* LIMITER_IR_AF_H */
