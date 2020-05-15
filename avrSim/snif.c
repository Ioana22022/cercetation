#include <util/delay.h>
#include <avr/interrupt.h>
#include "filter.h"
#include "usart.h"
#include "timer1.h"

#define BUFSZ (int)(sizeof(int)/sizeof(char))
#define SLAVESZ 3


volatile char state;
char saved_slave;

int searchID(int slaveID)
{
	int i;

	for(i = 0; i < SLAVESZ; i++)
	{
		if(filter[i].id == slaveID)
		{

			return 1;
		}
					
	}

	return -1;
}

int searchFunction(int slaveID, int fID)
{
	int i;

	for(i = 0; i < filter[slaveID - 1].length; i++)
	{
		if(filter[slaveID - 1].accepted_fct[i] == fID)
		{
			return 1;
		}
	}

	return -1;
}

ISR(TIMER1_COMPA_vect)
{
	state = 0;
	
	timer1_stop();

	PORTB ^= (1 << PB7);
}

int main()
{
	USART0_init();
	USART1_init();

	char c;
	char slaveID, fID, saved_slave;

	

	int i, rc;

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
				rc = searchID(slaveID);

				if(rc < 0)
				{
					state = 3;
					USART0_transmit(~c);
					break;
				}

				// if reached, slaveID found
				USART0_transmit(slaveID);
				state++;
	
				break;


			case 1:
				fID = c;
		
				rc = searchFunction(slaveID, fID);

				if(rc < 0)
				{	
					state = 3;	
					USART0_transmit(~c);		
					break;
				}

				USART0_transmit(c);
				state++;
				break;

				
			case 2:
				USART0_transmit(c);

				break;

			case 3:
				// flip the byte to break it and break the crc

				// send it broken
				USART0_transmit(~c);

				// send it broken untill timer expires
				break;
				
			default:
				// cannot reach, wait for timer to expire
				;
				break;
		}
	}

	return 0;
}
