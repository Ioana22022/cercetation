#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "timer1.h"
#include "search.h"
#include <stdlib.h>

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
	
	USART2_init();


	uint8_t c;
	uint8_t slaveID, fID;
	uint16_t addr[2]; // since address values are too large to keep in one byte, we shall store them in 2
	uint16_t address = 0; // result of the 2 chars from before;
	
	uint16_t value[2];
	uint8_t addr_no[2]; // number of addresses are stored in 2 bytes
	uint16_t reg_number;

	char rc;

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
				reg_number = 0;
				addr[0] = 0;
				addr[1] = 0;
				addr_no[0] = 0;
				addr_no[1] = 0;
				

				if(rc < 0)
				{
					state = 4;
					USART0_transmit(~c);
					break;
				}

				// if reached, slaveID found
				rc = 0;
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
				rc = 0;
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
				USART2_transmit(addr[0]);
				USART2_transmit(addr[1]);
				
				rc = searchNormalAddress(slaveID, address);

				if(rc == -1)
				{
					state = 4;
					USART0_transmit(~c);
					break;
				}
				//----------------------------------------
				//USART2_transmit(rc);
				//---------------------------------------

				if((fID != 0x05) && (fID != 0x06))
				{
					//then multiple registers/coils are involved, so number must be checked
					state = 6;
					USART0_transmit(c);
					break;
				}

				// if reached, then address is allowed, mark as passed			
				USART0_transmit(c);
				state = 5;

				break;

			case 4:
				// flip the byte to break it and break the crc

				// send it broken
				USART0_transmit(~c);

				// send it broken untill timer expires
				break;

			case 5:
				// pass!

				USART0_transmit(c);
				break;

			case 6:
				addr_no[0] = c;
				USART0_transmit(c);
				state++;

				break;

			case 7:
				addr_no[1] = c;
				reg_number = ((addr_no[0] << 8) | addr_no[1]);
				USART2_transmit(addr_no[0]);
				USART2_transmit(addr_no[1]);

				if(reg_number == 1)
				{
					// already checked at case 3
					USART0_transmit(c);
					state = 5;
					break;

				}
				
				//index 0 had already been checked at state 3
				//check all the next addresses that the slave might be allowed to access
				int i;

				for(i = 1; i < reg_number; i++)
				{

					rc = searchNormalAddress(slaveID, (address + i));

					// if one of the allowed addresses is the one we search for, then pass
					if(rc != -1)
					{
						USART0_transmit(c);
						state = 5;
						break;
					}
					
				}

				// if reached, all allowed addresses are different from this one, so this address is not allowed 
				if(i == reg_number)
				{
					state = 4;
					USART0_transmit(~c);
					break;
				}


				break;

				
				
			default:
				// cannot reach, wait for timer to expire
				;
				break;
		}
	}

	return 0;
}
