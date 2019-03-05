#ifndef DC_NOTCH_FILTER_H
#define DC_NOTCH_FILTER_H 

struct dc_notch_filter_1_pole;

struct dc_notch_filter_1_pole_init {
    /* The "radius" of the pole. Needs to be between 0 and 1. Values closer to 1
    make a sharper notch. */
    float r;
    unsigned int buffer_size;
};

void
dc_notch_filter_1_pole_free(
struct dc_notch_filter_1_pole *d);

struct dc_notch_filter_1_pole *
dc_notch_filter_1_pole_new(
struct dc_notch_filter_1_pole_init *i);

void
dc_notch_filter_1_pole_tick(
struct dc_notch_filter_1_pole *d,
float *buf);

#endif /* DC_NOTCH_FILTER_H */
