#include "timer.h"
#include "led.h"
/**************************************************************************
函数功能：定时中断初始化
入口参数：arr：自动重装值  psc：时钟预分频数 
返回  值：无
**************************************************************************/
void Timer1_Init(u16 arr,u16 psc)  
{  
	RCC->APB2ENR|=1<<11;//TIM1时钟使能    
 	TIM1->ARR=arr;      //设定计数器自动重装值   
	TIM1->PSC=psc;      //预分频器7200,得到10Khz的计数时钟
	TIM1->DIER|=1<<0;   //允许更新中断				
	TIM1->DIER|=1<<6;   //允许触发中断	   
	TIM1->CR1|=0x01;    //使能定时器
	MY_NVIC_Init(1,3,TIM1_UP_IRQn,2);
}  
/**************************************************************************
函数功能：定时器2通道4输入捕获
入口参数：入口参数：arr：自动重装值  psc：时钟预分频数 
返回  值：无
**************************************************************************/
void TIM2_Cap_Init(u16 arr,u16 psc)	
{	 
	RCC->APB1ENR|=1<<0;   	//TIM2 时钟使能 
	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟   	 
	GPIOA->CRL&=0XFFFF00FF; 
	GPIOA->CRL|=0X00008200;//Pa.2 推挽输出   	Pa.3 输入 
	
  TIM2->ARR=arr;  		//设定计数器自动重装值   
	TIM2->PSC=psc;  		//预分频器 
	TIM2->CCMR2|=1<<8;		//CC1S=01 	选择输入端 IC1映射到TI1上
 	TIM2->CCMR2|=0<<12; 		//IC1F=0000 配置输入滤波器 不滤波
 	TIM2->CCMR2|=0<<10; 	//IC2PS=00 	配置输入分频,不分频 

	TIM2->CCER|=0<<13; 		//CC1P=0	上升沿捕获
	TIM2->CCER|=1<<12; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

	TIM2->DIER|=1<<4;   	//允许捕获中断				
	TIM2->DIER|=1<<0;   	//允许更新中断	
	TIM2->CR1|=0x01;    	//使能定时器2
	MY_NVIC_Init(1,3,TIM2_IRQn,1);
}

