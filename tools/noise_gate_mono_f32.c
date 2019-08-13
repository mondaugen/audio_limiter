/*
Read in a file in f32 format and apply noise gate.
*/

#include <stdlib.h>
#include <stdio.h>
#include "noise_gate.h"
#include "common.h"

#define DEFAULT_RAMP_TIME 1024.0
#define DEFAULT_INFILE "/tmp/snd_in.f32"
#define DEFAULT_OUTFILE "/tmp/snd_out.f32"
#define DEFAULT_BUFFER_SIZE 256
#define DEFAULT_LOOKAHEAD 1024
#define DEFAULT_NOISE_THRESH 1e-3
#define DEFAULT_AVG_ALPHA 0.99

int main (void)
{
    FILE *sndfile_in = NULL, *sndfile_out = NULL;
    size_t n_items;
    char *infile_path,
         *outfile_path;
    int buffer_size,
        lookahead;
    float noise_thresh,
          avg_alpha,
          ramp_time;
    
    // get_param_from_env(dest,param,fmt,conv)
    get_param_from_env(ramp_time,RAMP_TIME,"%f",atof);
    get_param_from_env(infile_path,INFILE,"%s",id);
    get_param_from_env(outfile_path,OUTFILE,"%s",id);
    get_param_from_env(buffer_size,BUFFER_SIZE,"%d",atoi);
    get_param_from_env(lookahead,LOOKAHEAD,"%d",atoi);
    get_param_from_env(noise_thresh,NOISE_THRESH,"%f",atof);
    get_param_from_env(avg_alpha,AVG_ALPHA,"%f",atof);

    struct noise_gate_init ng_init = {
        .buffer_size = buffer_size,
        .lookahead = lookahead,
        .noise_thresh = noise_thresh,
        .avg_alpha = avg_alpha,
        .ramp_time = ramp_time
    };
    float tmp_buf[ng_init.buffer_size];

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
                    ng_init.buffer_size,
                    sndfile_in)) == ng_init.buffer_size) {
        noise_gate_tick(ng,tmp_buf);
        fwrite(tmp_buf,sizeof(float),ng_init.buffer_size,sndfile_out);
    }

fail:
    if (ng) { noise_gate_free(ng); }
    if (sndfile_in) { fclose(sndfile_in); }
    if (sndfile_out) { fclose(sndfile_out); }
    return 0;
}

