#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "groom/tft.h"
#include "groom/spi.h"

#define SS_DDR DDRB
#define SS_PORT PORTB
#define DD_SS 0

#define DC_DDR DDRD
#define DC_PORT PORTD
#define DD_DC 7

static int16_t cursor_x, cursor_y;
static uint16_t text_fg, text_bg;
static uint8_t text_size = 1;
static uint8_t wrap = 0;
static uint8_t clear_newline;

/*
 * it's kinda hacky to include a C file... but
 * the Adafruit library does it, so...
 */
#include "glcdfont.c"
//#include "logo.c"


void tft_init(void)
{
	SS_DDR |= (1 << DD_SS);
	DC_DDR |= (1 << DD_DC);

	tft_command(0xEF);
	tft_data(0x03);
	tft_data(0x80);
	tft_data(0x02);

	tft_command(0xCF);
	tft_data(0x00);
	tft_data(0XC1);
	tft_data(0X30);

	tft_command(0xED);
	tft_data(0x64);
	tft_data(0x03);
	tft_data(0X12);
	tft_data(0X81);

	tft_command(0xE8);
	tft_data(0x85);
	tft_data(0x00);
	tft_data(0x78);

	tft_command(0xCB);
	tft_data(0x39);
	tft_data(0x2C);
	tft_data(0x00);
	tft_data(0x34);
	tft_data(0x02);

	tft_command(0xF7);
	tft_data(0x20);

	tft_command(0xEA);
	tft_data(0x00);
	tft_data(0x00);

	tft_command(ILI9341_PWCTR1);    //Power control
	tft_data(0x23);   //VRH[5:0]

	tft_command(ILI9341_PWCTR2);    //Power control
	tft_data(0x10);   //SAP[2:0];BT[3:0]

	tft_command(ILI9341_VMCTR1);    //VCM control
	tft_data(0x3e); //¶Ô±È¶Èµ÷½Ú
	tft_data(0x28);

	tft_command(ILI9341_VMCTR2);    //VCM control2
	tft_data(0x86);  //--

	tft_command(ILI9341_MADCTL);    // Memory Access Control
	tft_data(0x48);

	tft_command(ILI9341_PIXFMT);
	tft_data(0x55);

	tft_command(ILI9341_FRMCTR1);
	tft_data(0x00);
	tft_data(0x18);

	tft_command(ILI9341_DFUNCTR);    // Display Function Control
	tft_data(0x08);
	tft_data(0x82);
	tft_data(0x27);

	tft_command(0xF2);    // 3Gamma Function Disable
	tft_data(0x00);

	tft_command(ILI9341_GAMMASET);    //Gamma curve selected
	tft_data(0x01);

	tft_command(ILI9341_GMCTRP1);    //Set Gamma
	tft_data(0x0F);
	tft_data(0x31);
	tft_data(0x2B);
	tft_data(0x0C);
	tft_data(0x0E);
	tft_data(0x08);
	tft_data(0x4E);
	tft_data(0xF1);
	tft_data(0x37);
	tft_data(0x07);
	tft_data(0x10);
	tft_data(0x03);
	tft_data(0x0E);
	tft_data(0x09);
	tft_data(0x00);

	tft_command(ILI9341_GMCTRN1);    //Set Gamma
	tft_data(0x00);
	tft_data(0x0E);
	tft_data(0x14);
	tft_data(0x03);
	tft_data(0x11);
	tft_data(0x07);
	tft_data(0x31);
	tft_data(0xC1);
	tft_data(0x48);
	tft_data(0x08);
	tft_data(0x0F);
	tft_data(0x0C);
	tft_data(0x31);
	tft_data(0x36);
	tft_data(0x0F);

	tft_command(ILI9341_SLPOUT);    //Exit Sleep
	_delay_ms(120);
	tft_command(ILI9341_DISPON);    //Display on
}

void tft_begin(void)
{
	spi_master_supah_speed();
	SS_PORT &= ~(1 << DD_SS);
}

void tft_data_on(void)
{
	DC_PORT |= (1 << DD_DC);
}

void tft_data_off(void)
{
	DC_PORT |= (1 << DD_DC);
}

void tft_command(uint8_t c)
{
	tft_begin();

	DC_PORT &= ~(1 << DD_DC);
	spi_master_shift(c);

	tft_end();
}

void tft_data(uint8_t d)
{
	tft_begin();

	DC_PORT |= (1 << DD_DC);
	spi_master_shift(d);

	tft_end();
}

