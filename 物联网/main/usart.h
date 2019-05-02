#ifndef __USART_H
#define __USART_H

void UART4_Init(unsigned long baud);
extern unsigned char USART3_RX_BUF[3];
extern unsigned char start;
extern unsigned char color[3];
extern unsigned char ord[3];
void USART1_Init(unsigned long baud);
void USART2_Init(unsigned long baud);
extern unsigned char num;
struct frame{
	unsigned char byteLen;
	unsigned char dat[127];
	signed char rssi;
};
extern struct frame RaFRAME[100];
void Usart_buff_copy1(unsigned char *usart_buff,unsigned char data_len);
void USART_RXD_Data_Process(void);
unsigned char Putchar(unsigned char ch);
void PutString(unsigned char *dat, unsigned char len);
void UART4_Init(unsigned long baud);




#endif
