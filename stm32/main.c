/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include "search.h"
#include "filter.h"


#define INIT 0
#define CHECK_FID 1
#define CHECK_ADDR_1 2
#define CHECK_ADDR_2 3
#define DENIED 4
#define PASS 5
#define DEVICES_NUMBER_1 6
#define DEVICES_NUMBER_2 7
#define CHECK_VALUE_1 8
#define CHECK_VALUE_2 9
#define DATA_BYTES_NUMBER 10


volatile char state;

static void clock_setup(void)
{
	/* Enable GPIOC clock for LED & USARTs. */
  // USART6 is also on port C
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

  // USART3 Transmit
	rcc_periph_clock_enable(RCC_GPIOB);

  //TBD: USART6
	//rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART6. */
	rcc_periph_clock_enable(RCC_USART6);

	/* Enable clocks for USART3. */
	rcc_periph_clock_enable(RCC_USART3);

  rcc_clock_setup_pll(&rcc_hse_12mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

static void timer_setup()
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM2);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 * (These are actually default values after reset above, so this call
	 * is strictly unnecessary, but demos the api for alternative settings)
	 */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 1000000)); // -----> 320 Hz, counting to 65535

	/* Disable preload. */
  timer_set_period(TIM2, 1042);

	timer_enable_preload(TIM2);

  timer_enable_update_event(TIM2);

  timer_continuous_mode(TIM2);

  timer_update_on_overflow(TIM2);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	timer_enable_irq(TIM2, TIM_DIER_UIE);

	/* Counter enable. */
	timer_enable_counter(TIM2);
}

void tim2_isr(void)
{
  TIM2_SR &= ~(TIM_SR_UIF);
	gpio_toggle(GPIOD, GPIO0);	/* LED on/off */
  state = INIT;
}

void tim2_start()
{
  timer_enable_counter(TIM2);
}

void tim2_stop()
{
  timer_disable_counter(TIM2);
  
}



static void usart_setup(void)
{
	/* Setup USART6 parameters. */
	usart_set_baudrate(USART6, 38400);
	usart_set_databits(USART6, 8);
	usart_set_stopbits(USART6, USART_STOPBITS_1);
	usart_set_mode(USART6, USART_MODE_TX_RX);
	usart_set_parity(USART6, USART_PARITY_NONE);
	usart_set_flow_control(USART6, USART_FLOWCONTROL_NONE);

	/* Setup USART3 parameters. */
	usart_set_baudrate(USART3, 38400);
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_mode(USART3, USART_MODE_TX);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);


	/* Finally enable the USART. */
	usart_enable(USART6);
  usart_enable(USART3);
}

static void gpio_setup(void)
{
	/* Setup GPIO pin GPIO13 on GPIO port C for LED. */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);

	/* Setup GPIO pins for USART6 transmit. */
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);

	/* Setup GPIO pins for USART6 receive. */
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);

	/* Setup USART6 TX pin as alternate function. */
	gpio_set_af(GPIOC, GPIO_AF8, GPIO6);

	/* Setup USART6 RX pin as alternate function. */
	gpio_set_af(GPIOC, GPIO_AF8, GPIO7);

	/* Setup GPIO pins for USART3 transmit. */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);

	/* Setup USART3 TX pin as alternate function. */
	gpio_set_af(GPIOB, GPIO_AF7, GPIO10);
}

char USART6_receive()
{
  char data;
  data = usart_recv_blocking(USART6);

  return data;
}

void USART3_transmit(char data)
{
  usart_send_blocking(USART6, data);
}

