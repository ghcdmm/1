#include "stm32f10x.h"
#include "usart.h"
#include "exti.h"
#include "tim.h"
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
	unsigned char aaa=0;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_Configuration();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //GPIO_Mode_IPD
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	while(1)
	{
		aaa=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
		if(aaa)
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
		else
			GPIO_ResetBits(GPIOC,GPIO_Pin_13); 
	}
}
