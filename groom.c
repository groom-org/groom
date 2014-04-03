#include <stdio.h>

#include "groom/usart.h"
#include "groom/encoder.h"

int main(void)
{
	usart_init();
	encoder_init();

	for(;;) {
		char buf[128];
		uint8_t value = encoder_sample();
		sprintf(buf, "sample: %d\r\n", value);
		usart_outstring(buf);
	}
}
