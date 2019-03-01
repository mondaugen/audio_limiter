#include <stdint.h>
#include "arm_math.h"
#include "data_1.h"

__attribute__((optimize("-Ofast")))
void
nicks_add_f32(
const float *in1,
const float *in2,
float *in3,
uint32_t len)
{
    while (len--) {
        *in3++ = *in2++ + *in1++;
    }
}
    
__attribute__((optimize("-O0")))
int main (void)
{
    uint32_t len = sizeof(x1_test_data)/sizeof(float);
    float ans1[len], ans2[len];
    while (1) {
        nicks_add_f32(x1_test_data,x2_test_data,ans1,len); 
        arm_add_f32(x1_test_data,x2_test_data,ans2,len); 
    }
    return 0;
}


