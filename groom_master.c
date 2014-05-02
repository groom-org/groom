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
#include "groom/smart.h"

#define TEMP_SET_STRING "Temperature:"
#define LIGHT_SET_STRING "Lights:"
#define BLIND_SET_STRING "Blinds:"
#define HVAC_SET_STRING "HVAC:"
#define MAIN_MENU 0
#define MANUAL_TEMP 1
#define MANUAL_LIGHT 2
#define MANUAL_BLIND 3
#define MANUAL_HVAC 4


/*
	Everyone working on the groom_master

	Please use the #define in com.h to send or receive any command

	a table is included here

	//!! This protocol should be used in all uP.
#define HB_ALPHA '1'
#define HB_BETA '2'
#define READ_ALPHA '3'
#define READ_BETA '4'
#define SEND_ALPHA '5'
#define SEND_BETA '6'
#define ACK 'A'
#define ACTIVE_RESPONSE 'R'
#define ACTIVE_RESPONSE_MOTION 'r'
#define DEFAULT '0' 
#define COMMAND_MODE 0
#define TRANSMIT_MODE 1

// For commands to BETA
#define HEAT_ON     'H'
#define HEAT_OFF    'h'
#define COOL_ON     'C'
#define COOL_OFF    'c'
#define FAN_ON      'F'
#define FAN_OFF     'f'
#define BLINDS_UP   'U'
#define BLINDS_DOWN 'D'
#define LIGHTS_FULL 'L'
#define LIGHTS_HALF 'i'
#define LIGHTS_OFF  'l'


*/

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
char *get_motion();
int  return_temp_val();
void init_status(struct status_item *items, size_t n, int x, int y);
void update_status(struct status_item *items, size_t n, int x, int y);
int analyze_temp(double temp, double ideal);
uint8_t analyze_motion();
uint8_t analyze_time();
void send_blind_command(uint8_t blindcontrol_temp);
void send_light_command(uint8_t sensor_val,uint8_t time_val);
void send_temp_command(uint8_t temp_control_val);

