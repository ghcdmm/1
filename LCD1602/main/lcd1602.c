#include "stm32f10x.h"
#include "lcd1602.h"
#include "delay.h"

enum rs1602 RS1602;
enum rw1602 RW1602;

void LCD1602_ins(unsigned char rs,unsigned char rw,unsigned char data)
{
	GPIOA->ODR&=0xfffb;
	GPIOA->ODR&=0xf800;
	GPIOA->ODR|=rs + (rw<<1) + (data<<3);
	GPIOA->ODR|=4;
	delay_us(2000);
	GPIOA->ODR&=0xfffb;
}

void LCD1602_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //∏¥”√Õ∆ÕÏ ‰≥ˆ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	delay_us(100000);
	LCD1602_ins(ins,wt,0x38);
	LCD1602_ins(ins,wt,0x0e);
	LCD1602_ins(ins,wt,0x06);
	LCD1602_ins(ins,wt,0x01);
}
