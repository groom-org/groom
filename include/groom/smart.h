#ifndef GROOM_SMART_H_
#define GROOM_SMART_H_

int temp_parse(char * temp);
int pd_parse(char* pd);
int day_parse();
void smart_control(int temp, int pd, uint8_t day_night, int motion);

#endif /* GROOM_SMART_H_ */
