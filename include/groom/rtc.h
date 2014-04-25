#ifndef GROOM_RTC_H_
#define GROOM_RTC_H_

struct rtc_time {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
};

void rtc_init(void);

uint8_t rtc_get_time(struct rtc_time *time_buf);

#endif /* GROOM_RTC_H_ */