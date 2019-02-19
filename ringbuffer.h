#ifndef RINGBUFFER_H
#define RINGBUFFER_H 

struct rngbuf;

struct rngbuf_slice {
    char *first_region;
    unsigned int first_region_size;
    char *second_region;
    unsigned int second_region_size;
};

void
rngbuf_free(struct rngbuf *rb);

struct rngbuf *
rngbuf_new(unsigned int capacity);

unsigned int
rngbuf_contents_size(struct rngbuf *rb);

unsigned int
rngbuf_available_capacity(struct rngbuf *rb);

int
rngbuf_advance_head(struct rngbuf *rb, unsigned int n);

int
rngbuf_push_copy(struct rngbuf *rb, const char *dat, unsigned int n);

int
rngbuf_shift_in(
    struct rngbuf *rb,
    const char *data,
    unsigned int n);

int
rngbuf_get_slice(
    struct rngbuf *rb,
    struct rngbuf_slice *rb_slice,
    unsigned int start,
    unsigned int length);

int
rngbuf_memcpy(
    struct rngbuf *rb,
    unsigned int start,
    unsigned int length,
    char *dest);

#endif /* RINGBUFFER_H */
