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

void dcini()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_DeInit(TIM1);
	GPIO_DeInit(GPIOA);
	GPIO_AFIODeInit();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE);  //使能GPIO外设时钟使能
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	                                                                     	
   //设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_15; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 7200; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	



 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputState_Disable;
	
	
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
//	GPIO_SetBits(GPIOA, GPIO_Pin_2);
//	GPIO_SetBits(GPIOA, GPIO_Pin_3);
//	GPIO_SetBits(GPIOA, GPIO_Pin_4);
//	GPIO_SetBits(GPIOA, GPIO_Pin_5);


	
//	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH4预装载使能	
//	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH2预装载使能	
//	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH3预装载使能
//	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
//	
//	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM2

}

int main()
{
	RCC_Configuration();
	dcini();
	while(1)
	{
		TIM_SetCompare1(TIM1,32768);
		TIM_SetCompare2(TIM1,32768);
		TIM_SetCompare3(TIM1,32768);
	}
}
