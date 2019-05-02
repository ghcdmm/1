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

void etrinit()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 65535; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
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
