#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/usart.h"
#include "groom/encoder.h"
#include "groom/spi.h"
#include "groom/tft.h"

int main(void)
{
	/* init usart */
	usart_init();
	/* init encoder (including encoder interrupt) */
	encoder_init();
	/* init spi */
	spi_master_init();
	/* init tft */
	tft_init();

	int i, j;

	i = 0;
	for(;;) {
		/*
		char buf[128];
		//sprintf(buf, "sending data over spi %d\r\n", i);
		//usart_outstring(buf);
		spi_master_shift(0xAA);
		//tft_begin();
		//tft_data(0xAA);
		//tft_end();
		
		for (i = 0; i < ILI9341_TFTWIDTH; i++) {
			for (j = 0; j < ILI9341_TFTHEIGHT; j++) {
				char buf[128];
				sprintf(buf, "setting %d, %d\r\n", i, j);
				tft_draw_pixel(i, j, 0);
				usart_outstring(buf);
			}
		}
		
		i++;
		*/

		tft_fill_rect(0, 0, 60, 60, ILI9341_RED);
		tft_fill_rect(0, 0, 60, 60, ILI9341_BLUE);
	}

	/* turn on interrupts */
	sei();

	for(;;) {
		char buf[128];
		int16_t value = encoder_val();
		int8_t sample = encoder_sample();
		sprintf(buf, "value: %d sample: %d\r\n", value, sample);
		usart_outstring(buf);

		//_delay_ms(1000);
	}
}
