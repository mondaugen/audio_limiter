#include <stdlib.h>
#include <stdio.h>

struct float_buf_where_val { unsigned int n; float f; };

static int
fval_where_val_cmp_dec(const void *a_, const void *b_)
{
    const struct float_buf_where_val *a = a_, *b = b_;
    return (b->f > a->f) - (a->f > b->f);
}

int main (void)
{
    struct float_buf_where_val vals[] = {
        {10,0.1},
        {5,0.5},
        {2,1.1},
        {7,0.5},
    };
    qsort(vals,
          sizeof(vals)/sizeof(struct float_buf_where_val),
          sizeof(struct float_buf_where_val),
          fval_where_val_cmp_dec);
    int n;
    for (n = 0; n < (sizeof(vals)/sizeof(struct float_buf_where_val)); n++) {
        printf("%f\n",vals[n].f);
    }
    return 0;
}

