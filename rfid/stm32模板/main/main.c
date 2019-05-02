#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "tim.h"
#include<stdio.h>
#include "RF100_DRV.h"

#define ang 30

unsigned char j=0,j1=0,j2=0;
unsigned char ord1[3]={0},color1[3]={0};
signed char rad=0;

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

void moterini()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//PE2~4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOE2,3,4,11     // WK_UP-->GPIOA.0
}

void moter()
{
	static unsigned char i=0;
//			delay_us(360000);
	if(i==0)
	{
		GPIOA->ODR&=0xff7f;
		GPIOA->ODR|=0x0010;
	}

//		delay_us(180000);
	else if(i==1)
	{
		GPIOA->ODR&=0xffef;
		GPIOA->ODR|=0x0040;
	}
//		delay_us(180000);
	else if(i==2)
	{
		GPIOA->ODR&=0xffbf;
		GPIOA->ODR|=0x0020;
	}
//	delay_us(180000);
	else if(i==3)
	{
		GPIOA->ODR&=0xffdf;
		GPIOA->ODR|=0x0080;
	}
	i++;
	rad++;
	if(i>3)i=0;
}

void dmoter()
{
	static unsigned char i=0;
//			delay_us(360000);
	if(i==0)
	{
		GPIOA->ODR&=0xff7f;//GPIOA->ODR|=0x0010;
	}
	else if(i==1)
	{
		GPIOA->ODR|=0x0040;//GPIOA->ODR&=0xff7f;
	}
	else if(i==2)
	{
		GPIOA->ODR&=0xffef;//GPIOA->ODR|=0x0020;
	}
	else if(i==3)
	{
		GPIOA->ODR|=0x0020;//GPIOA->ODR&=0xffef;
	}
	else if(i==4)
	{
		GPIOA->ODR&=0xffbf;//GPIOA->ODR|=0x0040;
	}
	else if(i==5)
	{
		GPIOA->ODR|=0x0080;//GPIOA->ODR&=0xffdf;
	}
	else if(i==6)
	{
		GPIOA->ODR&=0xffdf;//GPIOA->ODR|=0x0080;
	}
	else if(i==7)
	{
		GPIOA->ODR|=0x0010;//GPIOA->ODR&=0xffbf;
	}
	i++;
	rad++;
	if(i>7)i=0;
}

void moterb()
{
	while(rad>0)
	{
		delay_us(180000);

		GPIOA->ODR&=0xffdf;
		GPIOA->ODR|=0x0080;

		delay_us(180000);

		GPIOA->ODR&=0xffbf;
		GPIOA->ODR|=0x0020;

		delay_us(180000);


		GPIOA->ODR&=0xffef;
		GPIOA->ODR|=0x0040;

		delay_us(180000);

		GPIOA->ODR&=0xff7f;
		GPIOA->ODR|=0x0010;
		
		rad-=4;
	}
}

void dmoterb()
{
	while(rad>0)
	{
		delay_us(180000);
		GPIOA->ODR|=0x0010;//GPIOA->ODR&=0xffbf;
		delay_us(180000);
		GPIOA->ODR&=0xffdf;//GPIOA->ODR|=0x0080;
		delay_us(180000);
		GPIOA->ODR|=0x0080;//GPIOA->ODR&=0xffdf;
		delay_us(180000);
		GPIOA->ODR&=0xffbf;//GPIOA->ODR|=0x0040;
		delay_us(180000);
		GPIOA->ODR|=0x0020;//GPIOA->ODR&=0xffef;
		delay_us(180000);
		GPIOA->ODR&=0xffef;//GPIOA->ODR|=0x0020;
		delay_us(180000);
		GPIOA->ODR|=0x0040;//GPIOA->ODR&=0xff7f;
		delay_us(180000);
		GPIOA->ODR&=0xff7f;//GPIOA->ODR|=0x0010;
		
		rad-=8;
	}
	st=0;
}

void EXTI_PB9_Init(void)

