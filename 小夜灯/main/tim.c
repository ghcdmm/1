#include "stm32f10x.h"
#include "tim.h"

unsigned int light=10;
unsigned char led=0;
unsigned char direct=0;

void TIMinit()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period = 100; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =7200; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	TIM2->CNT = 0;
	
	TIM_Cmd(TIM2, ENABLE); 
}

void TIM2_IRQHandler()
{
	static unsigned char i;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	if(light>4000)direct=1;
	else if(light<10)direct=0;
	if(direct==0)light++;
	else light--;
	
}
