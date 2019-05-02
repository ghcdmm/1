#include "stm32f10x.h"
#include "usart.h"
#include "exti.h"
#include "tim.h"
#include<stdio.h>

void RCC_Configuration()
{
	ErrorStatus HSEStartUpStatus; 
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);                                            //外部晶振开
	HSEStartUpStatus=RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus==SUCCESS) 
		{
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);             //预取值缓存使能
			FLASH_SetLatency(FLASH_Latency_2);                                //代码延时时值
			RCC_HCLKConfig(RCC_SYSCLK_Div1);                                  //设置AHB时钟=系统时钟 AHB分频器 得到HCLK
			RCC_PCLK2Config(RCC_HCLK_Div1);                                   //设置APB=HCLK
			RCC_PCLK1Config(RCC_HCLK_Div2);                                   //APB1=HCLK/2
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);                              //PLL频率等于外部晶振频率,倍频系数*9   72MHz
			RCC_PLLCmd(ENABLE);                                               //等待PLL初始化成功
			while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET); 
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                         //选择PLL倍频后的时钟作为系统时钟;
			while(RCC_GetSYSCLKSource()!=0x08);                                //等待PLL倍频后的时钟作为系统时钟成功
			}
} 

void etrinit()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 65535; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF ,TIM_ExtTRGPolarity_NonInverted, 0);
	TIM_ITRxExternalClockConfig(TIM2, TIM_TS_ETRF);
	TIM2->CNT=0;//TIM_SetCounter(TIM2,0);
	
	TIM_Cmd(TIM2,ENABLE);
}

int main()
{
	int a=0;
	RCC_Configuration();
	etrinit();
	while(1)
	{
		a=TIM2->CNT;
		TIM2->CNT=0;
		;
	}
}
