#include <avr/io.h>

#include "groom/encoder.h"

void encoder_init(void)
{
	ENCODER_DDR &= ~(1 << ENCODER_A_PIN);
	ENCODER_DDR &= ~(1 << ENCODER_B_PIN);
}

uint8_t encoder_sample(void)
{
	uint8_t out = 0;
	uint8_t port_val = ENCODER_PORT;
	if (port_val & (1 << ENCODER_A_PIN)) {
		out |= 1;
	}

	if (port_val & (1 << ENCODER_B_PIN)) {
		out |= 2;
	}

	return out;
}