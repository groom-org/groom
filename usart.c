#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "groom/usart.h"

static uint8_t ms_total = 0;

void usart_init()
{
	UBRR0 = F_UBRR;
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);
	UCSR0C = (3 << UCSZ00);

	#ifdef GROOM_MASTER
	UCSR0B |= (1 << RXCIE0);	
	#endif

	#ifdef GROOM_BETA
	UCSR0B |= (1 << RXCIE0);	
	sei();//enable interrupt
	#endif

	/* set the timer for the timeout call */
	TCCR0B |= (1 << CS02) | (1 << CS00);
}

void usart_out(char ch)
{
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = ch;
}

char usart_in(void)
{
	while ( !(UCSR0A & (1 << RXC0)) );
	return UDR0;
}

/*
 * Call usart_in with a timeout of 'timeout'ms. If the
 * communication succeeded it will set c to the value
 * received and return 0. If the timout triggered or some other
 * error occured it will return a nonzero value.
 */
uint8_t usart_in_timeout(char *c, uint16_t timeout)
{
	ms_total = 0;
	/* reset the timer */
	TCNT0 = 0;
	/* enable the interrupt */
	TIMSK0 |= (1 << TOIE0);
	sei();

	while ( !(UCSR0A & (1 << RXC0)) ) {
		if (ms_total >= timeout) {
			return 1;
		}
	}
	/* disable the interrupt */
	TIMSK0 &= ~(1 << TOIE0);
	*c = UDR0;

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	if (ms_total == 254) {
		ms_total = 255;
		return;
	}
	ms_total += 2;
}

void usart_outstring(char *s)
{
	while (*s != '\0') {
		usart_out(*s);
		s++;
	}
}

int usart_printf(char *format_string, ...)
{
	va_list varargs;
	char *string = NULL;
	int size;

	va_start(varargs, format_string);
	size = vsnprintf(string, 0, format_string, varargs);
	va_end(varargs);

	string = malloc(size + 1);
	if (!string) {
		return -1;
	}

	va_start(varargs, format_string);
	vsnprintf(string, size + 1, format_string, varargs);
	va_end(varargs);

	usart_outstring(string);
	free(string);

	return 0;
}
