#ifndef FLOAT_BUF_H
#define FLOAT_BUF_H 
 
struct float_buf;

void
float_buf_free(struct float_buf *fb);

struct float_buf *
float_buf_new(unsigned int size);

struct float_buf_where_val { unsigned int n; float f; }; 

int
float_buf_push_copy(struct float_buf *fb, unsigned int n, const float *values);

int
float_buf_lookup(struct float_buf *fb, unsigned int n, float *dest);

/*
process a region starting at start that extends for length.
process may be called multiple times but will always be called in order (say if
the underlying datastructure is segmented).
*/
int
float_buf_process_region(
    struct float_buf *fb,
    unsigned int start,
    unsigned int length,
    void (*process)(
        float *seg,
        unsigned int len,
        void *aux),
    void *aux);

int
float_buf_shift_in(
    struct float_buf *fb,
    const float *values,
    unsigned int nvalues);

/*
Copy values out of the float buffer from start and extending for length into
dest.
*/
int
float_buf_memcpy(
    struct float_buf *fb,
    unsigned int start,
    unsigned int length,
    float *dest);

/*
Make an array of struct float_buf_where_val.
The value is included in the array if chk returns non-zero when called on the
value.
fun is then called on the array of values.
*/
int
float_buf_where_values(
    struct float_buf *fb,
    unsigned int start,
    unsigned int length,
    int (*chk)(float val, void *aux),
    void (*fun)(struct float_buf_where_val *v,
                unsigned int nvals,
                void *aux),
    void *aux);

#endif /* FLOAT_BUF_H */
