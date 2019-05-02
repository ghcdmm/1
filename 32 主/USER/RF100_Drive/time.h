/****************************************Copyright (c)**************************************************
**                               Guangzou cells-net Development 
***                              Science and technology company
**                                 http://www.cells-net.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			delay.h
** Last modified Date:  2017-04-17
** Last Version:		V1.00
** Descriptions:		Time Config
**------------------------------------------------------------------------------------------------------
** Created by:			ShiHong Hu
** Created date:		2017-04-17
** Version:				  V1.00
** Descriptions:		Time Config
**
********************************************************************************************************/
#ifndef __TIME_H
#define __TIME_H

#ifdef   __TIME_MANL
#define  __EXT_TIME
#else 
#define  __EXT_TIME    extern
#endif

__EXT_TIME unsigned int tmr1s;          //  200ms计数
__EXT_TIME unsigned int flag1s;         //  200ms溢出标志位

void TIM3_INIT(unsigned int arr,unsigned int psc);
void systick_init(void);
void timing_delay_decrement(void);
void delay_us(volatile unsigned long n);

#endif

