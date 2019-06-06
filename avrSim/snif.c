#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BUFSZ (int)(sizeof(int)/sizeof(char))
#define MASK_ID 0xFC
#define DEBUG 1

#if 0
int filter(char c){
	int slaveID_rule = 0, fctId = 0;
	static int count = 0;

#ifndef DEBUG
	pkg[count++] = c;

	// received 4 bytes, and check the correctness of the first 3.5 bytes
	if(count == 4)
	{
		
		if((pkg[0] != start_seq[0]) || (pkg[1] != start_seq[1]) || (pkg[2] != start_seq[2]) || (pkg[3] & 0xFC) != (start_seq[3] & 0xFC)) return 0;
	}
#endif

	if((c & MASK_ID) != ('a' & MASK_ID)) return 0;
	return 1;
	
}
#endif


volatile char state = '0';
ISR(TIMER1_COMPA_vect)
{
	state = '0';
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


void USART1_print(const char *data)
{
	while(*data != '\0')
	{
		USART1_transmit(*data++);
	}
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

void timer1_init()
{
	// set timer to count for a frequency of 1372 Hz equivalent of 38400 baud
	// rate
	OCR1A = 0x2D8E;
	TCCR1B |= (1 << CS10);

	// activtate interrupt at OCR1A_Compare
	TIMSK1 |= (1 << OCIE1A);
}

int main()
{
	USART0_init();
	USART1_init();

	char c;
	char slaveID, fID;

	while(1)
	{
		// byte receive
		c = USART0_receive();

		// reset timer
		TCNT1 = 0;
	

		// enable interrupts
		sei();
		// start timer
		timer1_init();

		switch(state)
		{
			// init state
			case '0':
				
				slaveID = c;
				USART1_print("slaveId: ");
				USART1_transmit(slaveID);
				state++;
				
				break;

			case '1':
				fID = c;
				USART1_print("functionID: ");
				USART1_transmit(fID);
				state++;
				
				break;

			case '2':

				USART1_transmit(c);
				break;
		}

		_delay_ms(10);
	}

	return 0;
}
