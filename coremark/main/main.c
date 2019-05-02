#include "stm32f10x.h"
#include "moter.h"
#include "usart.h"
#include "exti.h"
#include "tim.h"
#include<stdio.h>

unsigned long a=1234;
unsigned long b=5678;
unsigned long i,c;
unsigned int x;

//unsigned char j=0;
//unsigned char ord1[3]={0},color1[3]={0};

void RCC_Configuration()
{
    ErrorStatus HSEStartUpStatus; 
        RCC_DeInit();                                                         //����ʱ��
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
/*
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
	vsol(0,-100,0); //x.2543/line y_2186 1.����
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
	vsol(100,0,0); //2.����
	dis=0;
	while(dis>-x);//14500
	vsol(0,0,0); //3.ͣ����ά��
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

void ctrl()
{
//	vsol(0,100,0);
//	while(1);
	//vsol(unsigned char vx,unsigned char vy,unsigned char w);
	delayx=0;
	while(delayx<20);
  goleft(2543);
	goahead(13000);
	while(start==0);//2--�� 3--�� 1--��
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
	goright(1000); //x.2543/line y_2186 1.����
	delayx=0;while(delayx<40);//�˴����Ʋ������
	goleft(5000);
	if(color1[ord[j]]==2);
	else if(color1[ord[j]]==1)
	{
		goahead(1500);
	}
	else if(color1[ord[j]]==3)
	{
		goback(1500);
	}
	if(ord1[j]==1)
	{
		goback(2100);
	}
	else if(ord1[j]==2);
	else if(ord1[j]==3)
	{
		goahead(2100);
	}
////���ò���2186
	delayx=0;while(delayx<40);//�ڶ��β������
	if(ord1[j]==1)
	{
		goahead(2100);
	}
	else if(ord1[j]==2);
	else if(ord1[j]==3)
	{
		goback(2100);
	}
	goright(5000);
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
}*/

int main()
{
	unsigned char mode=0;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_Configuration();
	SysTick_Config(3600000);
//	dcini();
//	UART4_Init(115200);
//	delayini();
//	TIM_Cmd(TIM2, ENABLE);
//	//steini();
//	TIM7->CNT=0;

//	USART1_INIT(115200);
//	TIM4_PWM_Init(7200,3600);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	TIM_SetCompare1(TIM1,3600);
//	TIM_SetCompare2(TIM1,3600);
//	TIM_SetCompare3(TIM1,3600);
//	TIM_SetCompare4(TIM1,3600);


	while(1)
	{
		unsigned char j=0;
		for(i=0;i<1000000;i++)
		{
			for(j=0;j<100;j++)
			{
		
				c=a+b;
			}
		}
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		printf("%d\r\n",x);
		for(i=0;i<1000000;i++)
		{
			for(j=0;j<100;j++)
			{
		
				c=a+b;
			}
		}
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		
	}//printf("aaa");
}