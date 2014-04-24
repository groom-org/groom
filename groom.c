#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/usart.h"
#include "groom/encoder.h"
#include "groom/spi.h"
#include "groom/tft.h"
#include "dsh/dsh.h"

int get_temp();
int get_spsr();
char *get_hello();

struct menu_item {
	char *label;
	char *format_string;
	int (*fn)(void);
};

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
	tft_set_text_color(ILI9341_WHITE, ILI9341_BLACK);
	tft_set_text_size(1);
	tft_set_clear_newline(0);

	sei();

	struct menu_item mitems[] = {
		{
			"Temp",
			"%d",
			get_temp
		},
		{
			"SPSR",
			"%d",
			get_spsr
		},
		{
			"Hello",
			"%s",
			get_hello
		},
		{
			"Encoder Value",
			"%d",
			encoder_val
		}
	};
	int nitems = sizeof(mitems) / sizeof(mitems[0]);

	tft_set_cursor(0, 0);
	for (int i = 0; i < nitems; i++) {
		tft_printf(mitems[i].label);
		tft_text_write('\n');
	}

	int half_width = ILI9341_TFTWIDTH / 2;

	for(;;) {
		tft_set_clear_newline(1);

		for (int i = 0; i < nitems; i++) {
			tft_set_cursor(half_width, 8 * i);
			tft_printf(mitems[i].format_string, mitems[i].fn());
		}
		/*
		tft_set_cursor(half_width, 8);
		tft_printf("%d\n", get_temp());
		tft_set_cursor(half_width, 16);
		tft_printf("%d\n", encoder_sample());
		tft_set_cursor(half_width, 24);
		tft_printf("%d\n", encoder_val());
		tft_set_cursor(half_width, 32);
		tft_printf("%d\n", SPCR);
		tft_set_cursor(half_width, 40);
		tft_printf("%d\n", SPSR);
		*/

		/* print the divider */
		//for (int i = 0; i < ILI9341_TFTWIDTH / 6; i++) {
		//	tft_text_write('=');
		//}

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

int get_spsr()
{
	return SPSR;
}

char *get_hello()
{
	return "Hi there!";
}
