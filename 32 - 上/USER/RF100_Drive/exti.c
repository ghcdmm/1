#include "exti.h"
#include "stm32f10x.h"
#include "time.h"
#include "usart.h"
#include "usart.h"
#include<stdio.h>

#define DHT11_GPIO_TYPE  GPIOB
#define DHT11_GPIO_PIN   GPIO_Pin_11
#define DHT11_RCC        RCC_APB2Periph_GPIOB


#define DHT11_OUT_H GPIO_SetBits(DHT11_GPIO_TYPE, DHT11_GPIO_PIN)
#define DHT11_OUT_L GPIO_ResetBits(DHT11_GPIO_TYPE, DHT11_GPIO_PIN)
#define DHT11_IN    GPIO_ReadInputDataBit(DHT11_GPIO_TYPE, DHT11_GPIO_PIN)

unsigned char dht11step=1,humih=0,humil=0,temh=0,teml=0,chk=0,i=0,j=0,rad=0;
unsigned int ccnt=0;

void mq2_Init() 
{
    GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PE2~4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOE2,3,4,11     // WK_UP-->GPIOA.0
	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);

		EXTI_InitStructure.EXTI_Line = EXTI_Line0 ; //PE2 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;        
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;    
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                  
    EXTI_Init(&EXTI_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
		NVIC_Init(&NVIC_InitStructure); 
}

void moterini()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//PE2~4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOE2,3,4,11     // WK_UP-->GPIOA.0
}

