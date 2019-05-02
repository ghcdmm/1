/****************************************Copyright (c)**************************************************
**                           Guangzou cells-net Development Co.,LTD
**                                 http://www.cells-net.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			usart.c
** Last modified Date:  2017-04-15
** Last Version:		V1.00
** Descriptions:		usart1 Config
**------------------------------------------------------------------------------------------------------
** Created by:			ShiHong Hu
** Created date:		2017-04-15
** Version:				V1.00
** Descriptions:		usart1 Config
**
********************************************************************************************************/
#define __USART_MANL
#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h"
#include "RF100_DRV.h"

unsigned char    USART_RXD_Buf[5][USART_REC_LEN];
unsigned char num=0,FLAG=0,R1=0;
unsigned int     USART_RXD_Cnt[5] = {0};  

/**********************************************************************************************************
USART1_INIT：串口1初始化函数
功能：配置并使能串口1
参数：baud:波特率值	
**********************************************************************************************************/
void USART1_INIT(unsigned long baud)
{
    	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
		
		
//#ifdef EN_USART1_RX		

		//  串口NVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			
		/* NVIC configuration */
  /* Enable the USARTx Interrupt */


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
//#endif	
	USART_Cmd(USART1, ENABLE);//使能串口1
		
}

void USART3_Init(unsigned long baud)   
{  
    USART_InitTypeDef USART_InitStructure;  
    NVIC_InitTypeDef NVIC_InitStructure;   
    GPIO_InitTypeDef GPIO_InitStructure;    //?????????,?????GPIO  
    //?????RCC??  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //??UART3??GPIOB???  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  

    //?????GPIO???  
    // Configure USART3 Rx (PB.11) as input floating    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

    // Configure USART3 Tx (PB.10) as alternate function push-pull  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

    //????  
    USART_InitStructure.USART_BaudRate = baud;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  
    USART_InitStructure.USART_Parity = USART_Parity_No;  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  


    // Configure USART3   
    USART_Init(USART3, &USART_InitStructure);//????3 
    // Enable USART3 Receive interrupts ????????  
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
    // Enable the USART3   
    USART_Cmd(USART3, ENABLE);//????3  

    //??????  
    //Configure the NVIC Preemption Priority Bits     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

    // Enable the USART3 Interrupt   
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?????3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //????3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);       
      
} 

void USART2_Initialise(unsigned long bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_AFIO, ENABLE); 
    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = bound;                
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     
    USART_InitStructure.USART_Parity = USART_Parity_No;        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
}

/**********************************************************************************************************
Putchar：串口发送函数
功能：发送一个字符
参数：ch：字符				
**********************************************************************************************************/
unsigned char Putchar(unsigned char ch)
{
		USART_SendData(USART1, (unsigned char) ch);	
		while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		USART_ClearFlag(USART1, USART_FLAG_TXE);
		return ch;
}

/**********************************************************************************************************
PutString：串口发送函数
功能：发送一帧字符串数据
参数：*dat：字符串数组
       len：长度
**********************************************************************************************************/
void PutString(unsigned char *dat, unsigned char len)
{
	unsigned int    i;
	for(i = 0; i < len; i++) {
		  Putchar(*dat++);
	}
}

/**********************************************************************************************************
USART_RXD_Action：串口接收函数
功能：接收一帧数据
参数: time_sta：定时1ms时间标志输入
**********************************************************************************************************/
void USART_RXD_Data_Process(void)
{
	unsigned char i;
	unsigned int    length  ;    //  data length   
for(i=0;i<num;i++){	
	if (USART_RXD_Cnt[i]>5) 
	{
		length = USART_RXD_Buf[i][4];
		if(USART_RXD_Cnt[i]>=(length+7)&&USART_RXD_Buf[i][length+6] == 0x7e)//revice frame end
		{
			Usart_buff_copy(USART_RXD_Buf,length+7,i);
			USART_RXD_Cnt[i] = 0;
		}
	}
} 
}

/**********************************************************************************************************
BoundRemanCode：获取波特率值函数
功能：波特率编码对应波特率值映射函数
参数：BaudCode：波特率对应编码值
**********************************************************************************************************/
unsigned long BoundRemanCode(unsigned char BaudCode)
{
	  unsigned long bound[10]={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 128000, 256000};
    return bound[BaudCode];
}

/**********************************************************************************************************
USART1_IRQHandler：串口接收中断
功能：接收一帧数据
参数：
**********************************************************************************************************/

void USART1_IRQHandler(void)
{
    unsigned  char    res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			  res = USART_ReceiveData(USART1);
				if((USART_RXD_Cnt[num] == 0)&&(res == 0xBB))// frame start must 0xbb
				{
					USART_RXD_Buf[num][0] = res;
					USART_RXD_Cnt[num]++; 
				}
        else if((USART_RXD_Cnt>0)&&(USART_RXD_Cnt[num] < USART_REC_LEN))
        {
            USART_RXD_Buf[num][USART_RXD_Cnt[num]] = res; 
            USART_RXD_Cnt[num]++; 
					if(res==0x7e)num++;
        }
    }
}

void USART2_IRQHandler(void)
{
    unsigned  char    res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			  res = USART_ReceiveData(USART2);
				R1=res;
    }
}

void USART3_IRQHandler(void)
{
    unsigned  char    res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			  res = USART_ReceiveData(USART3);
				switch(res)
				{
					case 0xcb:printf("%c",dbm[l][1]);break;
				}
    }
}

int fputc(int ch, FILE *f)
{
    if (FLAG == 2)
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
        USART_SendData(USART2,(uint8_t)ch);
    }
    else
    {
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);
        USART_SendData(USART3,(uint8_t)ch);
    }

    return ch;
}
