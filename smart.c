
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "groom/usart.h"
#include "groom/com.h"
#include "groom/rtc.h"
#include "groom/tft.h"


#define Target_temp 31
#define Target_brightness 50



uint8_t Blind_status=0;     // 1 up   0 down
uint8_t AC_status=0;      // 1 heat on 2 heat off  3 cool on 4 cool off 5 everything off
uint8_t Light_status=0; // 2 full 1 half 0 off
uint8_t Time_status=0; //1 for daytime, 0 night


int temp_parse(char * temp){
	return atoi(temp);
}

int pd_parse(char* pd){
	return atoi(pd);
}

int day_parse(){
	struct rtc_time t;
	uint8_t res = rtc_get_time(&t);
	if (res) {
		return 1;
	}

	if(t.hours>6 && t.hours<19){
		return 1;
	}else{
		return 0;
	}
}
//uint8_t Light_status; // 2 full 1 half 0 off
//uint8_t AC_status;      // 1 heat on 2 heat off  3 cool on 4 cool off
void smart_control(int temp, int pd, uint8_t day_night, int motion){

 
    if(Light_status==0){
			draw_light_off();
		}else{
			draw_light_on();
		}
   
	if ((Target_temp-1)>temp) {	//if too cold, turn on heat
		char buf[5];
		if(AC_status!=1){
		sprintf(buf, "%c%c%c\r", COOL_OFF, FAN_ON, HEAT_ON);
		com_senddata(SEND_BETA, buf);
		AC_status=1;	//update to say heat on

		}
	}
	else if ((Target_temp+1)<temp) { //if too hot, turn on air
		char buf[5];
		if(AC_status!=3){
		sprintf(buf, "%c%c%c\r", HEAT_OFF, FAN_ON, COOL_ON);
		com_senddata(SEND_BETA, buf);
		AC_status=3;	//update to say air on
		}
	}	
	else  { //perfect temperature, turn off
		char buf[5];
		sprintf(buf, "%c%c%c\r", HEAT_OFF, FAN_OFF, COOL_OFF);
		com_senddata(SEND_BETA, buf);
		AC_status=5;	//update to say everything off
	}	
	
	_delay_ms(200);
	
	if (day_night) {	//if it is daytime
		if (!Blind_status) {		//if blinds down, put up
			char buf[2];
			if(Blind_status!=1){
			sprintf(buf, "%c\r", BLINDS_UP);
			com_senddata(SEND_BETA, buf);
			Blind_status=1;
			}
		}		// Target_brightness
		_delay_ms(200);
		
		if (pd>=Target_brightness) {	//if outside light enough, lights off
			char buf[2];
			sprintf(buf, "%c\r", LIGHTS_OFF);
			com_senddata(SEND_BETA, buf);
			Light_status=0;
		}else if (pd<Target_brightness ) { //if dark & blinds on turn on lights
			char buf[2];
			if(Light_status!=2){
			sprintf(buf, "%c\r", LIGHTS_FULL);
			com_senddata(SEND_BETA, buf);
			Light_status=2;
			}

		}	
		_delay_ms(200);
	}
	if (!day_night) {	//if it is nighttime
		if (Blind_status) {		//if blinds up, put down
			char buf[2];
			sprintf(buf, "%c\r", BLINDS_DOWN);
			com_senddata(SEND_BETA, buf);
			Blind_status=0;

		}	
		_delay_ms(200);	
		
			
		if ((!Light_status)&& motion) {	//if lights off & motion detected

			char buf[2];
			sprintf(buf, "%c\r", LIGHTS_HALF);
			com_senddata(SEND_BETA, buf);
			Light_status=1; //update to say lights half on

		}	
		if (Light_status!=0 && !motion) {  //if lights on and no motion
			char buf[2];
			sprintf(buf, "%c\r", LIGHTS_OFF);
			com_senddata(SEND_BETA, buf);
			Light_status=0; //update to say lights off
		}	
		_delay_ms(200);
	}	
