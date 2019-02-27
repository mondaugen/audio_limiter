/* Low level limiter routines, optimized for ARM Cortex-M4 */
#include <stdint.h>

/* computes y[n] += x[n] * a */
__attribute__((optimize("-O3")))
void
acc_with_scale(
float *y,
const float *x,
float a,
unsigned int len)
{
    uint32_t blkCnt;                               /* loop counter */

    float inA1, inA2, inA3, inA4;              /* temporary input variabels */
    float inB1, inB2, inB3, inB4;              /* temporary input variables */

    /*loop Unrolling */
    blkCnt = len >> 2U;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
     ** a second loop below computes the remaining 1 to 3 samples. */
    while (blkCnt > 0U)
    {
        /* C = A + B */
        /* Add and then store the results in the destination buffer. */

        /* read four inputs from sourceA and four inputs from sourceB */
        inA1 = *y;
        inB1 = *x;
        inA2 = *(y + 1);
        inB2 = *(x + 1);
        inA3 = *(y + 2);
        inB3 = *(x + 2);
        inA4 = *(y + 3);
        inB4 = *(x + 3);

        /* C = A + B */
        /* add and store result to destination */
        *y = inA1 + inB1 * a;
        *(y + 1) = inA2 + inB2 * a;
        *(y + 2) = inA3 + inB3 * a;
        *(y + 3) = inA4 + inB4 * a;

        /* update pointers to process next samples */
        y += 4U;
        x += 4U;

        /* Decrement the loop counter */
        blkCnt--;
    }

    /* If the len is not a multiple of 4, compute any remaining output samples here.
     ** No loop unrolling is used. */
    blkCnt = len & (4U-1);

    while (blkCnt > 0U)
    {
        /* C = A + B */
        /* Add and then store the results in the destination buffer. */
        *y = *y + a * *x;
        x++;
        y++;
        /* Decrement the loop counter */
        blkCnt--;
    }
}

/* compute y[n] *= 1 - x[n] */
__attribute__((optimize("-O3")))
void
mul_one_minus_vec(float *y, float *x, unsigned int len)
{
    /* Unrolling to 8 seems to be fastest. */
    float y0,y1,y2,y3,y4,y5,y6,y7,
          x0,x1,x2,x3,x4,x5,x6,x7;
    unsigned int blk_cnt = len >> 3U;
    while (blk_cnt > 0) {
        x0 = *x;
        x1 = *(x+1);
        x2 = *(x+2);
        x3 = *(x+3);
        x4 = *(x+4);
        x5 = *(x+5);
        x6 = *(x+6);
        x7 = *(x+7);
        y0 = *y;
        y1 = *(y+1);
        y2 = *(y+2);
        y3 = *(y+3);
        y4 = *(y+4);
        y5 = *(y+5);
        y6 = *(y+6);
        y7 = *(y+7);
        x0 *= y0;
        x1 *= y1;
        x2 *= y2;
        x3 *= y3;
        x4 *= y4;
        x5 *= y5;
        x6 *= y6;
        x7 *= y7;
        y0 -= x0;
        y1 -= x1;
        y2 -= x2;
        y3 -= x3;
        y4 -= x4;
        y5 -= x5;
        y6 -= x6;
        y7 -= x7;
        *y = y0;
        *(y+1) = y1;
        *(y+2) = y2;
        *(y+3) = y3;
        *(y+4) = y4;
        *(y+5) = y5;
        *(y+6) = y6;
        *(y+7) = y7;
        x += 8;
        y += 8;
        blk_cnt--;
    }
    blk_cnt = len & 7U;
    while (blk_cnt > 0) {
        x0 = *x;
        y0 = *y;
        x0 *= y0;
        y0 -= x0;
        *y = y0;
        x++;
        y++;
        blk_cnt--;
    }
}
