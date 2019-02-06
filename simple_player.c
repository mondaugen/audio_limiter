#include <stdlib.h>
#include "simple_player.h"


/* Simply plays a buffer until the end after which it just returns 0s */

struct simple_player {
    audio_processor _interface;
    struct simple_player_params params;
    int play_ptr;
    float *buffer;
};

static int
check_params(struct simple_player_params *params)
{
    if (params->samples == NULL) { return -1; }
    if (params->n_samples <= 0) { return -2; }
    if (params->buffer_size <= 0) { return -2; }
    return 0;
}

static int
buffer_size(audio_processor *p)
{
    struct simple_player *sp = (struct simple_player*)p;
    return sp->params.buffer_size;
}
    
static float *
out_buffer(audio_processor *p)
{
    struct simple_player *l = (struct simple_player *)p;
    return l->buffer;
}

/* in can be NULL, this just produces samples */
static int
tick(
    audio_processor *p,
    float *in)
{
    struct simple_player *l = (struct simple_player *)p;
    int n;
    for (n = 0; n < l->params.buffer_size; n++) {
        if (l->play_ptr < l->params.n_samples) {
            l->buffer[n] = l->params.samples[l->play_ptr];
            l->play_ptr++;
        } else {
            l->buffer[n] = 0;
        }
    }
}

static void
simple_player_free(audio_processor *p)
{
    free(p);
}

struct audio_processor_interface simple_player_interface = {
    tick,
    buffer_size,
    buffer_size,
    out_buffer,
    simple_player_free
};

audio_processor *
simple_player_new(struct simple_player_params *params)
{
    if (check_params(params) != 0) { return NULL; }
    struct simple_player *ret = calloc(1,sizeof(struct simple_player)
        + sizeof(float) * params->buffer_size);
    ret->params = *params;
    ret->buffer = (float*)(ret+1);
    ret->_interface = &simple_player_interface;
    return (audio_processor *)ret;
}
