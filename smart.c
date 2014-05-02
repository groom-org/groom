#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "groom/usart.h"
#include "groom/com.h"
#include "groom/rtc.h"


#define Target_temp 22
#define Target_brightness 75


uint8_t Blind_status;     // 1 up   0 down
uint8_t AC_status;      // 1 heat on 2 heat off  3 cool on 4 cool off
uint8_t Light_status; // 2 full 1 half 0 off
uint8_t Time_status; //1 for daytime, 0 night
int Temperature;

int temp_parse(char * temp){

	int i=0,j=0;
	int temp_num=0;
	while(temp[i]!='.'){
		i++;	
	}

	if(i==0 || i==1 || i>=4)
		return 0;
	if(i==2){
		return (int)temp[0];
	}
	if(i==3){
		return (int)temp[0]*10+temp[1];
	}

}

int pd_parse(char* pd){
	return atoi(*pd);
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
	if ((Target_temp-5)>temp) {	//if too cold, turn on heat
		char buf[5];
		sprintf(buf, "%c%c%c\r", COOL_OFF, FAN_ON, HEAT_ON);
		com_senddata(SEND_BETA, buf);
		AC_status=1;	//update to say heat on
	}
	if ((Target_temp+5)<temp) { //if too hot, turn on air
		char buf[5];
		sprintf(buf, "%c%c%c\r", HEAT_OFF, FAN_ON, COOL_ON);
		com_senddata(SEND_BETA, buf);
		AC_status=3;	//update to say air on
	}	
	if (day_night) {	//if it is daytime
		if (~Blind_status) {		//if blinds down, put up
			char buf[2];
			sprintf(buf, "%c\r", BLINDS_UP);
			com_senddata(SEND_BETA, buf);
			Blind_status=1;
		}		
		if (Light_status!=2) {	//if lights not full, turn lights on
			char buf[2];
			sprintf(buf, "%c\r", LIGHTS_FULL);
			Light_status=2;
		}	
		
	}
	if (~day_night) {	//if it is nighttime
		if (Blind_status) {		//if blinds up, put down
			char buf[2];
			sprintf(buf, "%c\r", BLINDS_DOWN);
			com_senddata(SEND_BETA, buf);
			Blind_status=0;
		}		
		if (~Light_status&& motion) {	//if lights off & motion detected
			char buf[2];
			sprintf(buf, "%c\r", LIGHTS_HALF);
			com_senddata(SEND_BETA, buf);
			Light_status=1; //update to say lights half on
		}	
		if (Light_status!=0 && ~motion) {  //if lights on and no motion
			char buf[2];
			sprintf(buf, "%c\r", LIGHTS_OFF);
			com_senddata(SEND_BETA, buf);
			Light_status=0; //update to say lights off
		}	
	}	




}

