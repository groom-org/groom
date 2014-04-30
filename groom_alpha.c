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

volatile int i=0;
volatile uint8_t buffer[20];
volatile uint8_t StrRxFlag=0;
volatile char c='0';
char buf[256]; //string for thermometer
volatile uint8_t interruptstate=0; //interrupt state

// Find divisors for the UART0 and I2C baud rates

#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz
// device ID and address
#define DEV_TYPE   0x90     
#define DEV_ADDR   0x00 
#define SLAVE_ID   DEV_TYPE | DEV_ADDR

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

// DS1621 Registers & Commands

uint8_t RD_TEMP   = 0xAA;                         // read temperature register
uint8_t ACCESS_TH = 0xA1;                         // access high temperature register
uint8_t ACCESS_TL  =0xA2;                         // access low temperature register
uint8_t ACCESS_CFG =0xAC;                         // access configuration register
uint8_t RD_CNTR    =0xA8;                         // read counter register
uint8_t RD_SLOPE   =0xA9;                         // read slope register
uint8_t START_CNV  =0xEE;                         // start temperature conversion
uint8_t STOP_CNV   =0X22;                         // stop temperature conversion

// DS1621 configuration bits

uint8_t DONE       =0x80;                    // conversion is done
uint8_t THF        =0x40;                    // high temp flag
uint8_t TLF        =0x20;                    // low temp flag
uint8_t NVB        =0x10;                    // non-volatile memory is busy
uint8_t POL        =0x02;                    // output polarity (1 = high, 0 = low)
uint8_t ONE_SHOT   =0x01;                    // 1 = one conversion; 0 = continuous conversion
uint8_t motion=0;

//thermometer functions

void i2c_init(uint8_t);
uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
               uint8_t *, uint16_t, uint8_t *, uint16_t);
int getHrTemp();
int getTemp(uint8_t);
void startConversion(int);
uint8_t getReg(uint8_t);
void setConfig(uint8_t);
void setup();


void control(char *command){
	//do whatever you want
}

// data sender temp
void gettempdata(){
	
	int i;
	_delay_ms(500);
	buf[0] = '\0';
	
	for(i=0;i<2;i++){
		
		int tC, tFrac;
		tC = getHrTemp();                             // read high-resolution temperature
		
		if (tC < 0) {
			tC = -tC;                                   // fix for integer division
			sprintf(buf, "-");
			usart_outstring(buf);	
		}
		
		tFrac = tC % 100;                             // extract fractional part
		tC /= 100;                                    // extract whole part
		
		if (tFrac < 10){
			tFrac=0;
			
		}
		
		sprintf(buf, "%d.%d\r",tC,tFrac);
		_delay_ms(100);
		
	}
	usart_outstring(buf);
    c=DEFAULT;
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
			}else{
				sprintf(buffer,"TIME_OUT");
				count=0;
				i=0;  
				interruptstate=COMMAND_MODE;              //Reset buffer index
			}
			
		}
	}
	
	//set relays
	control(buffer);
	
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
	setup();
	buf[0] = '\0';
	
	while(1){
		if (PINC & (1 << PC1))
			motion=1;
		else {
			motion=0;
		}
		
		switch(interruptstate){
			case TRANSMIT_MODE:
				receivecommand();
				break;
			default:
				break;
		}
		
	};
	
}

//I2C things, don't touch!

/* ----------------------------------------------------------------------- */

