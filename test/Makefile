CFLAGS+=-I.. -g

bin/ringbuffer : ringbuffer.c ../ringbuffer.c
	$(CC) $(CFLAGS) $^ -o $@

bin/dec_sort : dec_sort.c
	$(CC) $(CFLAGS) $^ -o $@
