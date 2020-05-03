#include <avr/io.h>
#include "timer1.h"

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
