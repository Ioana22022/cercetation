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


static void clock_setup(void)
{
	/* Enable GPIOC clock for LED & USARTs. */
  // USART6 is also on port C
	rcc_periph_clock_enable(RCC_GPIOC);

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
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	//timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 50000));
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 62400000)); // -----> 320 Hz, counting to 65535
	//timer_set_prescaler(TIM2, 16);

	/* Disable preload. */
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);

	/* count full range, as we'll update compare value continuously */
	//timer_set_period(TIM2, 65535);
	timer_set_period(TIM2, 21846); // ------> 961
	//timer_set_period(TIM2, 781);

	/* Set the initual output compare value for OC1. */
//	timer_set_oc_value(TIM2, TIM_OC1, 32000);

	/* Counter enable. */
	timer_enable_counter(TIM2);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void)
{
  if(timer_get_flag(TIM2, TIM_SR_CC1IF))
  {
    timer_clear_flag(TIM2, TIM_SR_CC1IF);
  }

	gpio_toggle(GPIOC, GPIO13);	/* LED on/off */
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

int main(void)
{
	int i, j = 0, c = 0, data = 0;

	clock_setup();
	gpio_setup();
	usart_setup();
  timer_setup();

	/* Blink the LED (PD12) on the board with every transmitted byte. */
	while (1) {
      //data = usart_recv_blocking(USART6);
		  usart_send_blocking(USART6, c + '0'); /* USART6: Send byte. */
		  usart_send_blocking(USART3, c + '0'); /* USART6: Send byte. */


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
