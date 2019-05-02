#ifndef __USART_H
#define __USART_H

void UART4_Init(unsigned long baud);
extern unsigned char USART3_RX_BUF[3];
void USART1_Init(unsigned long baud);
void USART2_Init(unsigned long baud);
void USART_RXD_Data_Process(void);
unsigned char Putchar(unsigned char ch);
void PutString(unsigned char *dat, unsigned char len);
void UART4_Init(unsigned long baud);




#endif
