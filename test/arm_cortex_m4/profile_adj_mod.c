/* Compare different styles of modulo of adjacent values */

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "profile_adj_mod_test_data.h"

__attribute__((optimize("-O3")))
void
straight_mod(
const uint32_t *x,
uint32_t *y0,
uint32_t *y1,
uint32_t *y2,
uint32_t m,
uint32_t len)
{
    uint32_t x0_0, x1_0, x2_0,
             x0_1, x1_1, x2_1,
             x0_2, x1_2, x2_2,
             x0_3, x1_3, x2_3,
             blk_cnt = len >> 2;
    while (blk_cnt-- > 0) {
        x0_0 = *x;
        x0_1 = *(x+1);
        x0_2 = *(x+2);
        x0_3 = *(x+3);
        x0_0 %= m;
        x0_1 %= m;
        x0_2 %= m;
        x0_3 %= m;
        x1_0 = (x0_0 + 1) % m;
        x1_1 = (x0_1 + 1) % m;
        x1_2 = (x0_2 + 1) % m;
        x1_3 = (x0_3 + 1) % m;
        x2_0 = (x0_0 + 2) % m;
        x2_1 = (x0_1 + 2) % m;
        x2_2 = (x0_2 + 2) % m;
        x2_3 = (x0_3 + 2) % m;
        *y0 = x0_0;
        *(y0 + 1) = x0_1;
        *(y0 + 2) = x0_2;
        *(y0 + 3) = x0_3;
        *y1 = x1_0;
        *(y1 + 1) = x1_1;
        *(y1 + 2) = x1_2;
        *(y1 + 3) = x1_3;
        *y2 = x2_0;
        *(y2 + 1) = x2_1;
        *(y2 + 2) = x2_2;
        *(y2 + 3) = x2_3;
        y0+=4;
        y1+=4;
        y2+=4;
        x+=4;
    }
    blk_cnt = len & 3;
    while (blk_cnt-- > 0) {
        x0_0 = *x;
        x0_0 %= m;
        x1_0 = (x0_0 + 1) % m;
        x2_0 = (x0_0 + 2) % m;
        *y0 = x0_0;
        *y1 = x1_0;
        *y2 = x2_0;
        y0++;
        y1++;
        y2++;
        x++;
    }
}

/* This is only about 9% faster..., but it works. */
__attribute__((optimize("-O3")))
void
smart_mod(
const uint32_t *x,
uint32_t *y0,
uint32_t *y1,
uint32_t *y2,
uint32_t m,
uint32_t len)
{
    uint32_t x0_0, x1_0, x2_0,
             x0_1, x1_1, x2_1,
             x0_2, x1_2, x2_2,
             x0_3, x1_3, x2_3,
             blk_cnt = len >> 2,
             m_1 = m - 1;
    while (blk_cnt-- > 0) {
        x0_0 = *x;
        x0_1 = *(x+1);
        x0_2 = *(x+2);
        x0_3 = *(x+3);
        x0_0 %= m;
        x0_1 %= m;
        x0_2 %= m;
        x0_3 %= m;
        x1_0 = x0_0 + 1;
        x1_1 = x0_1 + 1;
        x1_2 = x0_2 + 1;
        x1_3 = x0_3 + 1;
        x1_0 *= (x0_0 < m_1);
        x1_1 *= (x0_1 < m_1);
        x1_2 *= (x0_2 < m_1);
        x1_3 *= (x0_3 < m_1);
        x2_0 = x1_0 + 1;
        x2_1 = x1_1 + 1;
        x2_2 = x1_2 + 1;
        x2_3 = x1_3 + 1;
        x2_0 *= (x1_0 < m_1);
        x2_1 *= (x1_1 < m_1);
        x2_2 *= (x1_2 < m_1);
        x2_3 *= (x1_3 < m_1);
        *y0 = x0_0;
        *(y0 + 1) = x0_1;
        *(y0 + 2) = x0_2;
        *(y0 + 3) = x0_3;
        *y1 = x1_0;
        *(y1 + 1) = x1_1;
        *(y1 + 2) = x1_2;
        *(y1 + 3) = x1_3;
        *y2 = x2_0;
        *(y2 + 1) = x2_1;
        *(y2 + 2) = x2_2;
        *(y2 + 3) = x2_3;
        y0+=4;
        y1+=4;
        y2+=4;
        x+=4;
    }
    blk_cnt = len & 3;
    while (blk_cnt-- > 0) {
        x0_0 = *x;
        x0_0 %= m;
        x1_0 = x0_0 + 1;
        x1_0 *= (x0_0 < m_1);
        x2_0 = x1_0 + 1;
        x2_0 *= (x1_0 < m_1);
        *y0 = x0_0;
        *y1 = x1_0;
        *y2 = x2_0;
        y0++;
        y1++;
        y2++;
        x++;
    }
}

__attribute__((optimize("-O0")))
int chk_correct(uint32_t *a, uint32_t *b, uint32_t len)
{
    int correct = 1;
    while ((len-- > 0) && (correct != 0)) {
        correct &= *a++ == *b++;
    }
    return correct;
}
        
__attribute__((optimize("-O0")))
int main (void)
{
    uint32_t ans1_0[NVALS], ans1_1[NVALS], ans1_2[NVALS],
                 ans2_0[NVALS], ans2_1[NVALS], ans2_2[NVALS];
    while (1) {
        straight_mod(x, ans1_0, ans1_1, ans1_2, M, NVALS);
        smart_mod(x, ans2_0, ans2_1, ans2_2, M, NVALS);
        assert(chk_correct(y0,ans1_0,NVALS));
        assert(chk_correct(y1,ans1_1,NVALS));
        assert(chk_correct(y2,ans1_2,NVALS));
        assert(chk_correct(y0,ans2_0,NVALS));
        assert(chk_correct(y1,ans2_1,NVALS));
        assert(chk_correct(y2,ans2_2,NVALS));
    }
    return 0;
}
        
