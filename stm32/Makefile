OBJECTS=main.c

FP_FLAGS ?= -mfloat-abi=hard -mfpu=fpv4-sp-d16

CC        = $(PREFIX)gcc
AR        = $(PREFIX)ar
MCU_FLAGS = -mcpu=cortex-m4 -mthumb $(FP_FLAGS)
LDSCRIPT  = stm32f407_olimex.ld
LDFLAGS   = -L$(OPENCM3_DIR)/lib -Wl,--relax -Wl,--gc-sections $(MCU_FLAGS) \
	    --static -nostartfiles -T$(LDSCRIPT)
LDLIBS    = -lopencm3_stm32f4 

CFLAGS    =       -Os \
		  -Wall -Wextra -Wimplicit-function-declaration \
		  -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
		  -Wundef -Wshadow \
		  -I./include -fno-common \
		  -I$(OPENCM3_DIR)/include \
		  -Wstrict-prototypes \
		  -ffunction-sections -fdata-sections -MD -DSTM32F4 \
		  $(MCU_FLAGS)
ASFLAGS   = $(CFLAGS)
PREFIX    = arm-none-eabi-

OPENCM3_DIR = libopencm3

all: main.elf

$(OPENCM3_DIR)/lib/opencm3_stm32f4.a: Makefile
	$(MAKE) PREFIX="$(PREFIX)" -C $(OPENCM3_DIR) \
	       CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)" lib/stm32/f4	

%.o: %.c Makefile
	$(CC) $(CFLAGS) -o $@ -c $<

_OBJECTS=$(patsubst %.c,%.o,$(OBJECTS))

main.elf: $(_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) 
