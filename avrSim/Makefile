PORT_M=/dev/ttyUSB0
PORT_W=/dev/ttyUSB0

all: snif.hex

uploadWifi: snif.hex
	avrdude -D -p atmega328p -c arduino -P ${PORT_W} -b 115200 -F  -U flash:w:$<:a


uploadMega: snif.hex
	avrdude -D -p m2560 -c stk500v2 -P ${PORT_M} -b 115200 -F  -U flash:w:$<:a

snif.hex: snif.elf

	avr-objcopy  -j .text -j .data -O ihex $^ $@

	avr-size snif.elf



snif.elf: snif.c filter.c usart.c timer1.c search.c has.c

	avr-gcc -mmcu=atmega2560 -DF_CPU=16000000 -Os -Wall -o $@ $^



clean:

	rm -rf snif.elf snif.hex
