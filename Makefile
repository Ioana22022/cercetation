CC = gcc
CPPFLAGS = -I .
CFLAGS = -Wall -Wextra -g

.PHONY: all clean

all: sniffer

sniffer: sniffer.o

sniffer.o: sniffer.c


clean:
	-rm -f *.o *~ sniffer

