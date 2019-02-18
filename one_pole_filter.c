#include <stdlib.h>

/* A single pole, real filter */
struct one_pole_filter {
    /* The only feedback coefficient. */
    float a;
};

void
one_pole_filter_free(struct one_pole_filter *opf)
{
    free(opf);
}

struct one_pole_filter *
one_pole_filter_new(struct one_pole_filter_init *config)
{
    struct one_pole_filter *ret = calloc(1,sizeof(struct one_pole_filter));
    if (!ret) { return NULL; }
    ret->a = config->a;
    return ret;
}

int
one_pole_filter_tick(
    struct one_pole_filter *opf,
    float *x,
    float *y,
    unsigned int len,
    float *y_prev)
{
    while (len-- > 0) {
        *y_prev = *y++ = *x++ + opf->a * *y_prev;
    }
    return 0;
}
