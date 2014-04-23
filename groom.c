#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/usart.h"
#include "groom/encoder.h"
#include "groom/spi.h"
#include "groom/tft.h"
#include "dsh/dsh.h"

int get_temp();

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

	tft_fill_screen(ILI9341_BLACK);
	tft_set_text_color(ILI9341_WHITE, ILI9341_RED);

	sei();
	for(;;) {
		tft_set_cursor(0, 0);
		tft_set_text_size(1);

		tft_println("Status:\r\n");

		tft_printf("Temp: %d\r\n", get_temp());
		tft_printf("Encoder sample: %d\r\n", encoder_sample());
		tft_printf("Encoder val: %d\r\n", encoder_val());

		/*
		for(;;) {
			char c = usart_in();
			tft_text_write(c);
			usart_out(c);
		}
		*/
	}
}

int get_temp()
{
	static int dummy = 0;
	dummy++;

	return dummy;
}
