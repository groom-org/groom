#include <avr/io.h>
#include <util/delay.h>

#include "groom/tft.h"
#include "groom/spi.h"

#define SS_DDR DDRB
#define SS_PORT PORTB
#define DD_SS 0

#define DC_DDR DDRD
#define DC_PORT PORTD
#define DD_DC 7

void tft_init(void)
{
	SS_DDR |= (1 << DD_SS);
	DC_DDR |= (1 << DD_DC);

	SS_PORT |= (1 << DD_SS);

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
	/* spi_master_set_speed(3); */
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

	tft_data(color >> 8);
	tft_data(color);
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

void tft_end(void)
{
	SS_PORT |= (1 << DD_SS);
}