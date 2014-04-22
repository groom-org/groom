#ifndef DSH_DUMMY_H__
#define DSH_DUMMY_H__

#include <stdint.h>

/* 8 bit */
volatile uint8_t DDRB;
volatile uint8_t DDRC;
volatile uint8_t DDRD;

volatile uint8_t PORTB;
volatile uint8_t PORTC;
volatile uint8_t PORTD;

volatile uint8_t PINB;
volatile uint8_t PINC;
volatile uint8_t PIND;

volatile uint8_t TIFR0;
volatile uint8_t TIFR1;
volatile uint8_t TIFR2;

volatile uint8_t PCIFR;

volatile uint8_t EIFR;

volatile uint8_t EIMSK;

volatile uint8_t GPIOR0;

volatile uint8_t EECR;
volatile uint8_t EEDR;

volatile uint8_t EEARL;
volatile uint8_t EEARH;

volatile uint8_t GTCCR;

volatile uint8_t TCCR0A;
volatile uint8_t TCCR0B;

volatile uint8_t TCNT0;

volatile uint8_t OCR0A;
volatile uint8_t OCR0B;

volatile uint8_t GPIOR1;
volatile uint8_t GPIOR2;

volatile uint8_t SPCR;
volatile uint8_t SPSR;
volatile uint8_t SPDR;

volatile uint8_t ACSR;
volatile uint8_t SMCR;

volatile uint8_t MCUSR;
volatile uint8_t MCUCR;

volatile uint8_t SPMCSR;
volatile uint8_t WDTCSR;
volatile uint8_t CLKPR;
volatile uint8_t PRR;
volatile uint8_t OSCCAL;
volatile uint8_t PCICR;

volatile uint8_t EICRA;
volatile uint8_t PCMSK0;
volatile uint8_t PCMSK1;
volatile uint8_t PCMSK2;

volatile uint8_t TIMSK0;
volatile uint8_t TIMSK1;
volatile uint8_t TIMSK2;

volatile uint8_t ADCL;
volatile uint8_t ADCH;

volatile uint8_t ADCSRA;
volatile uint8_t ADCSRB;

volatile uint8_t ADMUX;

volatile uint8_t DIDR0;
volatile uint8_t DIDR1;

volatile uint8_t TCCR1A;
volatile uint8_t TCCR1B;
volatile uint8_t TCCR1C;

volatile uint8_t TCNT1L;
volatile uint8_t TCNT1H;

volatile uint8_t ICR1L;
volatile uint8_t ICR1H;

volatile uint8_t OCR1AL;
volatile uint8_t OCR1AH;

volatile uint8_t OCR1BL;
volatile uint8_t OCR1BH;

volatile uint8_t TCCR2A;
volatile uint8_t TCCR2B;

volatile uint8_t TCNT2;

volatile uint8_t OCR2A;
volatile uint8_t OCR2B;

volatile uint8_t ASSR;

volatile uint8_t TWBR;
volatile uint8_t TWSR;
volatile uint8_t TWAR;
volatile uint8_t TWDR;
volatile uint8_t TWCR;
volatile uint8_t TWAMR;

volatile uint8_t UCSR0A;
volatile uint8_t UCSR0B;
volatile uint8_t UCSR0C;

volatile uint8_t UBRR0L;
volatile uint8_t UBRR0H;

volatile uint8_t UDR0;

/* 16 bit */
volatile uint16_t EEAR;
volatile uint16_t ADC;
volatile uint16_t TCNT1;
volatile uint16_t ICR1;
volatile uint16_t OCR1A;
volatile uint16_t OCR1B;
volatile uint16_t UBRR0;

#endif /* DSH_DUMMY_H__ */
