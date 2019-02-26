OPTIMIZE?=-Ofast
DEBUG_FLAGS=-ggdb3
CFLAGS += $(DEBUG_FLAGS) -Wall -ffunction-sections -fdata-sections
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -dD --specs=nano.specs
CFLAGS += -Wno-unused-function
CFLAGS += $(OPTIMIZE)
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar

ARFLAGS   = rcs

liblimiter_ir_af.a : limiter_ir_af.o \
                  ringbuffer.o \
                  float_buf_rngbuf.o \
                  filter_w_ir_filter_imp_native.o \
                  one_pole_filter.o
	$(AR) $(ARFLAGS) $@ $^
