#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "groom/com.h"

#include "groom/usart_mux.h"
#include "groom/usart.h"

/* timeout after 100ms by default */
static uint8_t timeout = 100;

/*
 * Communication scheme:
 * Request data:
 *   Master sends single byte: bits 7-1 is the 7 bit data address.
 *   Bit 0 is 1 since it is a read.
 *   For example: to read from data address 3:
 *      7 6 5 4 3 2 1 0
 *      0 0 0 0 0 1 1 1
 *   Slave sends:
 *     An ACK for a successful read or a NACK for an unsuccessful one.
 *     A single byte for the message if ACK. ACK is 0x00. NACK is 0x01.
 *
 * Send data:
 *   Master sends a single byte: bits 7-1 is the 7 bit data address.
 *   Bit 0 is 0 since it is a write.
 *   For example to write to data address 3:
 *     7 6 5 4 3 2 1 0
 *     0 0 0 0 0 1 1 0
 *   Master then waits for an ack from the device. A successful ACK should
 *   be 0x00. A NACK should be 0x01.
 *   
 *   Master then sends data, which should be a single byte.
 *
 *   After the data is sent the master should wait for a final ACK.
 */

/*
 * Receive from the given address as a master. On success the uint8_t pointed
 * to by value will be set to the received value.
 *
 * Returns 0 on success.
 * Returns 1 on timeout
 * Returns 2 on NACK.
 */
uint8_t com_master_recv(uint8_t address, uint8_t *value)
{

}

/*
 * Send to the given address as a master. On success value will be
 * written to the device's address.
 *
 * Returns 0 on success.
 * Returns 1 on timeout.
 * Returns 2 on NACK.
 */
uint8_t com_master_send(uint8_t address, uint8_t value)
{

}

/*
 * Wait for a request as a slave. If the slave receives a request the
 * value pointed to by address will be set to the address requested,
 * including the read/write bit. You can check for the read/write bit
 * easily using the COM_IS_READ function and you can strip out this
 * bit from the address with COM_MASK_ADDRESS.
 *
 * Returns 0 on success.
 * Returns 1 on timeout.
 */
uint8_t com_slave_wait_for_request(uint8_t *address)
{

}

/*
 * Perform a slave receive (a master send) and store the data received in data.
 *
 * Returns 0 on success.
 * Returns 1 on timeout.
 */
uint8_t com_slave_recv(uint8_t *data)
{

}

/*
 * Perform a slave send (a master receive) by sending the value passed in.
 * The required ack is automatically sent as well.
 *
 * Returns 0.
 */
uint8_t com_slave_send(uint8_t value)
{

}

/*
 * Send a nack from a slave to indicate an invalid address
 * 
 * Returns 0.
 */
uint8_t com_slave_nack()
{
	
}

uint8_t com_set_timeout(uint8_t val)
{
	timeout = val;
}
