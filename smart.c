#include <cmath.h>

#define Target_temp 22
#define Target_brightness 75


uint8_t Blind=



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

int day_parse(char *RTC){

}

void smart_control(int temp, int pd, uint8_t day_night, int motion){
	
		




}

void 
