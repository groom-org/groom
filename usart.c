#include <avr/io.h>

#include "groom/usart.h"

void usart_init()
{
	UBRR0 = F_UBRR;
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);
	UCSR0C = (3 << UCSZ00);
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

void usart_outstring(char *s) {
	while (*s != '\0') {
		usart_out(*s);
		s++;
	}
}
