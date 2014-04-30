#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/button.h"

static uint8_t button_pressed = 0;
static uint8_t button_pressed_stack = 0;

void button_init(void)
{
	/* set pin C1 as an input */
	DDRC &= ~(1 << 1);

	/* pull C1 up */
	PORTC |= (1 << 1);

	/* enable pin change interrupt */
	PCICR  |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT9);
}

uint8_t button_was_pressed(void)
{
	if (button_pressed_stack) {
		button_pressed_stack = 0;
		return 1;
	}

	return 0;
}

uint8_t button_val(void)
{
	if (PINC & (1 << 1)) {
		return 1;
	}

	return 0;
}

ISR(PCINT1_vect)
{
	if (button_pressed) {
		if (button_val() == 1) {
			_delay_ms(5);
			if (button_val() == 1) {
				button_pressed = 0;
			}
		}
	} else {
		if (button_val() == 0) {
			_delay_ms(5);
			if (button_val() == 0) {
				button_pressed = 1;
				button_pressed_stack = 1;
			}
		}
	}
}
