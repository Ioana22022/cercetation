#include <avr/io.h>
#include "usart.h"

void USART2_init()
{
	// baud 38400
	UBRR2 = 25;

	// start transmitter and enable RX interrupt
	UCSR2B = (1 << TXEN2) | (1 << RXEN2);

	// set frame format: 8 data b, 1 stop, no parity
	UCSR2C = (3 << UCSZ20);
}

void USART2_transmit(char data)
{
	// wait for buffer to get empty
	while(!(UCSR2A & (1 << UDRE2)));
	// send data
	UDR2 = data;
}

void USART2_print(const char *data)
{
	while(*data != '\0')
	{
		USART2_transmit(*data++);
	}
}
void USART0_init()
{
	// baud 38400
	UBRR0 = 25;

	// start transmitter and enable RX interrupt
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

	// set frame format: 8 data b, 1 stop, no parity
	UCSR0C = (3 << UCSZ00);
}

void USART1_init()
{
	// same baud rate
	UBRR1 = 25;

	// start transmitter
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);

	// set frame format: 8 data, 1 stop, no parity
	UCSR1C = (3 << UCSZ10);
}


void USART1_transmit(char data)
{
	// wait for buffer to get empty
	while(!(UCSR1A & (1 << UDRE1)));
	// send data
	UDR1 = data;
}


char USART1_receive()
{
	while(!(UCSR1A & (1 << RXC1)));

	return UDR1;
}


void USART1_print(const char *data)
{
	while(*data != '\0')
	{
		USART1_transmit(*data++);
	}
}

void USART0_transmit(char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

char USART0_receive()
{
	while(!(UCSR0A & (1 << RXC0)));

	return UDR0;
}

void USART0_print(const char *data)
{
	while(*data != '\0')
	{
		USART0_transmit(*data++);
	}
}
