#ifndef AUDIO_LIMITER_COMMON_H
#define AUDIO_LIMITER_COMMON_H 

#define id(x) x

#define get_param_from_env(dest,param,fmt,conv) \
    {\
    char * param ## _str_ = getenv(#param);\
    if (!(param ## _str_)) { \
        fprintf( \
            stderr, \
            #param  " not specified, using default " fmt " \n", \
            DEFAULT_ ## param); \
        dest=DEFAULT_ ## param; \
    } else { \
        dest=conv(param ## _str_); \
    }\
    }

#endif /* AUDIO_LIMITER_COMMON_H */
