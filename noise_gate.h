#ifndef NOISE_GATE_H
#define NOISE_GATE_H 

struct noise_gate_init {
    unsigned int buffer_size;
    unsigned int lookahead;
    float noise_thresh;
    float avg_alpha;
    float ramp_time;
};

struct noise_gate;

void
noise_gate_free(struct noise_gate *ng);

struct noise_gate *
noise_gate_new(const struct noise_gate_init *init);

int
noise_gate_tick(struct noise_gate *ng, float *x);

#endif /* NOISE_GATE_H */
