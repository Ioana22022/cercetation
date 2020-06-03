#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "timer1.h"
#include "search.h"

volatile char state;

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

	uint8_t c;
	uint8_t slaveID, fID;
	uint16_t addr[2]; // since address values are too large to keep in one byte, we shall store them in 2
	uint16_t address = 0; // result of the 2 chars from before;
	

	int rc;

	DDRB = (1 << PB7);

	// initialize state
	state = 0;

	timer1_init();

	sei();

#if HAS_SEARCH
	insertH();
	insertHId();
#endif

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
				
				// always reset the address;
				address = 0;

				if(rc < 0)
				{
					state = 4;
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
					state = 4;	
					USART0_transmit(~c);		
					break;
				}

				// if reached, slaveid is allowed to perform action
				USART0_transmit(c);
				state++;
				break;

				
			case 2:
				// next comes the address, which is two bytes long;
				addr[0] = c;
				USART0_transmit(c);
				state++;

				break;

			case 3:
				addr[1] = c;
				address = ((addr[0] << 8) | addr[1]);

/*				rc = searchNormalAddress(slaveID, address);

				if(rc < 0)
				{
					state = 4;
					USART0_transmit(~c);
					break;
				}
*/
				// if reached, then address is allowed			
				USART0_transmit(c);

				break;

			case 4:
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
