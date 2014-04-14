#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/encoder.h"
#include "groom/usart.h"

static int16_t val = 0;

ISR(INT1_vect)
{
	if (PIND & (1 << 3)) {
		return;
		usart_outstring("this shouldn't happen!\r\n");
	}

	if (PIND & (1 << 5)) {
		val++;
	} else {
		val--;
	}


	//char buf[128];
	//sprintf(buf, "INT1 triggered with new val %d\r\n", val);
	//usart_outstring(buf);

	//_delay_ms(1000);
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