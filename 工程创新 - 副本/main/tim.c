#include "stm32f10x.h"
#include "tim.h"

void delayini()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = 10000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =7100; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	

	NVIC_InitStructure.NVIC_IRQChannel =TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断使能
	NVIC_Init(&NVIC_InitStructure);
		
	TIM_ITConfig(TIM7,TIM_IT_Update, ENABLE);
	
	TIM_ARRPreloadConfig(TIM7, DISABLE); //使能TIMx在ARR上的预装载寄存器
	
	//TIM_Cmd(TIM6, ENABLE);  //使能TIM6
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
