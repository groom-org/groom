/*************************************************************
 *       at328-0.c - Demonstrate simple I/O functions of ATmega328
 *
 *       Program loops turning PC0 on and off as fast as possible.
 *
 * The program should generate code in the loop consisting of
 *   LOOP:   SBI  PORTC,0        (2 cycles)
 *           CBI  PORTC,0        (2 cycles)
 *           RJMP LOOP           (2 cycles)
 *
 * PC0 will be low for 4 / XTAL freq
 * PC0 will be high for 2 / XTAL freq
 * A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
 *
 * Revision History
 * Date     Author      Description
 * 09/14/12 A. Weber    Initial Release
 * 11/18/13 A. Weber    Renamed for ATmega328P
 * 04/27/14 H. Xiao     Re-designed for slave uP (thermometer and motion sensor)
 *************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#include "groom//usart.h"



#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz

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
 
// device ID and address
volatile int i=0;
volatile uint8_t buffer[20];
volatile uint8_t StrRxFlag=0;
volatile char c='0';
char buf[256]; //string for thermometer
volatile uint8_t interruptstate=0; //interrupt state


/*
write the control you want to do in this function
*/
void control(char *command){
	//do whatever you want
}

//I/O init
void io_pin_init()
{
  usart_outstring("Initializing I/O pins...\r\n");

  DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output
  DDRC |= 1 << DDC2;          // Set PORTC bit 3 for output
  DDRC |= 1 << DDC4;          // Set PORTC bit 4 for output
  DDRC |= 1 << DDC5;          // Set PORTC bit 5 for output

  DDRC = 0xff;
}

//adc 
void adc_init(void){ 
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
	ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)
	ADCSRA |= (1<<ADEN);                //Turn on ADC
	ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}
uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;                    //Clear the older channel that was read
	ADMUX |= channel;                //Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);                //Starts a new conversion
	while(ADCSRA & (1<<ADSC));            //Wait until the conversion is done
	return ADCW;                    //Returns the ADC value of the chosen channel
}


//Thermostat controls
//PC5: Cool - Blue Wire - Light Three
//PC4: Heat - Green Wire - Light Two
//PC2: Fan - Yellow Wire - Light One
//
void thermo_fan_on()
{
  usart_printf("Turning Fan On\r\n");
  PORTC |= (1 << PC2);      // Set PC2 to a 1
}

void thermo_fan_off()
{
  usart_printf("Turning Fan Off\r\n");
  PORTC &= ~(1 << PC2);  //Set PC2 to 0
}

void thermo_turn_off()
{
  usart_printf("Turning All Systems Off\r\n");
  PORTC &= ~(1 << PC4);  //Set PC4 to 0
  PORTC &= ~(1 << PC5);  //Set PC5 to 0
}

void thermo_call_for_heat()
{
  thermo_turn_off();
  usart_printf("Turning Heat On\r\n");
  PORTC |= (1 << PC4);  //Set PC4 to 1
}

void thermo_call_for_cool()
{
  thermo_turn_off();
  usart_printf("Turning Cool On\r\n");
  PORTC |= (1 << PC5);  //Set PC5 to 1
}


// data sender general
void senddata(){
	
	uint16_t result = read_adc(1);	
	char buf[128];
	sprintf(buf, "%d\r", result);
	usart_outstring(buf);
	c=DEFAULT;
	
}

void receivecommand(){

	int count=0;
	
	while (1) {
		_delay_ms(10);
		count++;
		if(StrRxFlag || count>200){    //time_out
			if(StrRxFlag){
            	StrRxFlag=0;                // Reset String received flag
				count=0;
			}else{
				sprintf(buffer,"TIME_OUT");
				count=0;
				i=0;  
				interruptstate=COMMAND_MODE;     //Reset buffer index
			}
			break;
		}
	}
	
	control(buffer);	
} 

int main(void)
{
  int command = 0;

  /* for 9600 baud on with 9.304MHz clock */
  usart_init();
  adc_init();
  io_pin_init();

  
  while(1)
  {
    //usart_printf("Looping... command=%d\r\n", command);

    if(command % 2 == 0) 
    {
      PORTC |= 1 << PC0;   
    }
    else 
    {
      PORTC &= ~(1 << PC0);
    }

    switch(command)
    {
    case 0:
      thermo_call_for_cool();
      break;
    case 1:
      thermo_fan_on();
      break;
    case 2:
      thermo_call_for_heat();
      break;
    case 3:
      thermo_fan_off();
      break;
    default:
    case 4:
      thermo_turn_off();
      break;
    }

    _delay_ms(500);


    command = command + 1;
    if(command == 5)
    {
	command = 0;
    }

  } 
    
}

//interrpt handler
ISR(USART_RX_vect)
{
	switch(interruptstate){
		case COMMAND_MODE:
			while (!(UCSR0A & (1<<RXC0)));
			c=UDR0; 
			//Read USART data register
			switch (c) {
				case HB_BETA:
					usart_out(ACTIVE_RESPONSE);  //no motion detected
					break;
				case READ_BETA:
					senddata();   //receive data request
					break;
				case SEND_BETA:
					interruptstate=1;
					usart_out(ACK);
				    break;
				default:
					break;
			}
			break;
		case TRANSMIT_MODE:
			while (!(UCSR0A & (1<<RXC0)));
			buffer[i]=UDR0;         //Read USART data register
			if(buffer[i++]=='\r')   //check for carriage return terminator and increment buffer index
			{
				// if terminator detected
				StrRxFlag=1;        //Set String received flag
				buffer[i-1]=0x00;   //Set string terminator to 0x00
				i=0;  
				interruptstate=COMMAND_MODE;              //Reset buffer index
			}
			break;	
	}
	
}
