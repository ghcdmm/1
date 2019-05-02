#include "stm32f10x.h"
#include "tim.h"

void delayini()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = 10000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =7100; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	

	NVIC_InitStructure.NVIC_IRQChannel =TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//�ж�ʹ��
	NVIC_Init(&NVIC_InitStructure);
		
	TIM_ITConfig(TIM7,TIM_IT_Update, ENABLE);
	
	TIM_ARRPreloadConfig(TIM7, DISABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	//TIM_Cmd(TIM6, ENABLE);  //ʹ��TIM6
}

void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	}
//	TIM_Cmd(TIM7, DISABLE);
	;//

	
	
}
