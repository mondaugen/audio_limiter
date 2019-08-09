/*
Read in a file in f32 format and apply limiting.
*/

#include <stdlib.h>
#include <stdio.h>
#include "limiter_ir_af.h"
#include "_fwir_test_header.h"

#define DEFAULT_INFILE_PATH "/tmp/snd_in.f32"
#define DEFAULT_OUTFILE_PATH "/tmp/snd_out.f32"

int main (void)
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
    char *infile_path, *outfile_path;

    if (!lia) { goto fail; }

    if (!(infile_path = getenv("INFILE"))) {
        fprintf(
            stderr,
            "INFILE not specified, using default %s\n",
            DEFAULT_INFILE_PATH);
        infile_path=DEFAULT_INFILE_PATH;
    }

    if (!(outfile_path = getenv("OUTFILE"))) {
        fprintf(
            stderr,
            "OUTFILE not specified, using default %s\n",
            DEFAULT_OUTFILE_PATH);
        outfile_path=DEFAULT_OUTFILE_PATH;
    }

    sndfile_in = fopen(infile_path,"r");
    if (!sndfile_in) { goto fail; }
    sndfile_out = fopen(outfile_path,"w");
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
