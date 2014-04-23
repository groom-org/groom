#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/encoder.h"
#include "groom/usart.h"

static int16_t val = 0;

static void encoder_update(void);

ISR(INT1_vect)
{
	encoder_update();
}

ISR(TIMER1_OVF_vect)
{
	encoder_update();
}

void encoder_init(void)
{
	/* make sure pins 3 and 5 on D are intputs */
	DDRD &= ~(1 << 3);
	DDRD &= ~(1 << 5);

	//PORTD |= (1<<PD5)|(1<<PD3);

	/* falling edge on INT1 (PD3) to cause interrupt */
	//EICRA |=  (1 << ISC11);
	//EICRA &= ~(1 << ISC10);
	//EICRA = (1 << ISC11);
	EICRA = (EICRA & ~0xf3) | 0x8;

	/* enable INT1 */
	EIMSK |= (1 << 1);

	/* enable timer interrupts! */
	//TCCR1B |= _BV(CS10);
	//TIMSK1 |= _BV(TOIE1);
}

int16_t encoder_val(void)
{
	return val;
}

uint8_t encoder_sample(void)
{
	uint8_t out = 0;
	uint8_t port_val = PIND;
	if (port_val & (1 << 3)) {
		out |= 1;
	}

	if (port_val & (1 << 5)) {
		out |= 2;
	}

	return out;
}

void encoder_update(void)
{
	static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
	static uint8_t old_AB = 0;

	old_AB <<= 2;
	old_AB |= encoder_sample();
	val += ( enc_states[( old_AB & 0x0f )]);
}