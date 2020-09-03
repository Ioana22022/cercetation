APP=main
OBJECTS=$(APP).c mbedtls_mod/stm32_rng.c search.c filter.c has.c

FP_FLAGS ?= -mfloat-abi=hard -mfpu=fpv4-sp-d16

CC        = $(PREFIX)gcc
AR        = $(PREFIX)ar
GDB        = $(PREFIX)gdb
MCU_FLAGS = -DSTM32F4 -mcpu=cortex-m4 -mthumb $(FP_FLAGS)
LDSCRIPT  = stm32f407_olimex.ld
LDFLAGS   = -L$(OPENCM3_DIR)/lib -Wl,--relax -Wl,--gc-sections $(MCU_FLAGS) \
	    --static -nostartfiles -T$(LDSCRIPT)
LDLIBS    = -lopencm3_stm32f4 

INCLUDES = -I./include -fno-common \
		  -I$(OPENCM3_DIR)/include -I"$(MBEDTLS_PATH)/include"

CFLAGS_COMMON = -Os \
		  -Wall -Wextra -Wimplicit-function-declaration \
		  -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
		  -Wundef -Wshadow -Wstrict-prototypes \
		  -ffunction-sections -fdata-sections \
		  $(MCU_FLAGS)
CFLAGS = $(INCLUDES) $(CFLAGS_COMMON) -MD
ASFLAGS   = $(CFLAGS)
PREFIX    = arm-none-eabi-

OPENCM3_DIR = libopencm3

OPENOCD = openocd
OPENOCD_ARGS = -f interface/stlink-v2.cfg -f target/stm32f4x.cfg 

all: $(APP).elf

clean: 
	rm main.elf *.o *.d

include mbedtls_mod/Makefile.mbedtls

flash:
	openocd $(OPENOCD_ARGS) -c "program ./$(APP).elf verify reset exit"

gdb:
	$(GDB) -ex 'target remote | openocd $(OPENOCD_ARGS) -c "gdb_port pipe;"' \
				$(APP).elf

$(OPENCM3_DIR)/lib/libopencm3_stm32f4.a: Makefile
	$(MAKE) PREFIX="$(PREFIX)" -C $(OPENCM3_DIR) \
	       CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)" lib/stm32/f4	

%.o: %.c Makefile | $(OPENCM3_DIR)/lib/libopencm3_stm32f4.a
	$(CC) $(CFLAGS) -o $@ -c $<

_OBJECTS=$(patsubst %.c,%.o,$(OBJECTS))

$(APP).elf: $(_OBJECTS) | $(OPENCM3_DIR)/lib/libopencm3_stm32f4.a $(MBEDTLS_LIB)
	$(CC) $(LDFLAGS) $^ $(OPENCM3_DIR)/lib/libopencm3_stm32f4.a $(MBEDTLS_LIB) -o $@  

