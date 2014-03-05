#ifndef GROOM_USART_H__
#define GROOM_USART_H__

void usart_init();
void usart_out(char ch);
char usart_in(void);
void usart_outstring(char *s);

#endif /* GROOM_USART_H__ */
