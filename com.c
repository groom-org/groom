#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "groom/usart_mux.h"
#include "groom/usart.h"

volatile int i=0;
volatile uint8_t buffer[20]; //buffer for data passing
volatile uint8_t StrRxFlag=0; 
volatile uint8_t interruptstate=0; //interrupt state
volatile char c; // char for com protocol

//!! This protocol should be used in all uP.
#define HB_ALPHA '1'
#define HB_BETA '2'
#define READ_ALPHA '3'
#define READ_BETA '4'
#define SEND_ALPHA '5'
#define SEND_BETA '6'
#define ACK 'A'
#define ACTIVE_RESPONSE 'R'
#define ACTIVE_RESPONSE_MOTION 'r'
#define DEFAULT '0' 
#define COMMAND_MODE 0
#define TRANSMIT_MODE 1

/*
	Sending data, enter DeviceSendID and string pointer.
	Data will be sent as a string to the slave board and should end with \r
	'5' is for slave alpha '6' is for beta
	will return 1 if send successfully, 0 if fail

	sample usage:

	uint8_t status;
	char control[50];
	sprintf(control,"ASMDSFE");
	status=com_sendata('5',control);

*/
uint8_t com_senddata(char DeviceSendID, char * data_send){

	int count;
	
	if (DeviceSendID==SEND_ALPHA) {
		usart_mux_set(0);
	}else if (DeviceSendID==SEND_BETA) {
		usart_mux_set(1);
	}

	_delay_ms(100);
	interruptstate=COMMAND_MODE;
	sei();//enable interrupt
	usart_out(DeviceSendID);
	_delay_ms(100);
	
	for(count=0;count<100;count++){
		_delay_ms(10);
		if (c==ACK) {
			c=DEFAULT;
			usart_outstring(data_send);
			return 1;
		}
	}

	c=DEFAULT;
	cli();
	return 0;
	
}


char* com_requestdata(char DeviceDATAID){
	//set deviceID
	int count=0;
	
	if (DeviceDATAID==READ_ALPHA) {
		usart_mux_set(0);
		//PORTD &= ~(1 << PD2);  // Not pressed, LED off   select 0;
	}else if (DeviceDATAID==READ_BETA) {
		usart_mux_set(1);
		//PORTD |= 1 << PD2;  // select 1;
	}
	_delay_ms(100);
	interruptstate=TRANSMIT_MODE;
	usart_out(DeviceDATAID);
	sei();
	
	
	while (1) {
		_delay_ms(10);
		count++;
		if(StrRxFlag || count>100){    //time_out
			if(StrRxFlag){
            	StrRxFlag=0;                // Reset String received flag
				count=0;
				return buffer;
			}else{
				sprintf(buffer,"TIME_OUT");
				count=0;
				i=0;                //Reset buffer index
				interruptstate=COMMAND_MODE;
				return buffer;
			}
		}
	}
	
	cli();
}

// heartbeat function
uint8_t com_heartbeat(char DeviceID){	
	
	int count;
	//set deviceID
	if (DeviceID==HB_ALPHA) {
		usart_mux_set(0);
	}else if (DeviceID==HB_BETA) {
		usart_mux_set(1);
	}
	
	_delay_ms(100);
	interruptstate=COMMAND_MODE;
	sei();//enable interrupt
	usart_out(DeviceID);

	for(count=0;count<100;count++){
		_delay_ms(10);
		if (c==ACTIVE_RESPONSE) {
			c=DEFAULT;
			return 1;
		}
		if (c==ACTIVE_RESPONSE_MOTION) {
			c=DEFAULT;
			return 2;
		}
	}

	c=DEFAULT;
	cli();

	return 0;
}

//interrupt handler
ISR(USART_RX_vect)
{
	switch (interruptstate) {
		case COMMAND_MODE:
			while (!(UCSR0A & (1<<RXC0)));
			c=UDR0;         //Read USART data register
			break;
		case TRANSMIT_MODE:
			while (!(UCSR0A & (1<<RXC0)));
			buffer[i]=UDR0;         //Read USART data register
			if(buffer[i++]=='\r')   //check for carriage return terminator and increment buffer index
			{
				// if terminator detected
				StrRxFlag=1;        //Set String received flag
				buffer[i-1]=0x00;   //Set string terminator to 0x00
				i=0;                //Reset buffer index
				interruptstate=COMMAND_MODE;
			}
			break;
		default:
			break;
	}	
}