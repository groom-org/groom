#ifndef GROOM_SPI_H__
#define GROOM_SPI_H__

void spi_master_init(void);

void spi_master_set_speed(uint8_t s);

uint8_t spi_master_shift(uint8_t d);

#endif /* GROOM_SPI_H__ */
