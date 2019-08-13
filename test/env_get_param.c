#include <stdio.h>
#include <stdlib.h>

#define id(x) x
#define get_param_from_env(dest,param,fmt,conv) \
    char * param ## _str_ ## __LINE__ = getenv(#param);\
    if (!(param ## _str_ ## __LINE__)) { \
        fprintf( \
            stderr, \
            #param  " not specified, using default " fmt " \n", \
            DEFAULT_ ## param); \
        dest=DEFAULT_ ## param; \
    } else { \
        dest=conv(param ## _str_ ## __LINE__); \
    }

#define DEFAULT_A 123
#define DEFAULT_B 456.789
#define DEFAULT_C "hello"

int main (void)
{
    int A;
    float B;
    char *C;
    get_param_from_env(A,A,"%d",atoi);
    get_param_from_env(B,B,"%f",atof);
    get_param_from_env(C,C,"%s",id);
    printf("A = %d\n",A);
    printf("B = %f\n",B);
    printf("C = %s\n",C);
    return 0;
}