{

  GPIO_InitTypeDef GPIO_InitStructure;

  NVIC_InitTypeDef NVIC_InitStructure;

  EXTI_InitTypeDef EXTI_InitStructure;

  //1.??GPIO?AFIO??,??????,??????????????AFIO???

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);

  //2.GPIO???

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //????

  GPIO_Init(GPIOB,&GPIO_InitStructure);  

  //3.??EXTI?

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);  //?EXIT?9???PB9

  EXTI_InitStructure.EXTI_Line = EXTI_Line9;

  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //???????

  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//?????

  EXTI_Init(&EXTI_InitStructure);//?????

  //4.????

  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;

  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

}

int main()
{
	unsigned char wb=0;
//	unsigned char mode=0;
	//TIM3->CR1
	unsigned char i,ove=0;
	int srssi[30]={0};


//	char k=0;
	float pos[30][125]={0};
////	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_Configuration();
	USART1_Init(115200);
	TIM3_INIT(65535,0);//1ms
////	SysTick_Config(3600000);
////	UART4_Init(115200);
	
	USART2_Init(115200);
	EXTI_PB9_Init();
	

	
//	TIM4_PWM_Init(7200,3600);
	printf("aaa\n");
	moterini();
	delay_us(1000000);
	SendCmdFrame(MultiInventCMD);
////	SendCmdFrame(StopMultiInventCMD);
////	SendCmdFrame(PowerCMD);
//	delay_us(1000000);
//	SendCmdFrame(SetSelectModeCMD);
	GPIOA->ODR|=0x0090;
	TIM3->CNT=0;
	while(1)
	{
		
//		printf("a");
//		USART_SendData(USART2,'a');
//		while(st==0);
//		if(st==0)
//		{
//			dmoter();
//			st=1;
//		}
		if(num[0]>(ang-1))
		{
			for(j2=0;num[j2]!=0;j2++)
			{
				if(num[j2]<ang)
				{
					ove=0;
					break;
				}
				else
				{
					ove=1;
				}
			}
		}
		if(ove==1)
		{
//			
			SendCmdFrame(StopMultiInventCMD);
//			for(j=0;num[j]!=0;j++)
//				num[j]=0;
			for(j1=0;j1<j2;j1++){
			for(j=0;j<ang;j++)
			{
//				printf("%d:%d\r\n",j,RaFRAME[j].rssi);
//				if(RaFRAME[j].rssi==0)
//				{
//					for(k=0;k<15;k++)
//					{
//						printf("%c",RaFRAME[j].dat[k]);
//					}
//				}
				srssi[j1]+=RaFRAME[j1][j].rssi;
				RaFRAME[j1][j].rssi=0;
			}
			pos[j1][rad]=srssi[j1]/ang;//auto
			printf("ave%d=%f\r\n",j1,(float)srssi[j1]/ang);
			srssi[j1]=0;
			num[j1]=0;
		}
			
//			if(rad>=20)
//			{
//				moterb();
//				for(i=0;i<21;i++)
//				{
//					printf("%d	%f\r\n",i+1,pos[i]);
//				}
//				while(1);
//			}
//			moter();
//			num=0;
			rxclear();
		dmoter();
			SendCmdFrame(MultiInventCMD);
		ove=0;

		}
		USART_RXD_Data_Process();
//		RF100_FRAME_RX_HANDLE();	TIM_Cmd(TIM3,ENABLE);
		if(rad>120)
			dmoterb();
//			TIM_Cmd(TIM3,DISABLE);printf("ov=%d	arr=%d\r\n",tmr1s,TIM3->CNT);
//			tmr1s=0;TIM3->CNT=0;
			
	}
}

void EXTI9_5_IRQHandler(void)

{

  if(EXTI_GetITStatus(EXTI_Line9) != RESET)  //EXTI9?????

  {

    while(1);

  }

  EXTI_ClearITPendingBit(EXTI_Line9);//????

}
