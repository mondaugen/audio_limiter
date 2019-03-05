#ifndef DC_NOTCH_FILTER_PRIVATE_H
#define DC_NOTCH_FILTER_PRIVATE_H 

struct dc_notch_filter_1_pole {
    /* the a1 coefficient, we assume a0 is 1 */
    float a1;
    /* The b0 and b1 coefficient */
    float b[2];
    /* the previous delay line values */
    float d_1;
    /* the number of values this can process */
    unsigned int buffer_size;
};

#endif /* DC_NOTCH_FILTER_PRIVATE_H */
