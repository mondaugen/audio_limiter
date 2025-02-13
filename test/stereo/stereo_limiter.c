#include <stdlib.h>
#include "_gend_fwir_header.h"
#include "limiter_ir_af.h"

struct test_limiter {
    struct limiter_ir_af *audio_limiter;
    struct filter_w_ir *audio_limiter_fwir;
}

void test_limiter_free(struct test_limiter *tl)
{
    if (tl) {
        if (tl->audio_limiter_fwir) {
            filter_w_ir_free(audio_limiter_fwir);
        }
        if (tl->audio_limiter) {
            limiter_ir_af_free(tl->audio_limiter);
        }
        free(tl);
    }
}

struct test_limiter *test_limiter_new(float thresh, unsigned int n_channels)
{
    struct test_limiter *r = calloc(1,sizeof(struct test_limiter));
    if (!r) { goto fail; }
    r->audio_limiter_fwir = filter_w_ir_new(&gen_fwir_header_filter_w_ir_init);
    if (!r->audio_limiter_fwir) { goto fail; }
    struct limiter_ir_af_init liai = {
        .fwir = r->audio_limiter_fwir,
        .buffer_size = config->pwp_config.block_size,
        /* TODO: configure this threshold? */
        .threshold = .9,
        /* TODO: configure number of channels? */
        .n_channels = 2
    };
    r->audio_limiter = limiter_ir_af_new(&liai);
    if (!r->audio_limiter) { goto fail; }
    return r;
fail:
    test_limiter_free(r);
    return NULL;
};

struct limiter_ir_af *test_limiter_ir_af(struct test_limiter *tl)
{
    return tl->audio_limiter;
}
