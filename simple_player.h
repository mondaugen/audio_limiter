#ifndef SIMPLE_PLAYER_H
#define SIMPLE_PLAYER_H 

#include "audio_processor.h"

struct simple_player_params {
    int buffer_size;
    float *samples;
    int n_samples;
};

audio_processor *
simple_player_new(struct simple_player_params *params);

#endif /* SIMPLE_PLAYER_H */
