#include "stm32f10x.h"
#include "usart.h"
#include "exti.h"
#include "tim.h"
#include "music.h"
#include<stdio.h>

void RCC_Configuration()
{
	ErrorStatus HSEStartUpStatus; 
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);                                            //�ⲿ����
	HSEStartUpStatus=RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus==SUCCESS) 
		{
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);             //Ԥȡֵ����ʹ��
			FLASH_SetLatency(FLASH_Latency_2);                                //������ʱʱֵ
			RCC_HCLKConfig(RCC_SYSCLK_Div1);                                  //����AHBʱ��=ϵͳʱ�� AHB��Ƶ�� �õ�HCLK
			RCC_PCLK2Config(RCC_HCLK_Div1);                                   //����APB=HCLK
			RCC_PCLK1Config(RCC_HCLK_Div2);                                   //APB1=HCLK/2
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);                              //PLLƵ�ʵ����ⲿ����Ƶ��,��Ƶϵ��*9   72MHz
			RCC_PLLCmd(ENABLE);                                               //�ȴ�PLL��ʼ���ɹ�
			while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET); 
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                         //ѡ��PLL��Ƶ���ʱ����Ϊϵͳʱ��;
			while(RCC_GetSYSCLKSource()!=0x08);                                //�ȴ�PLL��Ƶ���ʱ����Ϊϵͳʱ�ӳɹ�
			}
} 

int main()
{
	RCC_Configuration();
	music_ini();
	b=m2_b;
	t=m2_t;
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	while(1)
	{
		if(*t==0)
		{
			TIM_Cmd(TIM2, DISABLE);
			TIM_Cmd(TIM3, DISABLE);
		}
	}
}
