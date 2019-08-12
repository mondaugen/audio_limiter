/*
I want to see if the compiler emits the instructions I want.

Conclusion: compiling with arm-none-eabi-gcc (GNU Tools for Arm Embedded
Processors 8-2019-q3-update) 8.3.1 20190703 (release) [gcc-8-branch revision
273027] emits the vabs.f32 instruction, so that is good :).
*/

#include <math.h>

float my_func(float f)
{
    return fabsf(f);
}
