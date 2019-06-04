#include <avr/io.h>
#include <util/delay.h>

#define BUFSZ (int)(sizeof(int)/sizeof(char))
#define MASK_ID 0xFC
#define START "ABC"

char pkg[100];
int filter(char c){
	int slaveID_rule = 0;
	/*int i;
	char start_seq[5];
	// received 4 bytes;
	for (i = 0; i < 4; i++) 
	{
		pkg[i] = c;
		start_seq[i] = c;
	}i
	if(strcmp(START, start_seq)) return false;
	pkg[4] = c;*/
	if((c & MASK_ID) != ('a' & MASK_ID)) return 0;
	return 1;
	
}
void USART0_init()
{
	// baud 38400
	UBRR0 = 25;

	// start transmitter
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

	// set frame format: 7 data b, 2 stop, no parity
	UCSR0C = (1 << USBS0) | (2 << UCSZ00);
}

void USART1_init()
{
	// same baud rate
	UBRR1 = 25;

	// start transmitter
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);

	// set frame format: 7 data, 2 stop, no parity
	UCSR1C = (1 << USBS1) | (2 << UCSZ10);
}


void USART1_transmit(char data)
{
	// wait for buffer to empty up
	while(!(UCSR1A & (1 << UDRE1)));

	UDR1 = data;
}


char USART1_receive()
{
	while(!(UCSR1A & (1 << RXC1)));

	return UDR1;
}

void USART0_transmit(char data)
{
	// wait for buffer to empty up
	while(!(UCSR0A & (1 << UDRE0)));

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
	USART0_init();
	USART1_init();

	char c;
	int pass = 1;
	while(1)
	{
		c = USART0_receive();
	
		pass = filter(c);
		if(pass) USART1_transmit(c);
		_delay_ms(10);
	}

	return 0;
}
