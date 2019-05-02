#include "stm32f10x.h"
#include "usart.h"
#include "exti.h"
#include "tim.h"
#include "lcd1602.h"
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

int main()
{
	RCC_Configuration();
	LCD1602_init();
	LCD1602_ins(dat,wt,'A');
	while(1)
	{
		;
	}
}
