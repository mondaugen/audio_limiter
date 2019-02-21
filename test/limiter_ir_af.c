#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "limiter_ir_af.h"
#include "_fwir_test_header.h"

void
print_buffer(float *f, int len)
{
    while (len-- > 0) {
        printf("%.18f\n", *f++);
    }
    printf("\n");
}


static int 
test_1(void)
{
    
    struct filter_w_ir_init fwiri = {
        .b = NULL,
        .len_b = 0,
        /* reaches 1e-3 in 3 time steps */
        .a = (float[1]) {0.1},
        .len_a = 1,
        /* The precomputed IR */
        .ir = (float[8]) {0,0.25,0.5,0.75,1,0.1,0.01,0.001},
        .len_ir = 8,
    };

    struct filter_w_ir *fwir = filter_w_ir_new(&fwiri);

    struct limiter_ir_af_init liai = {
        .fwir = fwir,
        .buffer_size = 4,
        .threshold = 1
    };

    float test_sig[] = {1,1,1,2,1,-3,1,1,1,1,1,1,0,0,0,0},
          *pflt = test_sig;

    struct limiter_ir_af *lia = limiter_ir_af_new(&liai);
    assert(lia != NULL);

    int n;
    for (n = 0; n < (sizeof(test_sig)/sizeof(float)); n += liai.buffer_size) {
        limiter_ir_af_tick(lia,pflt);
        pflt += liai.buffer_size;
    }

    print_buffer(test_sig,(sizeof(test_sig)/sizeof(float)));

    limiter_ir_af_free(lia);
    return 1;
}

int 
test_2 (void)
{

    struct filter_w_ir *fwir = filter_w_ir_new(&gen_fwir_header_filter_w_ir_init);

    struct limiter_ir_af_init liai = {
        .fwir = fwir,
        .buffer_size = 256,
        .threshold = 1
    };

    struct limiter_ir_af *lia = limiter_ir_af_new(&liai);
    FILE *sndfile_in = NULL, *sndfile_out = NULL;
    float tmp_buf[liai.buffer_size];
    size_t n_items;

    if (!lia) { goto fail; }

    sndfile_in = fopen("/tmp/guitar.f32","r");
    if (!sndfile_in) { goto fail; }
    sndfile_out = fopen("/tmp/guitar_limited.f32","w");
    if (!sndfile_out) { goto fail; }
    while ((n_items = fread(tmp_buf,
            sizeof(float),
            liai.buffer_size,
            sndfile_in)) == liai.buffer_size) {
        limiter_ir_af_tick(lia,tmp_buf);
        fwrite(tmp_buf,sizeof(float),liai.buffer_size,sndfile_out);
    }

fail:
    if (lia) { limiter_ir_af_free(lia); }
    if (sndfile_in) { fclose(sndfile_in); }
    if (sndfile_out) { fclose(sndfile_out); }
    return 0;
}

int main (void)
{
    test_1();
    test_2();
    return 0;
}
