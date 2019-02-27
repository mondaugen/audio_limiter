#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "scale_one_minus_x_data.h"

__attribute__((optimize("-O2"),noinline))
void
mul_one_minus_vec_unroll_1(float *y, float *x, unsigned int len)
{
    float y0,
          x0;
    while (len > 0) {
        x0 = *x;
        y0 = *y;
        x0 *= y0;
        y0 -= x0;
        *y = y0;
        x++;
        y++;
        len--;
    }
}

__attribute__((optimize("-O2")))
void
mul_one_minus_vec_unroll_4(float *y, float *x, unsigned int len)
{
    float y0,y1,y2,y3,
          x0,x1,x2,x3;
    unsigned int blk_cnt = len >> 2U;
    while (blk_cnt > 0) {
        x0 = *x;
        x1 = *(x+1);
        x2 = *(x+2);
        x3 = *(x+3);
        y0 = *y;
        y1 = *(y+1);
        y2 = *(y+2);
        y3 = *(y+3);
        x0 *= y0;
        x1 *= y1;
        x2 *= y2;
        x3 *= y3;
        y0 -= x0;
        y1 -= x1;
        y2 -= x2;
        y3 -= x3;
        *y = y0;
        *(y+1) = y1;
        *(y+2) = y2;
        *(y+3) = y3;
        x += 4;
        y += 4;
        blk_cnt--;
    }
    blk_cnt = len & 3U;
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

__attribute__((optimize("-O2")))
void
mul_one_minus_vec_unroll_8(float *y, float *x, unsigned int len)
{
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
        
__attribute__((optimize("-O2")))
void
mul_one_minus_vec_unroll_16(float *y, float *x, unsigned int len)
{
    float y0,y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12,y13,y14,y15,
          x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15;
    unsigned int blk_cnt = len >> 4U;
    while (blk_cnt > 0) {
        x0 = *x;
        x1 = *(x+1);
        x2 = *(x+2);
        x3 = *(x+3);
        x4 = *(x+4);
        x5 = *(x+5);
        x6 = *(x+6);
        x7 = *(x+7);
        x8 = *(x+8);
        x9 = *(x+9);
        x10 = *(x+10);
        x11 = *(x+11);
        x12 = *(x+12);
        x13 = *(x+13);
        x14 = *(x+14);
        x15 = *(x+15);
        y0 = *y;
        y1 = *(y+1);
        y2 = *(y+2);
        y3 = *(y+3);
        y4 = *(y+4);
        y5 = *(y+5);
        y6 = *(y+6);
        y7 = *(y+7);
        y8 = *(y+8);
        y9 = *(y+9);
        y10 = *(y+10);
        y11 = *(y+11);
        y12 = *(y+12);
        y13 = *(y+13);
        y14 = *(y+14);
        y15 = *(y+15);
        x0 *= y0;
        x1 *= y1;
        x2 *= y2;
        x3 *= y3;
        x4 *= y4;
        x5 *= y5;
        x6 *= y6;
        x7 *= y7;
        x8 *= y8;
        x9 *= y9;
        x10 *= y10;
        x11 *= y11;
        x12 *= y12;
        x13 *= y13;
        x14 *= y14;
        x15 *= y15;
        y0 -= x0;
        y1 -= x1;
        y2 -= x2;
        y3 -= x3;
        y4 -= x4;
        y5 -= x5;
        y6 -= x6;
        y7 -= x7;
        y8 -= x8;
        y9 -= x9;
        y10 -= x10;
        y11 -= x11;
        y12 -= x12;
        y13 -= x13;
        y14 -= x14;
        y15 -= x15;
        *y = y0;
        *(y+1) = y1;
        *(y+2) = y2;
        *(y+3) = y3;
        *(y+4) = y4;
        *(y+5) = y5;
        *(y+6) = y6;
        *(y+7) = y7;
        *(y+8) = y8;
        *(y+9) = y9;
        *(y+10) = y10;
        *(y+11) = y11;
        *(y+12) = y12;
        *(y+13) = y13;
        *(y+14) = y14;
        *(y+15) = y15;
        x += 16;
        y += 16;
        blk_cnt--;
    }
    blk_cnt = len & 15U;
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

#define TOL 1e-6

int chk_correct(float *a, float *b, uint32_t len)
{
    int correct = 1;
    while ((len-- > 0) && (correct != 0)) {
        correct &= fabs(*a++ - *b++) < TOL;
    }
    return correct;
}

__attribute__((optimize("-O0")))
int main (void)
{
    float ans1[NVALS], ans2[NVALS], ans3[NVALS];
    memcpy(ans1,y,sizeof(float)*NVALS);
    memcpy(ans2,y,sizeof(float)*NVALS);
    memcpy(ans3,y,sizeof(float)*NVALS);
    while (1) {
        mul_one_minus_vec_unroll_4(ans1,x,NVALS);
        mul_one_minus_vec_unroll_8(ans2,x,NVALS);
        mul_one_minus_vec_unroll_16(ans3,x,NVALS);
        assert(chk_correct(ans1,y_result,NVALS));
        assert(chk_correct(ans2,y_result,NVALS));
        assert(chk_correct(ans3,y_result,NVALS));
        memcpy(ans1,y,sizeof(float)*NVALS);
        memcpy(ans2,y,sizeof(float)*NVALS);
        memcpy(ans3,y,sizeof(float)*NVALS);
    }
    mul_one_minus_vec_unroll_1(ans1,x,NVALS);
}
        
