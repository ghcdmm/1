#include "moter.h"
#include "stm32f10x.h"
#include<stdio.h>

#define kp 15
#define ki 0.3
#define kd 0.4

unsigned char i,a=10,b=10,delayx;
int dis=0;

struct PID {
	float e,pe,p2e;
	short enco,v,dv,vt;
	}m[4];

void dcpid()
{
	for(i=0;i<4;i++)
	{
		m[i].e=m[i].v-m[i].enco;
		m[i].dv=kp*((m[i].e-m[i].pe)+ki*m[i].e+kd*(m[i].e-2*m[i].pe+m[i].p2e));
		m[i].pe=m[i].e;
		m[i].p2e=m[i].pe;
		m[i].vt+=m[i].dv;
		if(m[i].vt>7200)
			m[i].vt=7200;
		if(m[i].vt<-7200)
			m[i].vt=-7200;
	}
}

void dcpid2m()
{
	printf("v:%d	%d	%d	%d\r\n",m[0].vt,m[1].vt,m[2].vt,m[3].vt);
	if(m[0].vt>=0)
	{
		TIM_SetCompare1(TIM1,m[0].vt);
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	}
	else
	{
		TIM_SetCompare1(TIM1,7200+m[0].vt);
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		
	}
	if(m[1].vt>=0)
	{
		TIM_SetCompare2(TIM1,m[1].vt*1.01);
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
	}
	else
	{
		TIM_SetCompare2(TIM1,7200+m[1].vt);
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	}
	if(m[2].vt>=0)
	{
		TIM_SetCompare3(TIM1,m[2].vt);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
	}
	else
	{
		TIM_SetCompare3(TIM1,7200+m[2].vt);
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	}
	if(m[3].vt>=0)
	{
		TIM_SetCompare4(TIM1,m[3].vt);//
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
	}
	else
	{
		TIM_SetCompare4(TIM1,7200+m[3].vt);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	}
}

void dcenco()
{
	m[0].enco=TIM2->CNT;
	m[1].enco=TIM3->CNT;
	m[2].enco=TIM4->CNT;
	m[3].enco=TIM5->CNT;
	printf("r:%d	%d	%d	%d\r\n",m[0].enco,m[1].enco,m[2].enco,m[3].enco);
	dis+=m[0].enco;
//	printf("dis:%d\r\n",dis);
	TIM2->CNT = 0;
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	TIM5->CNT = 0;  //2186/line 
}

void vsol(int vx,int vy,int w)
{
	m[0].v=vy-vx+w*(a+b);
	m[1].v=vy+vx-w*(a+b);
	m[2].v=-(vy-vx-w*(a+b));
	m[3].v=-(vy+vx+w*(a+b));
	printf("vvvvv:%d %d %d\r\n",vx,vy,w);
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
	
	TIM_TimeBaseStructure.TIM_Period = 65535; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

  TIM_ICStructInit(&TIM_ICInitStructure);//定时器作为编码器初始化
  TIM_ICInitStructure.TIM_ICFilter = 6;//ICx_FILTER; 滤波器6t
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ICInit(TIM5, &TIM_ICInitStructure);
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,  TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12,  TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12,  TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);		
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,  TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//编码器设置
		
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	
	TIM2->CNT = 0;
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	TIM5->CNT = 0;

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
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);


	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH4预装载使能	
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH2预装载使能	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH3预装载使能
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM2
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM5, ENABLE);

}

//TIM_SetCompare1(TIM4,__GmRight.sSpeed);

void steini()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	TIM_DeInit(TIM8);
//	GPIO_DeInit(GPIOC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 14400; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值 20ms
	TIM_TimeBaseStructure.TIM_Prescaler =99; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 400; //设置待装入捕获比较寄存器的脉冲值 400_-90deg 1760_90deg  1080--1.5ms 720--1ms 1440--2ms
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH4预装载使能
	
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	
	TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM8, ENABLE);  //使能TIM2
	
	
}

void stepini()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 65535; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

	NVIC_InitStructure.NVIC_IRQChannel =TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断使能
	NVIC_Init(&NVIC_InitStructure);
		
	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);
	
	TIM_ARRPreloadConfig(TIM6, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	
	//TIM_Cmd(TIM6, ENABLE);  //使能TIM6
}

void TIM6_IRQHandler(void)
{
	static unsigned char i;
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	}
	if(i)
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
	else
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	i=~i;
}

void SysTick_Handler()
{;
	delayx++;
	dcenco();
	dcpid();
	dcpid2m();
//printf("%d\r\n",TIM2->CNT);
	
}
