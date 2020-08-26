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

static void clock_setup(void)
{
	/* Enable GPIOC clock for LED & USARTs. */
  // USART6 is also on port C
	rcc_periph_clock_enable(RCC_GPIOC);

  //TBD: USART6
	//rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART6);
}

static void usart_setup(void)
{
	/* Setup USART2 parameters. */
	usart_set_baudrate(USART6, 115200);
	usart_set_databits(USART6, 8);
	usart_set_stopbits(USART6, USART_STOPBITS_1);
	usart_set_mode(USART6, USART_MODE_TX_RX);
	usart_set_parity(USART6, USART_PARITY_NONE);
	usart_set_flow_control(USART6, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART6);
}

static void gpio_setup(void)
{
	/* Setup GPIO pin GPIO13 on GPIO port C for LED. */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

	/* Setup GPIO pins for USART6 transmit. */
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);

	/* Setup GPIO pins for USART6 receive. */
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);

	/* Setup USART6 TX pin as alternate function. */
	gpio_set_af(GPIOC, GPIO_AF8, GPIO6);

	/* Setup USART6 RX pin as alternate function. */
	gpio_set_af(GPIOC, GPIO_AF8, GPIO7);
}

int main(void)
{
	int i, j = 0, c = 0;

	clock_setup();
	gpio_setup();
	usart_setup();

	/* Blink the LED (PD12) on the board with every transmitted byte. */
  int data = 'P';
	while (1) {
		gpio_toggle(GPIOC, GPIO13);	/* LED on/off */
		usart_send_blocking(USART6, c + '0'); /* USART6: Send byte. */
    data = usart_recv_blocking(USART6);
		usart_send_blocking(USART6, data); /* USART6: Send byte. */


		c = (c == 9) ? 0 : c + 1;	/* Increment c. */
		if ((j++ % 80) == 0) {		/* Newline after line full. */
			usart_send_blocking(USART6, '\r');
			usart_send_blocking(USART6, '\n');
		}
    
		for (i = 0; i < 3000000; i++) {	/* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
