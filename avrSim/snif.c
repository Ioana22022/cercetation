#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "filter.h"
#include "usart.h"

#define BUFSZ (int)(sizeof(int)/sizeof(char))
#define SLAVESZ 3


volatile char state;

void timer1_stop();

ISR(TIMER1_COMPA_vect)
{
	state = 0;
	
	timer1_stop();

	PORTB ^= (1 << PB7);
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

	

	int i;

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
