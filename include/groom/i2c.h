#ifndef GROOM_I2C_H
#define GROOM_I2C_H_

void i2c_init(void);

uint8_t i2c_write(uint8_t device_addr, char *p, uint16_t n);

uint8_t i2c_read2(uint8_t device_addr, char *p, uint16_t n, uint16_t a);

#endif /* GROOM_I2C_H_ */