#ifndef _USART_H_
#define _USART_H_

void USART0_init();
void USART1_init();
void USART0_transmit(char data);
void USART1_transmit(char data);
char USART0_receive();
char USART1_receive();
void USART0_print(const char *data);
void USART1_print(const char *data);

#endif
