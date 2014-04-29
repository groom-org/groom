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



#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz
// device ID and address
volatile int i=0;
volatile uint8_t buffer[20];
volatile uint8_t StrRxFlag=0;
volatile char c='0';
char buf[256]; //string for thermometer
volatile uint8_t interruptstate=0; //interrupt state
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

// data sender general
void senddata(){
	
	uint16_t result = read_adc(1);	
	char buf[128];
	sprintf(buf, "sample: %d\r", result);
	usart_outstring(buf);
	//usart_outstring("HANXIAO\r");
	c='0';
	
}

void receivecommand(){
	int count=0;
	
	while (1) {
		_delay_ms(10);
		count++;
		if(StrRxFlag || count>100){    //time_out
			if(StrRxFlag){
            	StrRxFlag=0;                // Reset String received flag
				count=0;
				usart_outstring(buffer);
				return buffer;
			}else{
				sprintf(buffer,"TIME_OUT");
				count=0;
				i=0;  
				interruptstate=0;              //Reset buffer index
				usart_outstring(buffer);
				return buffer;
			}
			
		}
	}
	
	//do all the control here base on the command
	
} 

void usart_init(unsigned short ubrr)
{
	UBRR0 = ubrr;
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << RXCIE0); 
	UCSR0C = (3 << UCSZ00);
	sei();//enable interrupt
}

void usart_out(char ch)
{
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = ch;
}

char usart_in()
{
	while ( !(UCSR0A & (1 << RXC0)) );
	return UDR0;
}

void usart_outstring(char *s) {
	while (*s != '\0') {
		usart_out(*s);
		s++;
	}
}

int main(void)
{
	/* for 9600 baud on with 9.304MHz clock */
	usart_init(63);
	adc_init();
	
	while(1){

		switch(interruptstate){
			case 1:
				receivecommand();
				break;
			default:
				break;
		}
	}
	
}

//interrpt handler
ISR(USART_RX_vect)
{
	switch(interruptstate){
		case 0:
			while (!(UCSR0A & (1<<RXC0)));
			c=UDR0; 
			//Read USART data register
			switch (c) {
				case '2':
					if (motion==1) {
						usart_out('r');  //motion detected
					}else{
						usart_out('R');  //no motion detected
					}
					break;
				case '4':
					senddata();   //receive data request
					break;
				case '6':
					interruptstate=1;
					usart_out('A');
				    break;
				default:
					break;
			}
			break;
		case 1:
			while (!(UCSR0A & (1<<RXC0)));
			buffer[i]=UDR0;         //Read USART data register
			if(buffer[i++]=='\r')   //check for carriage return terminator and increment buffer index
			{
				// if terminator detected
				StrRxFlag=1;        //Set String received flag
				buffer[i-1]=0x00;   //Set string terminator to 0x00
				i=0;  
				interruptstate=0;              //Reset buffer index
			}
			break;	
	}
	
}