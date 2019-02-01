#include "limiter.h"

#define BUFFER_SIZE 4

int main (void)
{
    float x1[] = {.5,.75,.5,.75,.5,2,.5,4,.5,0,0,0,0,0,0,0};
    struct audio_limiter_params params = {
        .Nl = 4,
        .Nr = 3,
        .Nd = 3,
        .Nb = 4
    };
    struct audio_limiter *l = audio_limiter_new(&params);
    int n;
    for (n = 0; n < (sizeof(x1)/sizeof(float)); n += BUFFER_SIZE) {
        


