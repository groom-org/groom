#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "groom/usart.h"

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
