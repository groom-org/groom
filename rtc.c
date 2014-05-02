#include <avr/io.h>
#include <stdlib.h>

#include "groom/i2c.h"
#include "groom/rtc.h"

#define RTC_DEVICE_ADDRESS 0xd0

void rtc_init(void)
{
	/* nothing */
}

uint8_t rtc_get_time(struct rtc_time *time_buf)
{
	uint8_t abuf[1];
	uint8_t buf[7];

	abuf[0] = 0x00;

	int res = i2c_io(RTC_DEVICE_ADDRESS, abuf, sizeof(abuf), NULL, 0, buf, sizeof(buf));
	if (res) {
		return res;
	}

	/* YAY, BCD IS GREAT! */
	time_buf->seconds  = buf[0] & 0x0f;
	time_buf->seconds += ((buf[0] & 0x70) >> 4) * 10;

	time_buf->minutes  = buf[1] & 0x0f;
	time_buf->minutes += ((buf[1] & 0x70) >> 4) * 10;

	time_buf->hours    = buf[2] & 0x0f;
	time_buf->hours   += ((buf[2] & 0x30) >> 4) * 10;

	time_buf->day      = buf[3] & 0x07;

	time_buf->date     = buf[4] & 0x0f;
	time_buf->date    += ((buf[4] & 0x30) >> 4) * 10;

	time_buf->month    = buf[5] & 0x0f;
	time_buf->month   += ((buf[5] & 0x10) >> 4) * 10;

	time_buf->year     = buf[6] & 0x0f;
	time_buf->year    += ((buf[6] & 0xf0) >> 4) * 10;

	return 0;
}

uint8_t rtc_run(void)
{
	uint8_t buf[] = {
		0x30, // seconds
		0x55, // minutes
		0x18, // hours
		0x06, // day
		0x25, // date
		0x04, // month
		0x13  // year
	};
	uint8_t abuf[] = {
		0x00
	};

	return i2c_io(RTC_DEVICE_ADDRESS, abuf, sizeof(abuf), buf, sizeof(buf), NULL, 0);
}