#ifndef LIMITER_H
#define LIMITER_H 

#include "audio_processor.h"

struct audio_limiter_params {
    /* (all lengths in samples) */
    /* Length of lookahead */
    int Nl;
    /* Length of attack ramp */
    int Nr;
    /* Length of decay ramp */
    int Nd;
    /* The number of samples processed each tick (buffer size) */
    int Nb;
};

/* Get a new audio_limiter, according to params. Returns NULL on error. */
audio_processor *
audio_limiter_new(struct audio_limiter_params *params);

#endif /* LIMITER_H */
