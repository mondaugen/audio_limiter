void
mul_f32_vecs(float *x, const float *y, unsigned int N)
{
    while (N-- > 0) { *x++ *= *y++; }
}

