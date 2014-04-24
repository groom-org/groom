#ifndef GROOM_SPI_H__
#define GROOM_SPI_H__

#include <stdint.h>

#define DD_MOSI 3
#define DD_MISO 4
#define DD_SCK 5
#define DDR_SPI DDRB
#define SPI_PORT PORTB

void spi_master_init(void);

void spi_master_set_speed(uint8_t s);

void spi_master_supah_speed(void);

uint8_t spi_master_shift(uint8_t d);

#endif /* GROOM_SPI_H__ */
