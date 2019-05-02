#include "stm32f10x.h"
#include<stdio.h>
#include "usart.h"

unsigned char USART3_RX_BUF[3]={0},color[3]={0},ord[3]={0},start=0;
unsigned int     USART_RXD_Cnt = 0;  
unsigned char    USART_RXD_Buf[200];
unsigned char num=0;

/************************************-----usart1-----*****************************/
void USART1_Init(unsigned long baud)   
{    
    NVIC_InitTypeDef NVIC_InitStructure; 
    USART_InitTypeDef USART_InitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

    USART_InitStructure.USART_BaudRate = baud;  
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  
    USART_InitStructure.USART_Parity = USART_Parity_No;  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  


    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  
    USART_Cmd(USART1, ENABLE);

    //??????  
    //Configure the NVIC Preemption Priority Bits     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

    // Enable the USART3 Interrupt   
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);       
      
}

void USART1_IRQHandler(void)
{
    unsigned  char    res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
			  res = USART_ReceiveData(USART1);
				if((USART_RXD_Cnt == 0)&&(res == 0xBB))// frame start must 0xbb
				{
					USART_RXD_Buf[0] = res;
					USART_RXD_Cnt++; 
				}
        else if((USART_RXD_Cnt>0)&&(USART_RXD_Cnt < 200))
        {
            USART_RXD_Buf[USART_RXD_Cnt] = res; 
            USART_RXD_Cnt++; 
        }
    }
}

//void USART_RXD_Data_Process(void)
//{
//	unsigned int    length  ;    //  data length                     
//	if (USART_RXD_Cnt>5) 
//	{
////		printf("a\n");
//		length = USART_RXD_Buf[4];
//		if(USART_RXD_Cnt>=(length+7)&&USART_RXD_Buf[length+6] == 0x7e)//revice frame end
//		{
//			Usart_buff_copy1(USART_RXD_Buf,length+7);
//			USART_RXD_Cnt = 0;
//		}
//	} 
//}

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
/********************************-----usart1_end-----*****************************/

/************************************-----usart2-----*****************************/
void USART2_Init(unsigned long baud)
{  
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
		
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);  
		
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                

}
/********************************-----usart2_end-----*****************************/
//void UART4_Init(unsigned long baud)   
//{    
//    NVIC_InitTypeDef NVIC_InitStructure; 
//    USART_InitTypeDef USART_InitStructure;  
//    GPIO_InitTypeDef GPIO_InitStructure;    //?????????,?????GPIO  

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); //??UART3??GPIOB???  
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  

//    //?????GPIO???  
//    // Configure USART3 Rx (PB.11) as input floating    
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
//    GPIO_Init(GPIOC, &GPIO_InitStructure);  

//    // Configure USART3 Tx (PB.10) as alternate function push-pull  
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
//    GPIO_Init(GPIOC, &GPIO_InitStructure);  

//    //????  
//    USART_InitStructure.USART_BaudRate = baud;  
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;  
//    USART_InitStructure.USART_Parity = USART_Parity_No;  
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  


//    // Configure USART3   
//    USART_Init(UART4, &USART_InitStructure);//????3 
//    // Enable USART3 Receive interrupts ????????  
//    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  
//    // Enable the USART3   
//    USART_Cmd(UART4, ENABLE);//????3  

//    //??????  
//    //Configure the NVIC Preemption Priority Bits     
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

//    // Enable the USART3 Interrupt   
//    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;   
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//    NVIC_Init(&NVIC_InitStructure);       
//      
//}

unsigned long BoundRemanCode(unsigned char BaudCode)
{
	  unsigned long bound[10]={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 128000, 256000};
    return bound[BaudCode];
}

int fputc(unsigned char ch, FILE *f)
{
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
        USART_SendData(USART2,(uint8_t)ch);
    return ch;
}

//void UART4_IRQHandler()                    //??3??????
//{
//	static unsigned char UART4_RX_CNT=0;
//	static unsigned char n=0;
////    u8 Res;
//    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
//    {    
//       // USART3_RX_BUF[UART4_RX_CNT++] = UART4->DR;    //????????     
//				if(n<3)
//				{
//					color[n]=UART4->DR;
//					n++;
//				}
//				else if(n<6)
//				{
//					ord[n-3]=(UART4->DR)-49;
//					n++;
//				}
//				if(n>=6)start=1;
//    }
//    USART_ClearITPendingBit(UART4, USART_IT_RXNE);

//}
