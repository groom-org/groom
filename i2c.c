#include <avr/io.h>

#include "groom/i2c.h"

#define BDIV ((F_CPU / 100000 - 16) / 2 + 1)

void i2c_init(void)
{
	/* set prescaler for 1 */
	TWSR = 0;
	/* set bit rate register */
	TWBR = BDIV;
}

uint8_t i2c_write(uint8_t device_addr, char *p, uint16_t n)
{
	uint8_t status;

	/* send start condition */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	/* wait for TWINT to be set */
	while (!( TWCR & (1 << TWINT )));
	status = TWSR & 0xf8;
	if (status != 0x08) {
		return status;
	}

	/* Load device address and R/W = 0 */
	TWDR = device_addr & 0xfe ;
	/* start transmission */
	TWCR = (1 << TWINT ) | (1 << TWEN );
	/* wait for TWINT to be set */
	while (!( TWCR & (1 << TWINT )));
	status = TWSR & 0xf8 ;
	if (status != 0x08) {
		return status;
	}

	while (n-- > 0) {
		/* put the next data byte in TWDR */
		TWDR = *p++;
		/* start the transmission */
		TWCR = (1 << TWINT) | (1 << TWEN);
		/* wait for TWINT to be set */
		while (!( TWCR & (1 << TWINT )));

		status = TWSR & 0xf8;
		/* check that data sent was OK */
		if (status != 0x28) {
			return status;
		}

		/* send stop condition */
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	}

	return 0;
}

uint8_t i2c_read2(uint8_t device_addr, char *p, uint16_t n, uint16_t a)
{
	uint8_t status;

	/* send start condition */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	while (!( TWCR & (1 << TWINT))); 
	status = TWSR & 0xf8;
	/* check taht STRT was sent OK */
	if (status != 0x08) {
		return status;
	}

	/* load device address and R / W = 0; */
	TWDR = device_addr & 0xfe;
	/* start transmission */
	TWCR = (1 << TWINT ) | (1 << TWEN );
	/* wait for TWINT to be set */
	while (!( TWCR & (1 << TWINT)));

	status = TWSR & 0xf8 ;
	/* check that SLA + W was sent OK */
	if ( status != 0x18) {
		return status;
	}

	/* load high byte of address */
	TWDR = a >> 8;
	/* start transmission */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/* wait for TWINT to be set */
	while (!(TWCR & (1 << TWINT)));
	status = TWSR & 0xf8 ;
	if ( status != 0x28) {
		return status;
	}
	/* load low byte of address */
	TWDR = a & 0xff;
	/* start transmission */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/* wait for twint to be set */
	while (!(TWCR & (1 << TWINT)));
	status = TWSR & 0xf8 ;
	/* check that data was sent OK */
	if ( status != 0x28 ) {
		return status;
	}
	/* put TWI into Master Receive mode by sending a repeated START condition */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	/* Send repeated start condition */
	/* wait for TWINT to be set */
	while (!( TWCR & (1 << TWINT )));
	status = TWSR & 0xf8;
	/* check that repeated start sent OK */
	if ( status != 0x10 ) {
		return status;
	}
	/* load device address and R / W = 1 */
	TWDR = device_addr | 0x01;
	/* send address + r */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/* wait for TWINT to be set */
	while (!(TWCR & (1 << TWINT)));
	status = TWSR & 0xf8;
	/* check that SLA + R was sent OK */
	if ( status != 0x40 ) {
		return status;
	}

	/* Read all but the last of n bytes from the slave device in this loop */
	n--;
	while (n-- > 0) {
		/* read byte and send ACK */
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		/* wait for TWINT to be set */
		while (!( TWCR & (1 << TWINT )));
		status = TWSR & 0xf8;
		/* check that data was received OK */
		if (status != 0x50) {
			return status;
		}
		/* read the data */
		*p++ = TWDR;
	}
	/*
	 * The last byte is read in a similar manner but the control register is set to not send an acknowledge after
	 * receiving it. After the byte has been received and stored in the arry, the stop condition is set to terminate
	 * the transfers.
	 */
	/* read the last byte with NOT ack sent */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/* wait for TWINT to be set */
	while (!( TWCR & (1 << TWINT )));
	status = TWSR & 0xf8 ;
	/* check that data received OK */
	if (status != 0x58) {
		return status;
	}
	/* read the data */
	*p++ = TWDR;
	/* send stop condition */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return 0;
}
