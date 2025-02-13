CFLAGS+=-g -std=gnu11
INCLUDE=.
limiter_test : limiter_test.c limiter.c simple_player.c

limiter_ir_af.o : limiter_ir_af.c limiter_ir_af.h float_buf.h

float_buf_rngbuf.o : float_buf_rngbuf.c float_buf.h

filter_w_ir_filter_imp_native.o : filter_w_ir_filter_imp_native.c one_pole_filter.h limiter_ir_af.h

filter_w_ir_filter_imp_arm_cm4.o : filter_w_ir_filter_imp_arm_cm4.c limiter_ir_af.h

limiter_ir_af_routines_native.o : limiter_ir_af_routines_native.c limiter_ir_af.h

limiter_ir_af_routines_arm_cm4.o : limiter_ir_af_routines_arm_cm4.c limiter_ir_af.h

one_pole_filter.o : one_pole_filter.c

clean :
	rm -f *.o

.PHONY: clean
