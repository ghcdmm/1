#include "stm32f10x.h"
//#include "stm320518_eval.h"
#include "stm32f10x_rcc.h"
//#include "Dmx_Rx_System\dmx_uart.h"
//#include "Dmx_Rx_System\oled_9616.h"
#include "RF100_Drive\contain.h"
//#include "Dmx_Rx_System\pwm.h"
//#include "Dmx_Rx_System\timo_spi.h"
//#include "Dmx_Rx_System\dmx512.h"
#include <stdio.h>
#include "iic_analog.h"
#include "exti.h"
#include "usart.h"
#include "time.h"
#include "RF100_DRV.h"
#include "delay.h"

extern void oled2864_test(void);



int main(void)
{
	unsigned char q;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */     
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
/*  
	USART_InitStructure.USART_BaudRate = 250000;// 115200
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
*/	
	//DMX_Rx_init();
	
  /* Output a message on Hyperterminal using printf function */
  //printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");

  /* Loop until the end of transmission */
  /* The software must wait until TC=1. The TC flag remains cleared during all data
     transfers and it is set by hardware at the last frame’s end of transmission*/
//  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
//  {}


//unsigned long aaaa=0;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | 
                           RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);
	IIC_GPIO_Configuration( IIC_GOIO_SDA , IIC_SDA , IIC_GPIO_SCL , IIC_SCL );
//	dht11init();
	USART1_INIT(115200);
//	USART2_Initialise(9600);
	USART3_Init(9600);
	epcini();
//	num=0;

//    SystemInit();
//    systick_init();
 moterini();

    delay_init();
		mq2_Init();
		raindrops_Init() ;

	while(1)
	{
		speak1();
//		moter();
//		epcscan();
//		num=0;
//		for(q=0;q<12;q++)
//		{
//		printf("%c ",TAG_READ_DATA.EPC[0][q]);
//		}
//		unsigned char buffer[5];
//        double hum;
//        double temp;
//        if (dht11_read_data(buffer) == 0)
//        {
//            hum = buffer[0] + buffer[1] / 10.0;
//            temp = buffer[2] + buffer[3] / 10.0;
//        }
//			printf("___{\"temperature\": %.2f, \"humidness\": %.2f}___\n", temp, hum);
//        delay_us(2000000);
//        delay_us(2000000);
//		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
//		delay_us(180000);
//		GPIOA->ODR&=0xff7f;
//	GPIOA->ODR|=0x0010;

//		delay_us(180000);
//	GPIOA->ODR&=0xffef;
//	GPIOA->ODR|=0x0040;
//		delay_us(180000);
//	GPIOA->ODR&=0xffbf;
//	GPIOA->ODR|=0x0020;
//	delay_us(180000);
//	GPIOA->ODR&=0xffdf;
//	GPIOA->ODR|=0x0080;
		
		
		//contain_test2();
//		humih=humil=temh=teml=chk=i=j=0;
//		dht11();
//		aaaa=7200000;
//		while(aaaa>0){aaaa--;}//if(aaaa%10==0)printf("%d	%d\r\n",TIM2->CNT,dht11step);
//		printf("%d\r\n",temh);
		
	}
		
//  while (1)
//  {
//		
//		 //printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
//  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
