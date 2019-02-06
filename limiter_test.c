#include <stdio.h>
#include "limiter.h"
#include "simple_player.h"

#define BUFFER_SIZE 4

void
print_buffer(float *f)
{
    int n;
    for (n = 0; n < BUFFER_SIZE; n++) {
        printf("%.18e ", f[n]);
    }
}

void run_limiter_test (float *x1, int n_samples, char *name)
{
    struct audio_limiter_params alimiter_params = {
        .Nl = 4,
        .Nr = 3,
        .Nd = 3,
        .Nb = 4
    };
    struct simple_player_params splayer_params = {
        .buffer_size = alimiter_params.Nb,
        .samples = x1,
        .n_samples = n_samples
    };
    int n;
    audio_processor *alimiter = audio_limiter_new(&alimiter_params);
    audio_processor *aplayer = simple_player_new(&splayer_params);
    puts(name);
    for (n = 0; n < splayer_params.n_samples; n += BUFFER_SIZE) {
        (*aplayer)->tick(aplayer,NULL);
        printf("Player result:\n");
        print_buffer((*aplayer)->out_buffer(aplayer));
        printf("\n");
        (*alimiter)->tick(alimiter, (*aplayer)->out_buffer(aplayer));
        printf("Limiter result:\n");
        print_buffer((*alimiter)->out_buffer(alimiter));
        printf("\n");
    } 
    printf("\n");
    (*aplayer)->free(aplayer);
    (*alimiter)->free(alimiter);
}

int main (void)
{
    float x1[] = {.5,.75,.5,.75,.5,2,.5,4,.5,0,0,0,0,0,0,0};
    float x2[] = {.5,.75,.5,.75,.5,4,.5,3,.5,0,0,0,0,0,0,0};
    float x3[] = {.5,.75,.5,.75,.5,4,.5,1.5,.5,0,0,0,0,0,0,0};
    run_limiter_test(
        x1,
        sizeof(x1)/sizeof(float),
        "test 1"
    );
    run_limiter_test(
        x2,
        sizeof(x2)/sizeof(float),
        "test 2"
    );
    run_limiter_test(
        x3,
        sizeof(x3)/sizeof(float),
        "test 3"
    );
}


