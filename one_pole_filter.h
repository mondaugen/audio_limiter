#ifndef ONE_POLE_FILTER_H
#define ONE_POLE_FILTER_H 

struct one_pole_filter_init { float a; };

void
one_pole_filter_free(struct one_pole_filter *opf);

struct one_pole_filter *
one_pole_filter_new(struct one_pole_filter_init *config);

int
one_pole_filter_tick(
    struct one_pole_filter *opf,
    float *x,
    float *y,
    unsigned int len,
    float *y_prev);

#endif /* ONE_POLE_FILTER_H */
