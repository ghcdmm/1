#include "stm32f10x.h"
#include "moter.h"
#include "usart.h"
#include "exti.h"
#include "tim.h"
#include<stdio.h>

unsigned char j=0;
unsigned char ord1[3]={0},color1[3]={0};

void RCC_Configuration()
{
    ErrorStatus HSEStartUpStatus; 
        RCC_DeInit();                                                         //重置时钟
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

void tb()
{
	vsol(0,0,5);
	delayx=0;
	dis=0;
	while(dis<5100);
	vsol(0,0,0);
	delayx=0;
	while(delayx<30);
	dis=0;
	vsol(0,100,0);
	while(dis<400);
	vsol(0,0,0);
	delayx=0;
	while(delayx<30);
}

void goback(int x)//2186
{
	vsol(0,-100,0); //x.2543/line y_2186 1.后退
	dis=0;
	while(dis>-x);
	vsol(0,0,0);
	delayx=0;
	while(delayx<30);
}

void goright(int x)
{
	vsol(-100,0,0);
	dis=0;
	while(dis<x);
	vsol(0,0,0);
	delayx=0;
	while(delayx<30);
}

void goleft(int x)
{
	vsol(100,0,0); //2.左移
	dis=0;
	while(dis>-x);//14500
	vsol(0,0,0); //3.停，二维码
}

void goahead(int x)
{
	vsol(0,100,0); 
	dis=0;
	while(dis<x);
	vsol(0,0,0);
	delayx=0;
	while(delayx<30);
}

void pickup()
{
	TIM_SetCompare3(TIM8,1280);
	TIM_SetCompare1(TIM8,900);
	TIM_SetCompare2(TIM8,1080);
	delayx=0;
	while(delayx<30);
	TIM_SetCompare3(TIM8,1900);
	delayx=0;
	while(delayx<20);
	TIM_SetCompare1(TIM8,1700);
	TIM_SetCompare2(TIM8,600);
}

void takeoff()
{
	TIM_SetCompare1(TIM8,1370);
	TIM_SetCompare2(TIM8,1500);
	TIM_SetCompare3(TIM8,1900);
	delayx=0;
	while(delayx<30);
	TIM_SetCompare3(TIM8,1080);
	delayx=0;
	while(delayx<20);
	TIM_SetCompare1(TIM8,1700);
	TIM_SetCompare2(TIM8,600);
}

void ctrl()
{
//	vsol(0,100,0);
//	while(1);
	//vsol(unsigned char vx,unsigned char vy,unsigned char w);
	delayx=0;
	while(delayx<20);
  goleft(2543);
	goahead(13000);
	while(start==0);//2--绿 3--蓝 1--红
	printf("color:%c %c %c order:%d %d %d\r\n",color[0],color[1],color[2],ord[0],ord[1],ord[2]);
//	for(j=0;j<3;j++)
//	{
//		switch(ord[j])
//		{
//			case 1:ord1[j]=1;break;
//			case 2:ord1[j]=2;break;
//			case 3:ord1[j]=3;break;
//		}
//	}
	for(j=0;j<3;j++)
	{
		switch(color[j])
		{
			case 'r':color1[0]=j;break;
			case 'g':color1[1]=j;break;
			case 'b':color1[2]=j;break;
		}
	}
	delayx=0;
	while(delayx<30);
	goback(5200);
	delayx=0;
	while(delayx<30);
	for(j=0;j<3;j++){
	if(color1[ord[j]]==2);
	else if(color1[ord[j]]==1)
	{
		goback(1500);
	}
	else if(color1[ord[j]]==3)
	{
		goahead(1500);
	}
	goright(800); //x.2543/line y_2186 1.后退
	pickup();
	delayx=0;while(delayx<40);//此处控制步进电机
	goleft(5000);
	if(color1[ord[j]]==2);
	else if(color1[ord[j]]==1)
	{
		goahead(2500);
	}
	else if(color1[ord[j]]==3)
	{
		goback(2500);
	}
	if(ord1[j]==1)
	{
		goback(1500);
	}
	else if(ord1[j]==2);
	else if(ord1[j]==3)
	{
		goahead(1500);
	}
////放置补偿2186
	takeoff();
	delayx=0;while(delayx<40);//第二次步进电机
	if(ord1[j]==1)
	{
		goahead(1500);
	}
	else if(ord1[j]==2);
	else if(ord1[j]==3)
	{
		goback(1500);
	}
	goright(5500);
}
//	for(j=0;j<3;j++)
//	{
//		switch(ord1[j])
//		{
//			case 1:
				
			while(1);
//	while(dis<3000);
//	delayx=0;
//	while(delayx<10);
	;
}

int main()
{
	unsigned char mode=0;
//	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_Configuration();
	SysTick_Config(3600000);//
	dcini();
	UART4_Init(115200);
	steini();
	

//	USART1_INIT(115200);
//	TIM4_PWM_Init(7200,3600);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH1
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
////	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	TIM_SetCompare1(TIM1,3600);
//	TIM_SetCompare2(TIM1,3600);
//	TIM_SetCompare3(TIM1,3600);
//	TIM_SetCompare4(TIM1,3600);
	printf("aaa");
	while(1)
	{ctrl();}//printf("aaa");
}