uint8_t temp_hb = 0;
uint8_t pd_hb = 0;
double ideal_temp=0;
uint8_t motion_on=0;
uint8_t blindcontrol=0;
uint8_t blindcontrol_new=0;
uint8_t motion_val=0;
uint8_t tempcontrol=2;
uint8_t temp_control_new=2;
int temp_val=0;

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

	tft_draw_pixel(200,200,0xF800);
		tft_draw_pixel(200,201,0xF800);
			tft_draw_pixel(200,202,0xF800);
				tft_draw_pixel(200,203,0xF800);
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
			"Temp_int",
			"%d",
			return_temp_val,
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
		},
		{
			"MotionSensor",
			"%s",
			get_motion,
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
	tft_println("Set Blinds");
	tft_set_cursor(8 * 2, options_yloc + 24);
	tft_println("Set HVAC");
	tft_set_cursor(0, options_yloc + 32);
	for (int i = 0; i < ILI9341_TFTWIDTH / 6; i++) {
		tft_text_write('=');
	}
	uint8_t manual_temp = 22;
	tft_set_cursor(8 * 2, options_yloc + 40);
	tft_println(TEMP_SET_STRING);
	tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 40);
	tft_println("22");
	uint8_t manual_light = 1; //1 HI, 0 OFF, 2 LOW
	tft_set_cursor(8 * 2, options_yloc + 48);
	tft_println(LIGHT_SET_STRING);
	tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 48);
	tft_println("ON");
	uint8_t manual_blind = 0; //0 DOWN, 1 UP
	tft_set_cursor(8 * 2, options_yloc + 56);
	tft_println(BLIND_SET_STRING);
	tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 56);
	tft_println("DOWN");
	uint8_t manual_hvac = 4; //0 H_ON, 1 H_OFF, 2 C_ON, 3 C_OFF, 4 F_ON, 5 F_OFF
	tft_set_cursor(8 * 2, options_yloc + 64);
	tft_println(HVAC_SET_STRING);
	tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 64);
	tft_println("FAN ON");

	init_status(mitems, nitems, 0, 0);

	int cur_option = 0;
	int num_options = 4;
	int last_encoder_val = encoder_val();
	int menu_state = MAIN_MENU;

	for(;;) {
		update_status(mitems, nitems, 0, 0);
		int new_encoder_val = encoder_val();
		if(menu_state == MAIN_MENU){
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
			switch(cur_option){
				case 0:
					menu_state = MANUAL_TEMP;
					break;
				case 1:
					menu_state = MANUAL_LIGHT;
					break;
				case 2:
					menu_state = MANUAL_BLIND;
					break;
				case 3:
					menu_state = MANUAL_HVAC;
					break;
			}
			}
		}
		else if(menu_state == MANUAL_TEMP){
			tft_set_cursor(0, options_yloc + 40);
			tft_println("> ");
			tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 40);
			if (new_encoder_val > last_encoder_val) {
				manual_temp++;
				tft_printf("%d ", manual_temp);
			} else if (new_encoder_val < last_encoder_val) {
				manual_temp--;
				tft_printf("%d ", manual_temp);
			}
			char buf[3];
			if(manual_temp < return_temp_val() - 2){
				sprintf(buf, "%c%c\r", COOL_OFF, HEAT_ON);
				com_senddata(SEND_BETA, buf);
				tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 64);
				tft_println("HEAT ON ");
			}
			else if(manual_temp > return_temp_val() + 2){
				sprintf(buf, "%c%c\r", HEAT_OFF, COOL_ON);
				com_senddata(SEND_BETA, buf);
				tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 64);
				tft_println("COOL ON ");
			}

			if(button_was_pressed()){
				tft_set_cursor(0, options_yloc + 40);
				tft_text_write(' ');
				menu_state = MAIN_MENU;
				tft_set_cursor(0, options_yloc + 8 * cur_option);
			}
		}
		else if(menu_state == MANUAL_LIGHT){
			tft_set_cursor(0, options_yloc + 48);
			tft_println("> ");
			tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 48);
			uint8_t changed = 0;
			if (new_encoder_val > last_encoder_val) {
				changed = 1;
				manual_light++;

			} else if (new_encoder_val < last_encoder_val) {
				changed = 1;
				manual_light--;
			}
			manual_light %= 3;
			if(changed){
				char buf[2];
				switch(manual_light){
				case 0:
					tft_println("OFF");
					sprintf(buf, "%c\r", LIGHTS_OFF);
					com_senddata(SEND_BETA, buf);
					break;
				case 1:
					tft_println("HI ");
					sprintf(buf, "%c\r", LIGHTS_FULL);
					com_senddata(SEND_BETA, buf);
					break;
				case 2:
					tft_println("LOW");
					sprintf(buf, "%c\r", LIGHTS_HALF);
					com_senddata(SEND_BETA, buf);
					break;
				default:
					break;
				}
			}
			if(button_was_pressed()){
				tft_set_cursor(0, options_yloc + 48);
				tft_text_write(' ');
				menu_state = MAIN_MENU;
				tft_set_cursor(0, options_yloc + 8 * cur_option);
			}	
		}
		else if(menu_state == MANUAL_BLIND){
			tft_set_cursor(0, options_yloc + 56);
			tft_println("> ");
			tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 56);
			if(new_encoder_val != last_encoder_val){
				manual_blind = !manual_blind;
				if(manual_blind){
					tft_println("UP  ");
					char buf[2];
					sprintf(buf, "%c\r", BLINDS_UP);
					com_senddata(SEND_BETA, buf);
				}
				else{
					tft_println("DOWN");
					char buf[2];
					sprintf(buf, "%c\r", BLINDS_DOWN);
					com_senddata(SEND_BETA, buf);
				}
			}
			if(button_was_pressed()){
				tft_set_cursor(0, options_yloc + 56);
				tft_text_write(' ');
				menu_state = MAIN_MENU;
				tft_set_cursor(0, options_yloc + 8 * cur_option);
			}
			
		}
		else if(menu_state == MANUAL_HVAC){
			tft_set_cursor(0, options_yloc + 64);
			tft_println("> ");
			tft_set_cursor(ILI9341_TFTWIDTH / 2, options_yloc + 64);
			uint8_t changed = 0;
			if (new_encoder_val > last_encoder_val) {
				changed = 1;
				manual_hvac++;

			} else if (new_encoder_val < last_encoder_val) {
				changed = 1;
				manual_hvac--;
			}
			manual_hvac %= 6;
			if(changed){
				char buf[2];
				switch(manual_hvac){
				case 0:
					tft_println("HEAT ON ");
					sprintf(buf, "%c\r", HEAT_ON);
					com_senddata(SEND_BETA, buf);
					break;
				case 1:
					tft_println("HEAT OFF");
					sprintf(buf, "%c\r", HEAT_OFF);
					com_senddata(SEND_BETA, buf);
					break;
				case 2:
					tft_println("COOL ON ");
					sprintf(buf, "%c\r", COOL_ON);
					com_senddata(SEND_BETA, buf);
					break;
				case 3:
					tft_println("COOL OFF");
					sprintf(buf, "%c\r", COOL_OFF);
					com_senddata(SEND_BETA, buf);
					break;
				case 4:
					tft_println("FAN ON  ");
					sprintf(buf, "%c\r", FAN_ON);
					com_senddata(SEND_BETA, buf);
					break;
				case 5:
					tft_println("FAN OFF ");
					sprintf(buf, "%c\r", FAN_OFF);
					com_senddata(SEND_BETA, buf);
					break;
				default:
					break;
				}
			}
			if(button_was_pressed()){
				tft_set_cursor(0, options_yloc + 64
);
				tft_text_write(' ');
				menu_state = MAIN_MENU;
				tft_set_cursor(0, options_yloc + 8 * cur_option);
			}	
		}
		ideal_temp=(double) manual_temp;
		last_encoder_val = new_encoder_val;
		

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
	//do smart control stuff here maybe?
