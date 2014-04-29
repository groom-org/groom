#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "groom/com.h"

#include "groom/usart_mux.h"
#include "groom/usart.h"

/* timeout after 100ms */
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

/* receive from address. Return 0 on success, 1 on failure. */
uint8_t com_master_recv(uint8_t address)
{

}

/* send the data to the given address. Return 0 on success. */
uint8_t com_master_send(uint8_t address, uint8_t value)
{

}

uint8_t com_slave_wait_for_request(uint8_t *address)
{

}

uint8_t com_slave_recv(uint8_t *data)
{

}

uint8_t com_slave_send(uint8_t value)
{

}

uint8_t com_set_timeout(uint8_t val)
{
	timeout = val;
}

/*
//interrupt handler
ISR(USART_RX_vect)
{
	switch (interruptstate) {
		case 0:
			while (!(UCSR0A & (1<<RXC0)));
			c=UDR0;         //Read USART data register
			break;
		case 1:
			while (!(UCSR0A & (1<<RXC0)));
			buffer[i]=UDR0;         //Read USART data register
			if(buffer[i++]=='\r')   //check for carriage return terminator and increment buffer index
			{
				// if terminator detected
				StrRxFlag=1;        //Set String received flag
				buffer[i-1]=0x00;   //Set string terminator to 0x00
				i=0;                //Reset buffer index
				interruptstate=0;
			}
			break;
		default:
			break;
	}	
}
*/