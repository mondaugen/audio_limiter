CFLAGS+=-I.. -g

bin/ringbuffer : ringbuffer.c ../ringbuffer.c
	$(CC) $(CFLAGS) $^ -o $@

bin/dec_sort : dec_sort.c
	$(CC) $(CFLAGS) $^ -o $@

bin/limiter_ir_tf : limiter_ir_tf.c ../limiter_ir_tf.c ../ringbuffer.c ../float_buf_rngbuf.c
	$(CC) $(CFLAGS) $^ -o $@
