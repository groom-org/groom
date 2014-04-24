#include <avr/io.h>

#include "groom/spi.h"

/* see http://avrbeginners.net/architecture/spi/spi.html */

/* we can enable spi interrupts as well */
void spi_master_init(void)
{
	/* spi pins on port b MOSI SCK. Make them outputs. */
	DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK);

	/* Enable SPI, make us Master, and set clock rate to fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

	/* set SS as output and pull it high */
	DDRB |= (1 << 2);
	PORTB |= (1 << 2);

	spi_master_supah_speed();
}

/*
 * There are four options for the speed:
 * 0: CLK/4
 * 1: CLK/16
 * 2: CLK/64
 * 3: CLK/128
 */
void spi_master_set_speed(uint8_t s)
{
	s &= 0x3;

	/* 
	 * this is the lower two bits of the SPCR register, so don't
	 * bother shifting it
	 */
	SPCR |= s;
}

void spi_master_supah_speed(void)
{
	SPCR &= ~(0x3);
	SPSR |= (1 << SPI2X);
}

/* send the character d and return the result */
uint8_t spi_master_shift(uint8_t d)
{
	/* set the data and start transmission */
	SPDR = d;

	/* just spin until transmission finishes */
	while (!(SPSR & (1 << SPIF)));

	return SPDR;
}
