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

CMSIS_INCLUDES = -I../CMSIS_5/CMSIS/Core/Include/ -I../CMSIS_5/CMSIS/DSP/Include/

CMSIS_LDFLAGS = -L../CMSIS_5/CMSIS/Lib/GCC \
                -larm_cortexM4lf_math \
                -Ttest/arm_cortex_m4/stm32_support/stm32f429.ld

STM32_SUPPORT_FILES=test/arm_cortex_m4/stm32_support/system_init.c \
    test/arm_cortex_m4/stm32_support/syscalls.c \
    test/arm_cortex_m4/stm32_support/startup.c \
    test/arm_cortex_m4/stm32_support/fmc.c 

STM32_SUPPORT_INCLUDES=-Itest/arm_cortex_m4/stm32_support/

STM32_CFLAGS=-DSTM32F429_439xx

ARM_PROFILE_CFLAGS=-DARM_MATH_CM4 -D__FPU_PRESENT \
        $(STM32_CFLAGS) \
        $(STM32_SUPPORT_INCLUDES) \
 		$(CMSIS_INCLUDES) \
 		$(CFLAGS)

ARFLAGS   = rcs

liblimiter_ir_af.a : limiter_ir_af.o \
                  ringbuffer.o \
                  float_buf_rngbuf.o \
                  filter_w_ir_filter_imp_native.o \
                  one_pole_filter.o
	$(AR) $(ARFLAGS) $@ $^

test/.gen/data_1.h : test/gen_data_1.py
	OUTFILE=$@ python3 $<

define profile_template =
$(1) : $(2) $(STM32_SUPPORT_FILES) $(3)
	$(CC) $$(filter %.c, $$^) -o $$@ -Itest/.gen $(ARM_PROFILE_CFLAGS) $(CMSIS_LDFLAGS)
endef

# for reference, the following expands to 
#test/arm_cortex_m4/bin/profile_arm_add : \
#    test/arm_cortex_m4/profile_arm_add.c \
#    $(STM32_SUPPORT_FILES) \
#    test/.gen/data_1.h
#	$(CC) $(filter %.c, $^) -o $@ \
#        -Itest/.gen \
#        $(ARM_PROFILE_CFLAGS) \
# 		$(CMSIS_LDFLAGS)

$(eval $(call profile_template, \
test/arm_cortex_m4/bin/profile_arm_add, \
test/arm_cortex_m4/profile_arm_add.c, \
test/.gen/data_1.h ))
