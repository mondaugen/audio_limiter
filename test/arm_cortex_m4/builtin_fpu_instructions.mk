OPTIMIZE?=-Ofast
DEBUG_FLAGS=
CFLAGS += $(DEBUG_FLAGS) -Wall -ffunction-sections -fdata-sections
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -dD --specs=nano.specs
CFLAGS += -Wno-unused-function
CFLAGS += $(OPTIMIZE)
CFLAGS += -I.
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar

CMSIS_INCLUDES = -I../CMSIS_5/CMSIS/Core/Include/ -I../CMSIS_5/CMSIS/DSP/Include/

CMSIS_LDFLAGS = -L../CMSIS_5/CMSIS/Lib/GCC \
                -larm_cortexM4lf_math \
                -lm \

ARM_PROFILE_CFLAGS=-DARM_MATH_CM4 -D__FPU_PRESENT \
 		$(CMSIS_INCLUDES) \
 		$(CFLAGS)

test/arm_cortex_m4/builtin_fpu_instructions.S : test/arm_cortex_m4/builtin_fpu_instructions.c
	$(CC) $(CMSIS_INCLUDES) $(CFLAGS) -S $< -o $@
