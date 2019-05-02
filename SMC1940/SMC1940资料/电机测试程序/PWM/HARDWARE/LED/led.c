#include "led.h"


void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(GPIOA, &GPIO_InitStructure);	
 GPIO_SetBits(GPIOA,GPIO_Pin_0);	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 
 GPIO_ResetBits(GPIOB,GPIO_Pin_5 );						   //PB.5 输出低
 GPIO_SetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8);					   //PB.8输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				
 GPIO_ResetBits(GPIOC,GPIO_Pin_12); 						 //PC.12 输出低
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						  //PD.2 输出高
}
 
