/* A limiter whose attenuation function's decay is described by an IIR filter.
*/

/* A filter whose IR is stored with it. */
struct filter_w_ir {
    float *b;
    float *a;
};

