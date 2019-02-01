/* A limiter for audio. */

#include <stdlib.h>
#include <math.h>

#include "limiter.h"
#include "audio_processor.h"

struct audio_limiter {
    struct audio_processor _interface;
    /* Parameters for the audio limiter */
    struct audio_limiter_params params;
    /* Goal of attenuation ramp */
    float Gr;
    /* Increment of attenuation ramp */
    float Ir;
    /* Current attenuation ramp value */
    float Cr;
    /* The current threshold above which peaks are chosen to be attenuated. */
    float At;
    /* The increment of the threshold (could be called decay because it is
    always negative) */
    float It;
    /* The buffer holding the currently processed samples and the lookahead (has
    the size of Nb + Nla floats in memory) */
    float *x;
}

/* Returns non-zero if params are bad */
static int
check_params(
    struct audio_limiter_params *params)
{
    if ((params->Nl <= 0) ||
        (params->Nr <= 0) ||
        (params->Nd <= 0) ||
        (params->Nb <= 0)) {
        return -1;
    }
    if (params->Nr > params->Nl) { return -2; }
    return 0;
}

static int
buffer_size(audio_processor *p)
{
    struct audio_limiter *l = p;
    return l->Nb;
}

static float *
out_buffer(audio_processor *p)
{
    struct audio_limiter *l = p;
    return l->x;
}

static void
shift_in(struct audio_limiter *l, float *x)
{
    memmove(l->x,l->x+l->Nb,l->Nl);
    memcopy(l->x+l->Nl,x,l->Nb);
}

static int
tick(
    audio_processor *p,
    float *in)
{
    struct audio_limiter *l = p;
    int n;
    float x;
    /* Shift in l->Nb samples */
    shift_in(l,in);
    for (n = 0; n < l->Nb; n++) {
        x = fabsf(l->x[n+Nb]);
        if (x > l->At) {
            /* If the absolute value of the signal is greater than the
            threshold, start ramping to this attenuation value and set this to
            the new threshold. */
            l->Gr = x;
            l->Ir = (l->Gr - l->Cr) / l->Nr;
            l->At = l->Gr;
            l->It = (1 - l->At) / l->Nd;
        }
        /* Apply any attenuation */
        l->x[n] /= l->Cr;
        l->Cr += l->Ir;
        if ((l->Ir > 0) && (l->Cr >= l->Gr)) {
            /* We reached the attenuation goal from below, now we start ramping
            back down */
            l->Cr = l->Gr;
            l->Gr = 1;
            l->Ir = (1 - l->Cr) / l->Nd;
        } else if ((l->Ir < 0) && (l->Cr <= 1)) {
            /* We reached unity attenuation from above, stop the ramp (set increment
            to 0) */
            l->Cr = 1;
            l->Gr = 1;
            l->Ir = 0;
        }
        /* "Discount" (decrement) the threshold */
        l->At += l->It;
        if (l->At <= 1) {
            /* If the threshold is unity again, stop discounting */
            l->At = 1;
            l->It = 0;
        }
    }
    return 0;
}

static void
audio_limiter_free(audio_processor *p)
{
    free(p);
}

struct audio_processor audio_limiter_interface = {
    tick,
    buffer_size,
    buffer_size,
    out_buffer,
    free
};
    
/* Get a new audio_limiter, according to params. Returns NULL on error. */
audio_processor *
audio_limiter_new(
    struct audio_limiter_params *params)
{
    if (check_params(params) != 0) { return NULL; }
    struct audio_limiter *ret = calloc(1,sizeof(struct audio_limiter)+sizeof(float)*(params->Nb+params->Nl));
    if (!ret) { return NULL; }
    ret->params = *params;
    ret->Gr = 1;
    ret->Ir = 0;
    ret->Cr = 1;
    ret->At = 1;
    ret->It = 0;
    ret->x = (float*)(ret+1);
    *(audio_processor*)ret = &audio_limiter_interface;
    return (audio_processor*)ret;
}

