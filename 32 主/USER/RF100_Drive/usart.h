/****************************************Copyright (c)**************************************************
**                           Guangzou cells-net Development Co.,LTD
**                                 http://www.cells-net.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			usart.h
** Last modified Date:  2017-04-15
** Last Version:		V1.00
** Descriptions:		usart1 Config
**------------------------------------------------------------------------------------------------------
** Created by:			ShiHong Hu
** Created date:		2017-04-15
** Version:				  V1.00
** Descriptions:		usart1 Config
**
********************************************************************************************************/
#ifndef __USART_H
#define __USART_H

#ifdef   __USART_MANL
#define  __EXT_USART
#else  
#define  __EXT_USART  extern 
#endif

#define SET      1
#define RESET    0
#define USART_REC_LEN    200          //  ����ͨ�Ž������ݵ�����������С
#define EN_USART1_RX 		 1             //  ʹ�ܣ�1��/��ֹ��0�����ڽ���

__EXT_USART unsigned char    USART_RXD_Buf[5][USART_REC_LEN];      //  ����ͨ�Ž��ձ�������Ĵ���
__EXT_USART unsigned int     USART_RXD_Cnt[5];                     //  ����ͨ�Ž��ռ�����
                              //  ����ͨ�Ž������ݸ�������
extern unsigned char num;

 void USART1_INIT(unsigned long baud);
 unsigned char Putchar(unsigned char ch);
 void PutString(unsigned char *dat, unsigned char len);
 void USART_RXD_Data_Process(void);
 unsigned long BoundRemanCode(unsigned char BaudCode);
 void USART2_Initialise(unsigned long bound);
void USART3_Init(unsigned long baud);

#endif
