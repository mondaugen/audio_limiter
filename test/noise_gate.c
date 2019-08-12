#include <stdio.h>
#include "noise_gate.h"

int test1 (void)
{
    float in[] = {0, 1, 1, 0.1, -0.1, 0.1, -0.1, 1, 1, 0.2, -0.2, 0.2};
    struct noise_gate_init ng_init = {
        .buffer_size = 4,
        .lookahead = 3,
        .noise_thresh = .15,
        .avg_alpha = 1,
        .ramp_time = 2
    };
    struct noise_gate *ng = noise_gate_new(&ng_init);
    int n, len_in = sizeof(in)/sizeof(float);
    if (!ng) { return -1; }
    for (n = 0; n < len_in; n += ng_init.buffer_size) {
        noise_gate_tick(ng,in+n);
    }
    for (n = 0; n < len_in; n++) {
        printf("%f ", in[n]);
    }
    printf("\n");
    return 0;
}

int main (void)
{
    test1();
    return 0;
}
        

