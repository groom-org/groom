#ifndef GROOM_SPI_H__
#define GROOM_SPI_H__

void spi_master_init(void);

uint8_t spi_master_txn(uint8_t d);

#endif /* GROOM_SPI_H__ */
