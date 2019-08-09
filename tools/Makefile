CFLAGS+=-I.. -g

_fwir_test_header.h : ../gen_fwir_header.py
	OUTPUT_FILE=$@ python3 ../gen_fwir_header.py

bin/limit_mono_f32 : limit_mono_f32.c \
                    ../limiter_ir_af.c \
                    ../ringbuffer.c \
                    ../float_buf_rngbuf.c \
                    ../filter_w_ir_filter_imp_native.c \
                    ../one_pole_filter.c \
                    ../limiter_ir_af_routines_native.c \
                    _fwir_test_header.h
	$(CC) $(CFLAGS) $(filter %.c, $^) -o $@
