/****************************************Copyright (c)**************************************************
**                               Guangzou cells-net Development 
***                              Science and technology company
**                                 http://www.cells-net.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			time.c
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
#define __TIME_MANL
#include "stm32f10x.h"
#include "time.h"

/**********************************************************************************************************
TIM3_INIT:��ʱ��3��ʼ������
���ܣ���ʱ��3����
	    Tout=((arr+1)*(psc+1))/Tclk
	    Tclk:TIM3������ʱ��Ƶ�ʣ���λ��MHz��
	    Tout:TIM�����ʱ�䣨��λ��us��
������psc:Ԥ��Ƶϵ��(prescaler)
	    arr:�Զ���װ�Ĵ���ֵ��Auto relode register value��
**********************************************************************************************************/
void TIM3_INIT(unsigned int arr,unsigned int psc)
{
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	  
	  TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
	  TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
	  TIM_TimeBaseInitStructure.TIM_Period            = arr;
	  TIM_TimeBaseInitStructure.TIM_Prescaler         = psc;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	  NVIC_InitStructure.NVIC_IRQChannel         = TIM3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	
	  NVIC_Init(&NVIC_InitStructure);
	  TIM_Cmd(TIM3,ENABLE);
}

/**********************************************************************************************************
TIM3_IRQHandler����ʱ��3�ж�
���ܣ�
������
**********************************************************************************************************/
void TIM3_IRQHandler(void)
{

    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != DISABLE) {

		    if(++tmr1s >= 1000) {//1000ms
				    tmr1s  = 0;
					  flag1s = 1;
				}
		}
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
}


volatile unsigned long gTimingDelay;

/* SystemCoreClock / 1000    --> 1ms */
/* SystemCoreClock / 10000   --> 100us */
/* SystemCoreClock / 100000  --> 10us */
/* SystemCoreClock / 1000000 --> 1us */

void systick_init(void)
{
    while (SysTick_Config(SystemCoreClock / 1000000) == 1);
}

void timing_delay_decrement(void)
{
    if (gTimingDelay != 0x0)
    {
        gTimingDelay--;
    }
}

void SysTick_Handler(void)
{
    timing_delay_decrement();
}

void delay_us(volatile unsigned long n)
{
    gTimingDelay = n;
    while(gTimingDelay != 0)  gTimingDelay--;
}
