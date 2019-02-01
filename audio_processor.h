#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H 

typedef struct audio_processor_interface * audio_processor;

struct audio_processor_interface {
    /* The routine that will process the samples in *in and the result can be
    obtained through *out_buffer */
    int (*tick)(audio_processor *p, float *in);
    /* Number of samples that should be passed in to tick routine */
    int (*in_buffer_size)(audio_processor *p);
    /* Number of samples that can be read out of the out_buffer */
    int (*out_buffer_size)(audio_processor *p);
    /* Return memory location where others can read out processed audio data */
    float * (*out_buffer)(audio_processor *p);
    /* Free the instance of the audio processor */
    void (*free)(audio_processor *p);
};

/* call methods like so:
    audio_processor *p = ...;
    (*p)->tick(p,buffer); */

#endif /* AUDIO_PROCESSOR_H */
