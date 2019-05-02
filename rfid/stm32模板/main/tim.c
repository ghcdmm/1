#include "stm32f10x.h"
#include "tim.h"

unsigned int tmr1s=0;          //  200ms计数
unsigned int flag1s;         //  200ms溢出标志位

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
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	  NVIC_InitStructure.NVIC_IRQChannel         = TIM3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
	
	  NVIC_Init(&NVIC_InitStructure);
//	  TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != DISABLE)
			{
//				printf("qqq");

		    if(++tmr1s >= 1000) {//1000ms
				    tmr1s  = 0;
					  flag1s = 1;
				}
		}
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
}

