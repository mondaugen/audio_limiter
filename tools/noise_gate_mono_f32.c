/*
Read in a file in f32 format and apply noise gate.
*/

#include <stdlib.h>
#include <stdio.h>
#include "noise_gate.h"

#define DEFAULT_INFILE "/tmp/snd_in.f32"
#define DEFAULT_OUTFILE "/tmp/snd_out.f32"
#define DEFAULT_BUFFER_SIZE "256"
#define DEFAULT_LOOKAHEAD "1024"
#define DEFAULT_NOISE_THRESH "1e-3"
#define DEFAULT_AVG_ALPHA "0.99"
#define DEFAULT_RAMP_TIME DEFAULT_LOOKAHEAD

#define get_param_from_env(dest_str,param)\
    if (!(dest_str = getenv(#param))) { \
        fprintf( \
            stderr, \
            #param  " not specified, using default %s\n", \
            DEFAULT_ ## param); \
        dest_str=DEFAULT_ ## param; \
    }

int main (void)
{
    FILE *sndfile_in = NULL, *sndfile_out = NULL;
    float tmp_buf[liai.buffer_size];
    size_t n_items;
    char *infile_path,
         *outfile_path,
         *buffer_size_str,
         *lookahead_str,
         *noise_thresh_str,
         *avg_alpha_str,
         *ramp_time_str;
    
    get_param_from_env(infile_path,INFILE);
    get_param_from_env(outfile_path,OUTFILE);
    get_param_from_env(buffer_sdze_str,BUFFER_SIZE);
    get_param_from_env(lookahead_str,LOOKAHEAD);
    get_param_from_env(noise_thresh_str,NOISE_THRESH);
    get_param_from_env(avg_alpha_str,AVG_ALPHA);
    get_param_from_env(ramp_time_str,RAMP_TIME);

    struct noise_gate_init ng_init = {
        .buffer_size = atoi(buffer_size_str),
        .lookahead = atoi(lookahead_str),
        .noise_thresh = atof(noise_thresh_str),
        .avg_alpha = atof(noise_thresh_str),
        .ramp_time = atoi(ramp_time_str);
    };

    struct noise_gate *ng = noise_gate_new(&ng_init);

    if (!ng) { goto fail; }

    sndfile_in = fopen(infile_path,"r");
    if (!sndfile_in) { goto fail; }
    sndfile_out = fopen(outfile_path,"w");
    if (!sndfile_out) { goto fail; }
    /* TODO: this discards a chunk of the file at the end if it is not a
    multiple of buffer size in length */
    while ((n_items = fread(tmp_buf,
                    sizeof(float),
                    ng->config.buffer_size,
                    sndfile_in)) == liai.buffer_size) {
        noise_gate_tick(ng,tmp_buf);
        fwrite(tmp_buf,sizeof(float),ng->config.buffer_size,sndfile_out);
    }

fail:
    if (lia) { noise_gate_free(ng); }
    if (sndfile_in) { fclose(sndfile_in); }
    if (sndfile_out) { fclose(sndfile_out); }
    return 0;
}

