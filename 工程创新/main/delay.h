/****************************************Copyright (c)**************************************************
**                               Guangzou cells-net Development 
***                              Science and technology company
**                                 http://www.cells-net.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			delay.h
** Last modified Date:  2017-04-17
** Last Version:		V1.00
** Descriptions:		Delay Config
**------------------------------------------------------------------------------------------------------
** Created by:			ShiHong Hu
** Created date:		2017-04-17
** Version:				  V1.00
** Descriptions:		Delay Config
**
********************************************************************************************************/
#ifndef __DELAY_H
#define __DELAY_H

#ifdef  _DELAY_MANL
#define _EXT_DELAY
#else 
#define _EXT_DELAY    extern 
#endif

_EXT_DELAY unsigned int    fac_us;       //  1ms延时，即1ms等于多少个滴答时钟
_EXT_DELAY unsigned int    fac_ms;       //  1ms延时，即1ms等于多少个滴答时钟

void delay_init(void);
//void delay_us(unsigned long nus);
void delay_ms(unsigned long nms);
void delay_10us(uint32_t cnt);

#endif