void moter()
{
	static unsigned char i=0;
			delay_us(360000);
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

void moterb()
{
	while(rad>0){
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

void raindrops_Init() 
{
    GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PE2~4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOE2,3,4,11     // WK_UP-->GPIOA.0
	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);

		EXTI_InitStructure.EXTI_Line = EXTI_Line1 ; //PE2 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;        
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;    
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                  
    EXTI_Init(&EXTI_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
		NVIC_Init(&NVIC_InitStructure); 
}

void dht11init()
{	

    GPIO_InitTypeDef GPIO_InitStructure;
//		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//	
//	//外部中断A1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PE2~4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOE2,3,4,11     // WK_UP-->GPIOA.0
	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);

//		EXTI_InitStructure.EXTI_Line = EXTI_Line1 ; //PE2 
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;        
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;    
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                  
//    EXTI_Init(&EXTI_InitStructure);

//		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//		NVIC_Init(&NVIC_InitStructure); 
//		
		//定时器
		TIM_DeInit(TIM2);
		TIM_TimeBaseStructure.TIM_Period=2000;  //??????????
		TIM_TimeBaseStructure.TIM_Prescaler=71;         //??????
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //??????
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		
		TIM_ARRPreloadConfig(TIM2,DISABLE);
		TIM_UpdateRequestConfig(TIM2,1);
		TIM_UpdateDisableConfig(TIM2,DISABLE);
		
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
		
}

void dht11()
{
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		TIM_SetAutoreload(TIM2,20000);
		TIM_Cmd(TIM2,ENABLE);
}

void EXTI0_IRQHandler()//烟雾报警器
{
	static unsigned char i=0;
	EXTI_ClearITPendingBit(EXTI_Line0);
	i=1;
	printf("mq2\r\n");
}

void EXTI1_IRQHandler()//Raindrops
{
	EXTI_ClearITPendingBit(EXTI_Line1);
	printf("rain\r\n");
}

//void EXTI1_IRQHandler()//danexti
//{
//	;
//}

void TIM2_IRQHandler()
{
	TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
//	TIM_Cmd(TIM2,DISABLE);
	if(dht11step==1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
		TIM_SetAutoreload(TIM2,19);
		TIM2->CNT=0;
		
	}
	else if(dht11step==2)
	{
		TIM_Cmd(TIM2,DISABLE);
		TIM2->CNT=0;
		while(GPIOA->IDR&2);
		TIM_SetAutoreload(TIM2,38);
		TIM_Cmd(TIM2,ENABLE);
	}
	else if(dht11step==3)
	{
		while((GPIOA->IDR&2)==0);
		TIM2->CNT=0;
	}
	else if(dht11step==4)
	{
		while(GPIOA->IDR&2);
		TIM2->CNT=0;
		TIM_SetAutoreload(TIM2,10);
	}
	else if(dht11step==5)
	{
		while((GPIOA->IDR&2)==0);
		TIM2->CNT=0;
		TIM_SetAutoreload(TIM2,24);
	}
	else if(dht11step==6)
	{
		TIM_SetAutoreload(TIM2,1000);
		while(GPIOA->IDR&2);
		ccnt=TIM2->CNT;
		if(ccnt>0&&ccnt<50)
		{
			;
		}
		else
			if(j==0)
				humih+=1<<i;
			else if(j==1)
				humil+=1<<i;
			else if(j==2)
				temh+=1<<i;
			else if(j==3)
				teml+=1<<i;
			else if(j==4)
				chk+=1<<i;
		i++;
		dht11step=5;
		if(i>7)
		{
			i=0;
			j++;
			if(j>4)
			{
				dht11step=0;
				TIM2->CNT=0;
				TIM_Cmd(TIM2,DISABLE);
			}
		}
		TIM2->CNT=0;
		TIM_SetAutoreload(TIM2,10);
		TIM_Cmd(TIM2,ENABLE);
	}
	//TIM_SetAutoreload(TIM_TypeDef* TIMx, uint16_t Autoreload)
	// TIM_Cmd(TIM2,ENABLE);
	dht11step++;
}

//_________________________________________________________________________________________
void dht11_gpio_input(void)
{
    GPIO_InitTypeDef g;
    
    RCC_APB2PeriphClockCmd(DHT11_RCC, ENABLE);
    
    g.GPIO_Pin = DHT11_GPIO_PIN;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    g.GPIO_Mode = GPIO_Mode_IN_FLOATING; //????
    
    GPIO_Init(DHT11_GPIO_TYPE, &g);
}

void dht11_gpio_output(void)
{
    GPIO_InitTypeDef g;
    
    RCC_APB2PeriphClockCmd(DHT11_RCC, ENABLE);
    
    g.GPIO_Pin = DHT11_GPIO_PIN;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    g.GPIO_Mode = GPIO_Mode_Out_PP; //????

    GPIO_Init(DHT11_GPIO_TYPE, &g);
}

void dht11_reset(void)
{
    // ??DHT11????
    dht11_gpio_output();
    DHT11_OUT_L;
    delay_us(19000);
    DHT11_OUT_H;
    delay_us(30);
    dht11_gpio_input();
}

unsigned int dht11_scan(void)
{
    return DHT11_IN;
}

unsigned int dht11_read_bit(void)
{
    while (DHT11_IN == RESET);
    delay_us(40);
    if (DHT11_IN == SET)
    {
        while (DHT11_IN == SET);
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int dht11_read_byte(void)
{
    unsigned int i;
    unsigned int data = 0;
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= dht11_read_bit();
    }
    return data;
}

unsigned int dht11_read_data(unsigned char buffer[5])
{
    unsigned int i = 0;unsigned char checksum;
    
    dht11_reset();
    if (dht11_scan() == RESET)
    {
        //???DHT11??
        while (dht11_scan() == RESET);
        while (dht11_scan() == SET);
        for (i = 0; i < 5; i++)
        {
            buffer[i] = dht11_read_byte();
        }
        
        while (dht11_scan() == RESET);
        dht11_gpio_output();
        DHT11_OUT_H;
        
         checksum= buffer[0] + buffer[1] + buffer[2] + buffer[3];
        if (checksum != buffer[4])
        {
            // checksum error
            return 1;
        }
    }
    
    return 0;
}
