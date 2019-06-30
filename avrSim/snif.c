#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BUFSZ (int)(sizeof(int)/sizeof(char))
#define MASK_ID 0xFC
#define DEBUG 1
#define SLAVESZ 3


volatile char state;

void timer1_stop();

typedef struct pkg
{
	int id;
	int accepted_fct[8];
	int length;
}pkg;

ISR(TIMER1_COMPA_vect)
{
	state = 0;
	
	timer1_stop();

	PORTB ^= (1 << PB7);
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

void timer1_init()
{
	// set timer to count for a frequency of 1000 Hz equivalent of 38400 baud
	// rate
	OCR1A = 8333;

	// activtate interrupt at OCR1A_Compare
	TIMSK1 |= (1 << OCIE1A);
}

void timer1_start()
{
	TCCR1B = (1 << CS10);
}

void timer1_stop()
{
	TCCR1B = 0;
}

int main()
{
	USART0_init();
	USART1_init();

	char c;
	char slaveID, fID, saved_slave;
	pkg filter[3] = {
		{1, {1, 2, 3, 4, 5, 6, 15, 16}, 8},
		{2, {1, 2, 4, 5, 6, 16}, 7}, // slave 2 cannot force multiple coils
		{3, {1, 2, 3, 4, 5, 6, 15, 16}, 8}
	};

	

	int i, j;

	DDRB = (1 << PB7);

	// initialize state
	state = 0;

	timer1_init();

	sei();


	while(1)
	{
		// byte receive
		c = USART1_receive();

		timer1_stop();

		// reset timer
		TCNT1 = 0;

		timer1_start();

		switch(state)
		{
			// init state
			case 0:
				
				slaveID = c;
				for(i = 0; i < SLAVESZ; i++)
				{
					if(filter[i].id == slaveID)
					{
						USART0_transmit(slaveID);
						saved_slave = slaveID;
						state++;
						break;
					}
					
				}

				if(i == SLAVESZ)
				{
					state = 3;	
					USART0_transmit(~c);		
					break;
				}
				break;


			case 1:
				fID = c;
				//USART0_transmit(fID);
				//state++;
				//break;
		
				for(i = 0; i < filter[saved_slave - 1].length; i++)
				{
					if(filter[saved_slave - 1].accepted_fct[i] == fID)
					{
						USART0_transmit(fID);
						state++;
						break;
					}
				}
				if(i == SLAVESZ)
				{
					state = 3;	
					USART0_transmit(~c);		
					break;
				}
				break;

				
			case 2:
				USART0_transmit(c);

				break;

			case 3:
				// flip the byte to break it and break the crc

				// send it broken
				USART0_transmit(~c);
				break;
				
			default:
				;
				break;
		}
	}

	return 0;
}
