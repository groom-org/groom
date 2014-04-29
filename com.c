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


/*
	Sending data, enter DeviceSendID and string pointer.
	Data will be sent as a string to the slave board and should end with \r
	'5' is for slave alpha '6' is for beta
	will return 1 if send successfully, 0 if fail

	sample usage

	uint8_t status;
	char control[50];
	sprintf(control,"ASMDSFE");
	status=com_sendata('5',control);

*/
uint8_t com_senddata(char DeviceSendID, char * data_send){
	
	if (DeviceSendID=='5') {
		PORTD &= ~(1 << PD2);  // Not pressed, LED off   select 0;
	}else if (DeviceSendID=='6') {
		PORTD |= 1 << PD2;  // select 1;
	}

	_delay_ms(100);
	
	int count;
	interruptstate=0;
	sei();//enable interrupt
	
	usart_out(DeviceSendID);
	
	_delay_ms(100);
	
	for(count=0;count<100;count++){
		_delay_ms(10);
		if (c=='A') {
			c='0';
			usart_outstring(data_send);
			return 1;
		}
	}
	c='0';
	cli();
	return 0;
	
}


char* com_requestdata(char DeviceDATAID){
	//set deviceID
	int count=0;
	
	if (DeviceDATAID=='3') {
		usart_mux_set(0);
		//PORTD &= ~(1 << PD2);  // Not pressed, LED off   select 0;
	}else if (DeviceDATAID=='4') {
		usart_mux_set(1);
		//PORTD |= 1 << PD2;  // select 1;
	}
	_delay_ms(100);
	interruptstate=1;
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
				interruptstate=0;
				return buffer;
			}
			
		}
	}
	
	cli();
}

// heartbeat function
uint8_t com_heartbeat(char DeviceID){	
	//set deviceID
	if (DeviceID=='1') {
		usart_mux_set(0);
		//PORTD &= ~(1 << PD2);  // Not pressed, LED off   select 0;
	}else if (DeviceID=='2') {
		usart_mux_set(1);
		//PORTD |= 1 << PD2;  // select 1;
	}
	
	_delay_ms(100);
	
	int count;
	interruptstate=0;
	sei();//enable interrupt
	usart_out(DeviceID);
	for(count=0;count<100;count++){
		_delay_ms(10);
		if (c=='R') {
			c='0';
			return 1;
		}
		if (c=='r') {
			c='0';
			return 2;
		}
	}
	c='0';
	cli();
	return 0;
}

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