void tft_draw_pixel(int16_t x, int16_t y, int16_t color)
{
	tft_set_addr_window(x, y, x + 1, y + 1);

	tft_data_on();
	tft_begin();

	spi_master_shift(color >> 8);
	spi_master_shift(color);

	tft_end();
	tft_data_off();
}

void tft_set_addr_window(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	tft_command(ILI9341_CASET); // Column addr set
	tft_data(x0 >> 8);
	tft_data(x0 & 0xFF);     // XSTART
	tft_data(x1 >> 8);
	tft_data(x1 & 0xFF);     // XEND

	tft_command(ILI9341_PASET); // Row addr set
	tft_data(y0 >> 8);
	tft_data(y0);     // YSTART
	tft_data(y1 >> 8);
	tft_data(y1);     // YEND

	tft_command(ILI9341_RAMWR); // write to RAM
}

void tft_fill_screen(uint16_t color)
{
	tft_fill_rect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, color);
}

void tft_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	tft_set_addr_window(x, y, x + w - 1, y + h - 1);

	uint8_t hi = color >> 8, lo = color;

	tft_data_on();
	tft_begin();

	for (y = h; y > 0; y--) {
		for (x = w; x > 0; x--) {
			spi_master_shift(hi);
			spi_master_shift(lo);
		}
	}

	tft_end();
	tft_data_off();
}

void tft_set_cursor(int16_t x, int16_t y)
{
	cursor_x = x;
	cursor_y = y;
}

void tft_set_text_color(uint16_t fg, uint16_t bg)
{
	text_fg = fg;
	text_bg = bg;
}

void tft_set_text_size(uint8_t s)
{
	text_size = s;
}

void tft_set_clear_newline(uint8_t cn)
{
	clear_newline = cn;
}

void tft_println(char *s)
{
	while (*s != '\0') {
		tft_text_write(*s);
		s++;
	}
}

int tft_printf(char *format_string, ...)
{
	va_list varargs;
	char *string = NULL;
	int size;

	va_start(varargs, format_string);
	size = vsnprintf(string, 0, format_string, varargs);
	va_end(varargs);

	string = malloc(size + 1);
	if (!string) {
		return -1;
	}

	va_start(varargs, format_string);
	vsnprintf(string, size + 1, format_string, varargs);
	va_end(varargs);

	tft_println(string);
	free(string);

	return 0;
}

void tft_text_write(uint8_t c)
{
	if (c == '\n') {
		if (clear_newline) {
			tft_fill_rect(cursor_x, cursor_y, ILI9341_TFTWIDTH - cursor_x, text_size * 8, text_bg);
		}
		cursor_y += text_size * 8;
		cursor_x = 0;
	} else if (c == '\r') {
		//skip!
	} else {
		tft_draw_char(cursor_x, cursor_y, c, text_fg, text_bg, text_size);
		cursor_x += text_size * 6;
		if (wrap && (cursor_x > (ILI9341_TFTWIDTH - text_size * 6))) {
			cursor_y += text_size * 8;
			cursor_x = 0;
		}
	}
}

void tft_draw_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	if ((x >= ILI9341_TFTWIDTH)       ||
	    (y >= ILI9341_TFTHEIGHT)      ||
	    ((x + 6 * text_size - 1) < 0) ||
	    ((y + 8 * text_size - 1) < 0)) {
		return;
	}

	for (int8_t i = 0; i < 6; i++) {
		uint8_t line;
		if (i == 5) {
			line = 0x0;
		} else {
			line = pgm_read_byte(font + (c * 5) + i);
		}
		for (int8_t j = 0; j < 8; j++) {
			if (line & 0x1) {
				if (size == 1) {
					tft_draw_pixel(x + i, y + j, text_fg);
				} else {
					tft_fill_rect(x + i * text_size, y + j * text_size, text_size, text_size, text_fg);
				}
			} else if (text_bg != color) {
				if (size == 1) {
					tft_draw_pixel(x + i, y + j, text_bg);
				} else {
					tft_fill_rect(x + i * text_size, y + j * text_size, text_size, text_size, text_bg);
				}
			}
			line >>= 1;
		}
	}
}

int sign(int x) {
    if (x < 0){
		return -1;
	}
    else{
		return 1;
	}
}

void tft_draw_line(int xPrev, int yPrev, int x, int y, int16_t color){
    int x1 = xPrev;
    int y1 = yPrev;
    int x2 = x;
    int y2 = y;
    int dy = y2 - y1;
    int dx = x2 - x1;

    if (fabs(dy) > fabs(dx)){
        for(y = y1; y != y2; y += sign(dy)){
            x = x1 + (y - y1) * dx/dy;
			tft_draw_pixel((int)x, (int)y, color);
        }
    }
    else {
        for (x = x1; x != x2; x += sign(dx)){
            y = y1 + (x - x1) * dy/dx;
			tft_draw_pixel((int)x, (int)y, color);
        }
    }
    tft_draw_pixel((int)x2, (int)y2, color);

}

