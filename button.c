#include <avr/io.h>

#include "groom/button.h"

void button_init(void)
{
	/* set pin C1 as an input */
	DDRC &= ~(1 << 1);

	/* pull C1 up */
	PORTC |= (1 << 1);
}

uint8_t button_was_pressed(void)
{
	return 0;
}

uint8_t button_val(void)
{
	if (PINC & (1 << 1)) {
		return 1;
	}

	return 0;
}