/*
 i2c_io - write and read bytes to a slave I2C device
 
 This funtions write "an" bytes from array "ap" and then "wn" bytes from array
 "wp" to I2C device at address "device_addr".  It then reads "rn" bytes from
 the same device to array "rp".
 
 Return values (might not be a complete list):
 0    - Success
 0x20 - NAK received after sending device address for writing
 0x30 - NAK received after sending data
 0x38 - Arbitration lost with address or data
 0x48 - NAK received after sending device address for reading
 
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
 This "i2c_io" I2C routine is an attempt to provide an I/O function for both
 reading and writing, rather than have separate functions.
 
 I2C writes consist of sending a stream of bytes to the slave device.  In some
 cases the first few bytes may be the internal address in the device, and then
 the data to be stored follows.  For example, EEPROMs like the 24LC256 require a
 two-byte address to precede the data.  The DS1307 RTC requires a one-byte
 address.
 
 I2C reads often consist of first writing one or two bytes of internal address
 data to the device and then reading back a stream of bytes starting from that
 address.  Some devices appear to claim that that reads can be done without
 first doing the address writes, but so far I haven't been able to get any to
 work that way.
 
 This function does writing and reading by using pointers to three arrays "ap",
 "wp", and "rp".  The function performs the following actions in this order:
 If "an" is greater than zero, then "an" bytes are written from array "ap"
 If "wn" is greater then zero, then "wn" bytes are written from array "wp"
 If "rn" is greater then zero, then "rn" byte are read into array "rp"
 Any of the "an", "wn", or "rn" can be zero.
 
 The reason for separate "ap" and "wp" arrays is that the address data can be
 taken from one array, and then the write data from another without requiring
 that the contents be merged into one array before calling the function.  This
 means the following three calls all do exactly the same thing.
 
 i2c_io(0xA0, buf, 100, NULL, 0, NULL, 0);
 i2c_io(0xA0, NULL, 0, buf, 100, NULL, 0);
 12c_io(0xA0, buf, 2, buf+2, 98, NULL, 0);
 
 In all cases 100 bytes from array "buf" will be written to the I2C device at
 bus address 0xA0.
 
 A typical write with a 2-byte address is done with
 
 i2c_io(0xA0, abuf, 2, wbuf, 50, NULL, 0);
 
 A typical read with a 1-byte address is done with
 
 i2c_io(0xD0, abuf, 1, NULL, 0, rbuf, 20);
 */

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn)
{
    uint8_t status, send_stop, wrote, start_stat;
	
    status = 0;
    wrote = 0;
    send_stop = 0;
	
	
	
    if (an > 0 || wn > 0) {
        wrote = 1;
        send_stop = 1;
        
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);  // Send start condition
		
		while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
		
		status = TWSR & 0xf8;
        if (status != 0x08)                 // Check that START was sent OK
            return(status);
		
		
		
        TWDR = device_addr & 0xfe;          // Load device address and R/W = 0;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x18) {               // Check that SLA+W was sent OK
            if (status == 0x20)             // Check for NAK
                goto nakstop;               // Send STOP condition
            return(status);                 // Otherwise just return the status
        }
		
		
        // Write "an" data bytes to the slave device
        while (an-- > 0) {
            TWDR = *ap++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }
		
        // Write "wn" data bytes to the slave device
        while (wn-- > 0) {
            TWDR = *wp++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }
		
		
        status = 0;                         // Set status value to successful
    }
	
    if (rn > 0) {
        send_stop = 1;
		
        // Set the status value to check for depending on whether this is a
        // START or repeated START
        start_stat = (wrote) ? 0x10 : 0x08;
		
        // Put TWI into Master Receive mode by sending a START, which could
        // be a repeated START condition if we just finished writing.
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
		// Send start (or repeated start) condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != start_stat)           // Check that START or repeated START sent OK
            return(status);
		
		
        TWDR = device_addr  | 0x01;         // Load device address and R/W = 1;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Send address+r
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x40) {               // Check that SLA+R was sent OK
            if (status == 0x48)             // Check for NAK
                goto nakstop;
            return(status);
        }
		
        // Read all but the last of n bytes from the slave device in this loop
        rn--;
        while (rn-- > 0) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read byte and send ACK
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x50)             // Check that data received OK
                return(status);
            *rp++ = TWDR;                   // Read the data
        }
		
        // Read the last byte
        TWCR = (1 << TWINT) | (1 << TWEN);  // Read last byte with NOT ACK sent
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x58)                 // Check that data received OK
            return(status);
        *rp++ = TWDR;                       // Read the data
		
        status = 0;                         // Set status value to successful
    }
    
