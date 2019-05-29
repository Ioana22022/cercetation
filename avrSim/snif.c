#include <avr/io.h>
#include <util/delay.h>

void filter();
void USART0_init()
{
	// baud 38400
	UBRR0 = 25;

	// start transmitter
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

	// set frame format: 7 data b, 2 stop, no parity
	UCSR0C = (1 << USBS0) | (2 << UCSZ00);
}

void USART0_transmit(char data)
{
	// wait for buffer to empty up
	while(!(UCSR0A) & (1 << UDRE0));

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


int main()
{
	while(1)
	{
		PORTB ^= (1 << PB7);
		_delay_ms(1000);
	}

	return 0;
}
