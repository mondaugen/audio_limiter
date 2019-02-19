CFLAGS+=-I.. -g

bin/ringbuffer : ringbuffer.c ../ringbuffer.c
	$(CC) $(CFLAGS) $^ -o $@

bin/dec_sort : dec_sort.c
	$(CC) $(CFLAGS) $^ -o $@

/tmp/guitar.flac :
	bash ../make_guitar_test_file.sh

/tmp/guitar.f32 : /tmp/guitar.flac ../make_guitar_test_file_f32.py
	python3 ../make_guitar_test_file_f32.py

_fwir_test_header.h : ../gen_fwir_header.py
	OUTPUT_FILE=$@ python3 ../gen_fwir_header.py

bin/limiter_ir_af : limiter_ir_af.c \
                    ../limiter_ir_af.c \
                    ../ringbuffer.c \
                    ../float_buf_rngbuf.c \
                    ../filter_w_ir_filter_imp_native.c \
                    ../one_pole_filter.c \
                    /tmp/guitar.f32 \
                    _fwir_test_header.h
	$(CC) $(CFLAGS) $(filter %.c, $^) -o $@
