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
	
	uint8_t value[2];
	uint16_t val;
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
				value[0] = 0;
				value[1] = 0;
				val = 0;
				

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
				//if it is a single operation, then next byte to come takes part in the value
				if((fID == 0x05) || (fID == 0x06))
				{
					// then it's a single operation, so values must be checked
					state = 8;
					USART0_transmit(c);
					break;
				}

				// if reached, then there are multiple registers involved, so we must filter the number of addresses
				state = 6;
				USART0_transmit(c);

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

				if(reg_number == 1)
				{
					// already checked at case 3
					USART0_transmit(c);
					// next comes number of bytes, then check value
					state = 10;
					break;

				}
				
				//index 0 had already been checked at state 3
				//check all the next addresses that the slave might be allowed to access
				int i;

				for(i = 1; i < reg_number; i++)
				{

					rc = searchNormalAddress(slaveID, (address + i));
					USART2_transmit(rc);

					// if one of the allowed addresses is the one we search for, then pass
					if(rc > 0)
					{
						USART0_transmit(c);
						// if it's write operation
						if(fID > 0x04 && fID < 0x0F)
						{
							// check value
							state = 8;
							break;
						}
						else if(fID >= 0x0F)
						{
							// next follows number of bytes
							state = 10;
							break;
						}
						// if it's read operation, then all is passed
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

			// 2 states, value check
			case 8:
				value[0] = c;

				USART0_transmit(c);
				state++;
				break;

			case 9:
				value[1] = c;
				val = ((value[0] << 8) | value[1]);

				rc = searchNormalValue(slaveID, val);

				if(rc < 0)
				{	
					state = 4;	
					USART0_transmit(~c);		
					break;
				}

				if(reg_number > 1)
				{
					USART0_transmit(c);
					reg_number--;
					state = 8;
					break;
				}

				USART0_transmit(c);
				state = 5;
				break;

			// number of data bytes to follow, useless information
			case 10:
				USART0_transmit(c);

				// check value to come
				state = 8;
				break;

			/*case 11:	
				if(reg_number > 0)
				{
					//check value
					state = 8;
					reg_number--;
					break;
				}

				break;
			*/

				
				
				
			default:
				// cannot reach, wait for timer to expire
				;
				break;
		}
	}

	return 0;
}