void draw_logotext(){
    int x = 40;
    int y = 250;
    draw_logo();
    draw_light_on();
    draw_light_off();
    draw_fan_on();
    draw_fan_off();
    tft_draw_line(x, y, x+45, y, 0x001F);
    tft_draw_line(x, y, x, y+42, 0x001F);
    tft_draw_line(x, y+42, x+42, y+42, 0x001F);
    tft_draw_line(x+42, y+29, x+42, y+42, 0x001F);
    tft_draw_line(x+28, y+29, x+42, y+29, 0x001F);

    tft_draw_line(x+47, y+14, x+77, y+14, 0xF800);
    tft_draw_line(x+47, y+14, x+47, y+43, 0xF800);
    tft_draw_line(x+47, y+43, x+77, y+43, 0xF800);
    tft_draw_line(x+77, y+14, x+77, y+43, 0xF800);

    tft_draw_line(x+81, y+14, x+111, y+14, 0xFFE0);
    tft_draw_line(x+81, y+14, x+81, y+43, 0xFFE0);
    tft_draw_line(x+81, y+43, x+111, y+43, 0xFFE0);
    tft_draw_line(x+111, y+14, x+111, y+43, 0xFFE0);

    tft_draw_line(x+115, y+15, x+139, y+15, 0x001F);
    tft_draw_line(x+115, y+15, x+115, y+36, 0x001F);
    tft_draw_line(x+115, y+36, x+139, y+36, 0x001F);
    tft_draw_line(x+139, y+15, x+139, y+36, 0x001F);
    tft_draw_line(x+129, y+36, x+135, y+47, 0x001F);
    tft_draw_line(x+112, y+47, x+141, y+47, 0x001F);
    tft_draw_line(x+112, y+61, x+141, y+61, 0x001F);
    tft_draw_line(x+112, y+47, x+112, y+61, 0x001F);
    tft_draw_line(x+141, y+47, x+141, y+61, 0x001F);

    tft_draw_line(x+148, y, x+148, y+42, 0x07E0);
    tft_draw_line(x+142, y, x+148, y, 0x07E0);
    tft_draw_line(x+142, y+42, x+156, y+42, 0x07E0);

    tft_draw_line(x+158, y+14, x+183, y+14, 0xF800);
    tft_draw_line(x+158, y+14, x+158, y+43, 0xF800);
    tft_draw_line(x+158, y+43, x+183, y+43, 0xF800);
    tft_draw_line(x+183, y+14, x+183, y+25, 0xF800);
    tft_draw_line(x+158, y+25, x+183, y+25, 0xF800);
}

void draw_light_on(){
    int x = 27;
    int y = 200;
    tft_draw_line(x+18, y+1, x+2, y+8, 0xFFFF);
    tft_draw_line(x+24, y+6, x+2, y+14, 0xFFFF);
    tft_draw_line(x+24, y+14, x+2, y+20, 0xFFFF);

    tft_draw_line(x+2, y+20, x+10, y+23, 0xFFFF);
    tft_draw_line(x+10, y+23, x+10, y+26, 0xFFFF);

    tft_draw_line(x+24, y+20, x+16, y+23, 0xFFFF);
    tft_draw_line(x+16, y+23, x+16, y+26, 0xFFFF);

    tft_draw_line(x+8, y+27, x+18, y+27, 0xFFFF);
    tft_draw_line(x+8, y+27, x+8, y+29, 0xFFFF);
    tft_draw_line(x+8, y+29, x+18, y+29, 0xFFFF);
    tft_draw_line(x+18, y+27, x+18, y+29, 0xFFFF);

    tft_draw_line(x+9, y+31, x+17, y+31, 0xFFFF);
    tft_draw_line(x+9, y+31, x+9, y+36, 0xFFFF);
    tft_draw_line(x+9, y+36, x+17, y+36, 0xFFFF);
    tft_draw_line(x+17, y+31, x+17, y+36, 0xFFFF);

    tft_draw_line(x+13, y-2, x+13, y-10, 0xFFFF);
    tft_draw_line(x+25, y+1, x+29, y-4, 0xFFFF);
    tft_draw_line(x+1, y+1, x-4, y-4, 0xFFFF);
}

