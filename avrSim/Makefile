PORT=/dev/ttyUSB0

all: snif.hex

upload: snif.hex
	avrdude -D -p m2560 -c stk500v2 -P /dev/ttyUSB0 -b 115200 -F  -U flash:w:$<:a

snif.hex: snif.elf

	avr-objcopy  -j .text -j .data -O ihex $^ $@

	avr-size snif.elf



snif.elf: snif.c

	avr-gcc -mmcu=atmega2560 -DF_CPU=16000000 -Os -Wall -o $@ $^



clean:

	rm -rf snif.elf snif.hex
