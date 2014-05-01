#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "groom/usart.h"
#include "groom/encoder.h"
#include "groom/spi.h"
#include "groom/tft.h"
#include "groom/button.h"
#include "groom/i2c.h"
#include "groom/rtc.h"
#include "groom/usart_mux.h"
#include "groom/com.h"

#define TEMP_SET_STRING "Manual Temperature:"
#define LIGHT_SET_STRING "Manual Lights:"

struct status_item {
	char *label;
	char *format_string;
	int (*fn)(void);
	uint16_t (*color_fn)(void);
};

char *get_temp();
int get_spsr();
int get_spcr();
char *get_rtc();
char *get_s1_status();
char *get_s2_status();
char *get_photodiode();
void init_status(struct status_item *items, size_t n, int x, int y);
void update_status(struct status_item *items, size_t n, int x, int y);

uint8_t temp_hb = 0;
uint8_t pd_hb = 0;

double temp_val=0;
double pd_val=0;
uint8_t motion_on=0;

int main(void)
{
	/* init usart */
	usart_init();
	/* init encoder (including encoder interrupt) */
	encoder_init();
	/* init spi */
	spi_master_init();
	/* init i2c */
	i2c_init();
	/* wait at least 1 second for all our SPI devices to properly power up */
	_delay_ms(1000);
	/* init tft (a SPI device) */
	tft_init();
	/* init button */
	button_init();
	/* init rtc */
	rtc_init();
	/* initialize the mux for the usart */
	usart_mux_init();

	tft_fill_screen(ILI9341_BLACK);
	tft_set_text_color(ILI9341_WHITE, ILI9341_BLACK);
	tft_set_text_size(1);
	tft_set_clear_newline(0);

	/* uncomment to set time */
	/*
	uint8_t rtc_run_res = rtc_run();
	usart_printf("rct_run res: 0x%.2x", rtc_run_res);
	*/

	sei();
	
	//tft_draw_logo(200, 200);
	//for(;;);

	struct status_item mitems[] = {
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
		},
		{
			"Button Value",
			"%d",
			button_val,
			NULL
		},
		{
			"RTC",
			"%s",
			get_rtc,
			NULL
		},
		{
			"Slave 1 Status",
			"%s",
			get_s1_status,
			NULL
		},
		{
			"Slave 2 Status",
			"%s",
			get_s2_status,
			NULL
		},
		{
			"Temperature",
			"%s",
			get_temp,
			NULL
		},
		{
			"Photodiode",
			"%s",
			get_photodiode,
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
	tft_println("Set Lights");
	tft_set_cursor(8 * 2, options_yloc + 16);
	for (int i = 0; i < ILI9341_TFTWIDTH / 6; i++) {
		tft_text_write('=');
	}
	uint8_t manual_temp = 70;
	tft_set_cursor(8 * 2, options_yloc + 24);
	tft_println(TEMP_SET_STRING);
	tft_set_cursor(8 * 2 + 8 * strlen(TEMP_SET_STRING), options_yloc + 24);
	tft_println(" 70");
	uint8_t manual_light = 1; //1 on, 0 off
	tft_set_cursor(8 * 2, options_yloc + 32);
	tft_println(LIGHT_SET_STRING);
	tft_set_cursor(8 * 2 + 8 * strlen(LIGHT_SET_STRING), options_yloc + 32);
	tft_println(" ON");

	init_status(mitems, nitems, 0, 0);

	int cur_option = 0;
	int num_options = 2;
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
		if(button_was_pressed()){ 
			int last_enc_val = encoder_val();
			int new_enc_val;
			switch(cur_option){
				case 0: //temperature
					tft_set_cursor(0, options_yloc + 24);
					tft_println("> ");
					tft_set_cursor(8*2 + 8 * strlen(TEMP_SET_STRING), options_yloc + 24);
					while(!button_was_pressed()){
						new_enc_val = encoder_val();
						if (new_enc_val > last_enc_val) {
							manual_temp++;
							tft_printf(" %d", manual_temp);
						} else if (new_encoder_val < last_encoder_val) {
							manual_temp--;
							tft_printf(" %d", manual_temp);
						}
						//////
						//send command to board alpha
						
						//////
						last_enc_val = new_enc_val;
					}
					break;
				case 1: //lights
					tft_set_cursor(0, options_yloc + 32);
					tft_println("> ");
					tft_set_cursor(8*2 + 8 * strlen(LIGHT_SET_STRING), options_yloc + 32);
					while(!button_was_pressed()){
						int new_enc_val = encoder_val();
						if (new_enc_val != last_enc_val) {
							manual_light = !manual_light;
							if(manual_light){
								tft_println("ON");
							}
							else{
								tft_println("OFF");
							}
							///////
							//send command to board alpha
							
							///////
						}
						last_enc_val = new_enc_val;
					}
					break;
			}
		}
		tft_set_cursor(0, options_yloc + 8 * cur_option);

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

char *get_temp()
{
	static int i = 0;
	static char buf[9];

	if (i != 10) {
		i++;
		return buf;
	} else {
		i = 0;
	}

	if (temp_hb) {
		char *val = com_requestdata('3');
		strcpy(buf, val);
		temp_val=atof(buf);
		return buf;
	} else {
		return buf;
	}
}

char *get_photodiode()
{
	static int i = 0;
	static char buf[9];

	if (i != 5) {
		i++;
		return buf;
	} else {
		i = 0;
	}

	if (pd_hb) {
		char *val = com_requestdata('4');
		strcpy(buf, val);
		pd_val=atof(buf);
		return buf;
	} else {
		return buf;
	}
}

int get_spsr()
{
	return SPSR;
}

int get_spcr()
{
	return SPCR;
}

char *get_rtc()
{
	// YYYY-MM-DD HH:MM:SS
	static char buf[20];
	struct rtc_time t;
	uint8_t res = rtc_get_time(&t);
	if (res) {
		sprintf(buf, "err: 0x%.2x", res);
		return buf;
	}
	sprintf(buf, "20%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
	        t.year,
	        t.month,
	        t.date,
	        t.hours,
	        t.minutes,
	        t.seconds);

	return buf;
}

char *get_s1_status()
{
	uint8_t res = com_heartbeat('1');

	if (res==1) {
		temp_hb = 1;
		motion_on = 0;
		return "active";
	}else if(res==2){
		temp_hb = 1;
		motion_on = 1;
		return "active";
    }

	temp_hb = 0;
	motion_on = 0;
	return "inactive";
}

char *get_s2_status()
{
	uint8_t res = com_heartbeat('2');

	if (res) {
		pd_hb = 1;
		return "active";
	}

	pd_hb = 0;
	return "inactive";
}