void draw_light_off(){
    int x = 27;
    int y = 200;
    tft_draw_line(x+13, y-2, x+13, y-10, 0x0000);
    tft_draw_line(x+25, y+1, x+29, y-4, 0x0000);
    tft_draw_line(x+1, y+1, x-4, y-4, 0x0000);
}

void draw_fan_on(){
    int x = 67;
    int y = 200;
    tft_draw_line(x+0, y+0, x+30, y+30, 0xFFFF);
    tft_draw_line(x+30, y+0, x+0, y+30, 0xFFFF);
    tft_draw_line(x+15, y+7, x+15, y+23, 0xFFFF);
    tft_draw_line(x+7, y+15, x+23, y+15, 0xFFFF);

    tft_draw_line(x+0, y+0, x+0, y+8, 0xFFFF);
    tft_draw_line(x+30, y+0, x+22, y+0, 0xFFFF);
    tft_draw_line(x+0, y+30, x+8, y+30, 0xFFFF);
    tft_draw_line(x+30, y+30, x+30, y+22, 0xFFFF);

    tft_draw_line(x+22, y+0, x+11, y+11, 0xFFFF);
    tft_draw_line(x+0, y+8, x+11, y+19, 0xFFFF);
    tft_draw_line(x+30, y+22, x+19, y+11, 0xFFFF);
    tft_draw_line(x+8, y+30, x+19, y+19, 0xFFFF);



    tft_draw_line(x+8, y-3, x+6, y-6, 0xFFFF);
    tft_draw_line(x+16, y-3, x+14, y-6, 0xFFFF);
    tft_draw_line(x+24, y-3, x+22, y-6, 0xFFFF);
    tft_draw_line(x+6, y-6, x+8, y-9, 0xFFFF);
    tft_draw_line(x+14, y-6, x+16, y-9, 0xFFFF);
    tft_draw_line(x+22, y-6, x+24, y-9, 0xFFFF);
    tft_draw_line(x+8, y-9, x+6, y-12, 0xFFFF);
    tft_draw_line(x+16, y-9, x+14, y-12, 0xFFFF);
    tft_draw_line(x+24, y-9, x+22, y-12, 0xFFFF);
}

void draw_fan_off(){
    int x = 67;
    int y = 200;
    tft_draw_line(x+8, y-3, x+6, y-6, 0x0000);
    tft_draw_line(x+16, y-3, x+14, y-6, 0x0000);
    tft_draw_line(x+24, y-3, x+22, y-6, 0x0000);
    tft_draw_line(x+6, y-6, x+8, y-9, 0x0000);
    tft_draw_line(x+14, y-6, x+16, y-9, 0x0000);
    tft_draw_line(x+22, y-6, x+24, y-9, 0x0000);
    tft_draw_line(x+8, y-9, x+6, y-12, 0x0000);
    tft_draw_line(x+16, y-9, x+14, y-12, 0x0000);
    tft_draw_line(x+24, y-9, x+22, y-12, 0x0000);
}

void draw_logo(){
    int x = 5;
    int y = 270;
    tft_draw_line(x+15, y+0, x+29, y+5, 0xF800);
    tft_draw_line(x+29, y+5, x+16, y+10, 0xF800);
    tft_draw_line(x+16, y+10, x+15, y+9, 0xF800);
    tft_draw_line(x+15, y+0, x+15, y+9, 0xF800);

    tft_draw_line(x+14, y+0, x+2, y+6, 0xFFE0);
    tft_draw_line(x+2, y+6, x+14, y+9, 0xFFE0);
    tft_draw_line(x+14, y+9, x+14, y+0, 0xFFE0);

    tft_draw_line(x+1, y+7, x+16, y+11, 0x001F);
    tft_draw_line(x+16, y+11, x+16, y+29, 0x001F);
    tft_draw_line(x+16, y+29, x+1, y+21, 0x001F);
    tft_draw_line(x+1, y+7, x+1, y+21, 0x001F);

    tft_draw_line(x+29, y+6, x+17, y+11, 0x07E0);
    tft_draw_line(x+17, y+11, x+17, y+29, 0x07E0);
    tft_draw_line(x+17, y+21, x+21, y+17, 0x07E0);
    tft_draw_line(x+22, y+17, x+22, y+26, 0x07E0);
    tft_draw_line(x+22, y+26, x+29, y+21, 0x07E0);
    tft_draw_line(x+29, y+6, x+29, y+21, 0x07E0);
}



void tft_end(void)
{
	SS_PORT |= (1 << DD_SS);
}
