#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include <mbedtls/poly1305.h>
#include "mbedtls_mod/stm32_rng.h"


#define ANALYZER1_PORT GPIOD
#define ANALYZER1_BIT GPIO0

#define ANALYZER2_PORT GPIOD
#define ANALYZER2_BIT GPIO1


static void clock_setup(void)
{
  rcc_clock_setup_pll(&rcc_hse_12mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

	/* Enable clocks for USART6. */
	rcc_periph_clock_enable(RCC_USART6);

	/* Enable clocks for USART3. */
	rcc_periph_clock_enable(RCC_USART3);
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

	/* Analyzer GPIOs. */
	gpio_mode_setup(ANALYZER1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ANALYZER1_BIT);
	gpio_mode_setup(ANALYZER2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ANALYZER2_BIT);

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
	uint8_t key[32];
	for (i=0; i<32; i++) {
		key[i] = (13 * i * i + 67 * i + 2);
	}

	clock_setup();
	gpio_setup();
	usart_setup();

	STM32RNG_Init();

	/** Initialize Poly1305 state. */
	int res;
	mbedtls_poly1305_context mac_state;
	mbedtls_poly1305_init(&mac_state);

	res = mbedtls_poly1305_starts(&mac_state, key);
	if (res != 0) {
		usart_send_blocking(USART3, 'F');
		usart_send_blocking(USART3, -res);
		return;
	}

	uint8_t hash_byte = 0;

	while (1) {
		hash_byte ++;
		gpio_clear(ANALYZER1_PORT, ANALYZER1_BIT);
		/* Compute MAC. */
		res = mbedtls_poly1305_update(&mac_state, &hash_byte, 1);
		gpio_set(ANALYZER1_PORT, ANALYZER1_BIT);
		if (res != 0) {
			usart_send_blocking(USART3, 'G');
			usart_send_blocking(USART3, -res);
			return;
		}
		usart_send_blocking(USART3, 'Y');

		for (i = 0; i < 3000000; i++) {	/* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