/*	blindcontrol_new=analyze_time();		
	send_blind_command(blindcontrol_new);
	blindcontrol=blindcontrol_new;
	motion_val=analyze_motion();
	send_light_command(motion_val,blindcontrol_new);
	temp_control_new=analyze_temp(temp_val, ideal_temp);
	send_temp_command(temp_control_new);
	tempcontrol=temp_control_new; */
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
		temp_val=temp_parse(val);
		return buf;
	} else {
		return buf;
	}
}

int return_temp_val(){
	return temp_val;
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
		return buf;
	} else {
		return buf;
	}
}

char *get_motion()
{

	if (motion_on) {
		return "Motion_Detected";
	} else {
		return "No_Motion";
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

	if (res == 1) {
		temp_hb = 1;
		motion_on = 0;
		return "active";
	}else if(res == 2){
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

uint8_t analyze_time(){
	struct rtc_time t;
	if (t.hours>0&&t.hours<0){  //edit this to include actual daytime hours
		return 1;
	}
	else {
		return 0;
	}
}
	
uint8_t analyze_motion() {
	if (motion_on) {
		return 1;
		}
		return 0;
}

int analyze_temp(double temp, double ideal) {
	double ideal_low;
	ideal_low=(ideal-7.0);
	if (temp>ideal) {
		return 0;
		} 
/*	if (temp<ideal_low) {	not sure why this section is causing error?
		return 1;
		} */
	return 2;
} 
	
void send_blind_command(uint8_t blindcontrol_temp) {
	if (blindcontrol!=blindcontrol_temp); {
		if (blindcontrol_temp) { //blinds go up if not already up and if its during the daytime
			com_senddata('6', 'U');
		}	
		if (!blindcontrol_temp) { //blinds go down if not already down, else nothing at night time
			com_senddata('6', 'D');
		}	
	}	
return;
}		

void send_light_command(uint8_t sensor_val,uint8_t time_val) {
	if (sensor_val&&time_val); {	//motion detected and daytime
		com_senddata('6', 'L');
	}	
	if (sensor_val&& ~time_val) { //motion detected and night time
		com_senddata('6', 'i');
	}	
return;
}	

void send_temp_command(uint8_t temp_control_val) {
	if (temp_control_val != tempcontrol) {
		if (temp_control_val==0) {
			com_senddata('6', 'h');
			com_senddata('6', 'F');
			com_senddata('6', 'C');
		}	
		if (temp_control_val==1) {
			com_senddata('6', 'c');
			com_senddata('6', 'F');
			com_senddata('6', 'H');
		}
		if (temp_control_val==2) {
			com_senddata('6', 'h');
			com_senddata('6', 'c');
			com_senddata('6', 'f');
		}
	}
}	
