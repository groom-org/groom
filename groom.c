#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/usart.h"
#include "groom/encoder.h"
#include "groom/spi.h"
#include "groom/tft.h"
#include "dsh/dsh.h"

struct status_item {
	char *label;
	char *format_string;
	int (*fn)(void);
	uint16_t (*color_fn)(void);
};

int get_temp();
int get_spsr();
int get_spcr();
char *get_hello();
void init_status(struct status_item *items, size_t n, int x, int y);
void update_status(struct status_item *items, size_t n, int x, int y);

int main(void)
{
	/* init usart */
	usart_init();
	/* init encoder (including encoder interrupt) */
	encoder_init();
	/* init spi */
	spi_master_init();
	/* wait at least 1 second for all our SPI devices to properly power up */
	_delay_ms(1000);
	/* init tft (a SPI device) */
	tft_init();

	tft_fill_screen(ILI9341_BLACK);
	tft_set_text_color(ILI9341_WHITE, ILI9341_BLACK);
	tft_set_text_size(1);
	tft_set_clear_newline(0);

	sei();

	struct status_item mitems[] = {
		{
			"Temp",
			"%d",
			get_temp,
			NULL
		},
		{
			"SPSR",
			"0x%.2x",
			get_spsr,
			NULL
		},
		{
			"SPCR",
			"0x%.2x",
			get_spcr,
			NULL
		},
		/*
		{
			"Hello",
			"%s",
			get_hello,
			NULL
		},
		*/
		{
			"Encoder Value",
			"%d",
			encoder_val,
			NULL
		}
	};
	int nitems = sizeof(mitems) / sizeof(mitems[0]);
	int options_yloc = (nitems + 2) * 8;

	tft_set_cursor(0, options_yloc);
	tft_text_write('>');

	tft_set_cursor(8 * 2, options_yloc);
	tft_println("Set Temperature");
	tft_set_cursor(8 * 2, options_yloc + 8);
	tft_println("Do something else");
	tft_set_cursor(8 * 2, options_yloc + 16);
	tft_println("Do yet another thing");

	init_status(mitems, nitems, 0, 0);

	int cur_option = 0;
	int num_options = 3;
	int last_encoder_val = encoder_val();

	for(;;) {
		update_status(mitems, nitems, 0, 0);
		int new_encoder_val = encoder_val();
		int last_option = cur_option;

		if (new_encoder_val > last_encoder_val) {
			cur_option++;
		} else if (new_encoder_val < last_encoder_val) {
			cur_option--;
			if (cur_option == -1) {
				cur_option = num_options - 1;
			}
		}
		cur_option %= num_options;
		last_encoder_val = new_encoder_val;

		if (cur_option != last_option) {
			tft_set_cursor(0, options_yloc + 8 * last_option);
			tft_text_write(' ');
			tft_set_cursor(0, options_yloc + 8 * cur_option);
			tft_text_write('>');
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

void init_status(struct status_item *items, size_t n, int x, int y)
{
	tft_set_cursor(x, y);
	for (int i = 0; i < ILI9341_TFTWIDTH / 6; i++) {
		tft_text_write('=');
	}
	tft_text_write('\n');
	for (int i = 0; i < n; i++) {
		tft_println(items[i].label);
		tft_text_write('\n');
	}
	for (int i = 0; i < ILI9341_TFTWIDTH / 6; i++) {
		tft_text_write('=');
	}
}

void update_status(struct status_item *items, size_t n, int x, int y)
{
	tft_set_clear_newline(1);

	for (int i = 0; i < n; i++) {
		tft_set_cursor(ILI9341_TFTWIDTH / 2, y + 8 * (i + 1));
		tft_printf(items[i].format_string, items[i].fn());
		tft_text_write('\n');
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

int get_spcr()
{
	return SPCR;
}

char *get_hello()
{
	return "Hi there!";
}
