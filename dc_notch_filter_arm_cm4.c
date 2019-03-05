#include "dc_notch_filter_private.h"

/* Implementation of the tick function that is faster for ARM cortex-M4 */
__attribute__((optimize("-O3")))
void
dc_notch_filter_1_pole_tick(
struct dc_notch_filter_1_pole *d,
float *buf)
{
    unsigned int buffer_size = d->buffer_size,
                 blk_cnt = buffer_size >> 2U;
    float        b0 = d->b[0],
                 b1 = d->b[1],
                 a1 = d->a1,
                 d1 = d->d_1,
                 p0,p1,p3,
                 A1,
                 xn1,xn2,xn3,xn4,
                 acc1,acc2,acc3,acc4;
    while (blk_cnt-- > 0) {
        /*
        Direct form II transposed:
        y[n] + y[n-1]*a1 = x[n]*b0 + x[n-1]*b1 ->
        y[n] = x[n]*b0 + x[n-1]*b1 - y[n-1]*a1 ->
        y[n] = x[n]*b0 + d_1
        */
         xn1 = buf[0];
         xn2 = buf[1];
         xn3 = buf[2];
         xn4 = buf[3];

         p0 = b0 * xn1;
         p1 = b1 * xn1;
         acc1 = p0 + d1;
         p0 = b0 * xn2;
         p3 = a1 * acc1;
         A1 = p1 - p3;
         d1 = A1;

         p1 = b1 * xn2;
         acc2 = p0 + d1;
         p0 = b0 * xn3;
         p3 = a1 * acc2;
         A1 = p1 - p3;
         d1 = A1;

         p1 = b1 * xn3;
         acc3 = p0 + d1;
         p0 = b0 * xn4;
         p3 = a1 * acc3;
         A1 = p1 - p3;
         d1 = A1;

         acc4 = p0 + d1;
         p1 = b1 * xn4;
         p3 = a1 * acc4;
         A1 = p1 - p3;
         d1 = A1;

         buf[0] = acc1;
         buf[1] = acc2;
         buf[2] = acc3;
         buf[3] = acc4;
		 buf += 4;
    }
    blk_cnt = buffer_size & 3U;
    while (blk_cnt-- > 0) {
         xn1 = *buf;

         p0 = b0 * xn1;
         p1 = b1 * xn1;
         acc1 = p0 + d1;
         p3 = a1 * acc1;
         A1 = p1 - p3;
         d1 = A1;

         *buf++ = acc1;
    }
    d->d_1 = d1;
}
