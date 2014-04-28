#include <avr/io.h>

#include "groom/usart_mux.h"

#ifdef GROOM_MASTER
#define USART_MUX_PORT PORTD
#define USART_MUX_DDR DDRD
#define USART_MUX_NUM 2
#endif /* GROOM_MASTER */

void usart_mux_init(void)
{
	/* set the port as an output */
	USART_MUX_DDR |= (1 << USART_MUX_NUM);

	/* set it to an initial value of 0 */
	usart_mux_set(0);
}

void usart_mux_set(uint8_t val)
{
	if (val) {
		USART_MUX_PORT |= (1 << USART_MUX_NUM);
		return;
	}

	USART_MUX_PORT &= ~(1 << USART_MUX_NUM);
}
