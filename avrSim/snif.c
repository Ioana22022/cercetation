#include <avr/io.h>
#include <util/delay.h>

void filter()
{
	DDRB |= (1 << PB7);
	PORTB ^= (1 << PB7);
}

int main()
{
	filter();
	while(1)
	{
		PORTB ^= (1 << PB7);
		_delay_ms(1000);
	}

	return 0;
}