nakstop:                                    // Come here to send STOP after a NAK
    if (send_stop)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Send STOP condition
	
    return(status);
}


/*
 i2c_init - Initialize the I2C port
 */
void i2c_init(uint8_t bdiv)
{
    TWSR = 0;                           // Set prescalar for 1
    TWBR = bdiv;                        // Set bit rate register
}

//thermometer set up 

// SDA pin is Analog4
// SCL pin is Analog5
// DS1621 has A2, A1, and A0 pins connected to GND

void setup()
{
	//PORTC |= (1 << PC1);        // Enable pull-up for switch on PORTC bit 1
    i2c_init(BDIV);             // Initialize the I2C port
	startConversion(0);                       // stop if presently set to continuous
	setConfig(POL);                    // Tout = active high; 1-shot mode
}


// Set configuration register
void setConfig(uint8_t cfg)
{
	i2c_io(SLAVE_ID, &ACCESS_CFG, 1, &cfg, 1, NULL, 0);
	// allow EE write time to finish
}


// Read a DS1621 register
uint8_t getReg(uint8_t reg)
{
	uint8_t regVal=0;
	i2c_io(SLAVE_ID, &reg, 1, NULL, 0, &regVal, 1);
	return regVal;
}

// Sets temperature threshold
// -- whole degrees C only
// -- works only with ACCESS_TL and ACCESS_TH

// Start/Stop DS1621 temperature conversion
void startConversion(int start)
{
	
	
	if (start == 1){
		
		i2c_io(SLAVE_ID, &START_CNV, 1, NULL, 0, NULL, 0);
		
	}
	else{
		
		i2c_io(SLAVE_ID, &STOP_CNV, 1, NULL, 0, NULL, 0);
		
	}
}


// Reads temperature or threshold
// -- whole degrees C only
// -- works only with RD_TEMP, ACCESS_TL, and ACCESS_TH
int getTemp(uint8_t reg)
{
	int tC;
	
	if (reg == RD_TEMP || reg == ACCESS_TL || reg == ACCESS_TH) {
		uint8_t tVal = getReg(reg);
		if (tVal >= 0xb10000000) {                    // negative?
			tC = 0xFF00 | tVal;                       // extend sign bits
		}
		else {
			tC = tVal;
		}
		return tC;                                  // return threshold
	}
	return 0;                                     // bad reg, return 0
}


// Read high resolution temperature
// -- returns temperature in 1/100ths degrees
// -- DS1620 must be in 1-shot mode
int getHrTemp()
{
	
    
	startConversion(1);                        // initiate conversion
	
	uint8_t cfg = 0;
	_delay_ms(10);
	//while ((cfg & 0x80) == 0) {                          // let it finish
	//	cfg = getReg(ACCESS_CFG);
	//}
	
	
	int tHR = getTemp(RD_TEMP);  // get whole degrees reading
	
	
	uint8_t cRem = getReg(RD_CNTR);                  // get counts remaining
	uint8_t slope = getReg(RD_SLOPE);                // get counts per degree
	
	if (tHR >= 0)
		tHR = (tHR * 100 - 25) + ((slope - cRem) * 100 / slope);
	else {
		tHR = -tHR;
		tHR = (25 - tHR * 100) + ((slope - cRem) * 100 / slope);
	}
	return tHR;
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
				case HB_ALPHA:
					if (motion==1) {
						usart_out(ACTIVE_RESPONSE_MOTION);  //motion detected
					}else{
						usart_out(ACTIVE_RESPONSE);  //no motion detected
					}
					break;
				case READ_ALPHA:
					gettempdata();   //receive data request
					break;
				case SEND_ALPHA:
					interruptstate=TRANSMIT_MODE;
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