int main(void)
{

	uint8_t c;
	uint8_t slaveID, fID;
	uint16_t addr[2]; // since address values are too large to keep in one byte, we shall store them in 2
	uint16_t address = 0; // result of the 2 chars from before;

	uint8_t value[2];
	uint16_t val;
	uint8_t addr_no[2]; // number of addresses are stored in 2 bytes
	uint16_t reg_number;

	char rc;

	clock_setup();
	gpio_setup();
	usart_setup();
  timer_setup();

	while (1) {
    c = USART6_receive();

    tim2_stop();

    TIM_CNT(TIM2) = 0;

    tim2_start();

    switch(state)
		{
			// init state
			case INIT:

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
					state = DENIED;
					USART3_transmit(~c);
					break;
				}

				// if reached, slaveID found
				rc = 0;
				USART3_transmit(slaveID);
				state = CHECK_FID;

				break;


			case CHECK_FID:
				fID = c;

				rc = searchFunction(slaveID, fID);

				if(rc < 0)
				{
					state = DENIED;
					USART3_transmit(~c);
					break;
				}

				// if reached, slaveid is allowed to perform action
				rc = 0;
				USART3_transmit(c);
				state = CHECK_ADDR_1;
				break;


			case CHECK_ADDR_1:
				// next comes the address, which is two bytes long;
				addr[0] = c;
				USART3_transmit(c);
				state = CHECK_ADDR_2;

				break;

			case CHECK_ADDR_2:
				addr[1] = c;
				address = ((addr[0] << 8) | addr[1]);

				rc = searchNormalAddress(slaveID, address);

				if(rc == -1)
				{
					state = DENIED;
					USART3_transmit(~c);
					break;
				}
				//----------------------------------------
				//USART2_transmit(rc);
				//---------------------------------------
				//if it is a single operation, then next byte to come takes part in the value
				if((fID == 0x05) || (fID == 0x06))
				{
					// then it's a single operation, so values must be checked
					state = CHECK_VALUE_1;
					USART3_transmit(c);
					break;
				}

				// if reached, then there are multiple registers involved, so we must filter the number of addresses
				state = DEVICES_NUMBER_1;
				USART3_transmit(c);

				break;


			case DENIED:
				// flip the byte to break it and break the crc

				// send it broken
				USART3_transmit(~c);

				// send it broken untill timer expires
				break;

			case PASS:
				// pass!

				USART3_transmit(c);
				break;

			case DEVICES_NUMBER_1:
				addr_no[0] = c;
				USART3_transmit(c);
				state = DEVICES_NUMBER_2;

				break;

			case DEVICES_NUMBER_2:
				addr_no[1] = c;
				reg_number = ((addr_no[0] << 8) | addr_no[1]);

				if(reg_number == 1)
				{
					// already checked at case 3
					USART3_transmit(c);
					// next comes number of bytes, then check value
					state = DATA_BYTES_NUMBER;
					break;

				}

				//index 0 had already been checked at state 3
				//check all the next addresses that the slave might be allowed to access
				int i;

				for(i = 1; i < reg_number; i++)
				{

					rc = searchNormalAddress(slaveID, (address + i));

					// if one of the checked addresses is not allowed, fail CRC, go to last state
					if(rc < 0)
					{
						USART3_transmit(~c);
						state = DENIED;
						break;
					}

				}

				// if reached, all allowed addresses are different from this one, so this address is not allowed
				/*if(i == reg_number)
				{
					state = 4;
					USART3_transmit(~c);
					break;
				}*/

				USART3_transmit(c);
				// if it's write operation
				if(fID > 0x04 && fID < 0x0F)
				{
					// check value
					state = CHECK_VALUE_1;
					break;
				}

				else if(fID >= 0x0F)
				{
					// next follows number of bytes
					state = DATA_BYTES_NUMBER;
					break;
				}
				// if it's read operation, then all is passed
				state = PASS;

				break;

			// 2 states, value check
			case CHECK_VALUE_1:
				value[0] = c;

				USART3_transmit(c);
				state = CHECK_VALUE_2;
				break;

			case CHECK_VALUE_2:
				value[1] = c;
				val = ((value[0] << 8) | value[1]);

				rc = searchNormalValue(slaveID, val);

				if(rc < 0)
				{
					state = DENIED;
					USART3_transmit(~c);
					break;
				}

				if(reg_number > 1)
				{
					USART3_transmit(c);
					reg_number--;
					state = CHECK_VALUE_1;
					break;
				}

				USART3_transmit(c);
				state = PASS;
				break;

			// number of data bytes to follow, useless information
			case DATA_BYTES_NUMBER:
				USART3_transmit(c);

				// check value to come
				state = CHECK_VALUE_1;
				break;


			default:
				// cannot reach, wait for timer to expire
				;
				break;
    
    }
  }

	return 0;
}
