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
unsigned char ff[]={0XFD,0X00,0X14,0X01,0x01,0XB8,0XC3,0XCE,0XEF,0XC6,0XB7,0XD4,0XDA,0XCE,0XD2,0XB5,0XC4,0XD7,0XF3,0XC7,0XB0,0XB7,0XBD};
unsigned char rf[]={0xfd,0x00,0x14,0x01,0x01,0xb8,0xc3,0xce,0xef,0xc6,0xb7,0xd4,0xda,0xce,0xd2,0xb5,0xc4,0xd3,0xd2,0xc7,0xb0,0xb7,0xbd};
unsigned char f0[]={0xfd,0x00,0x12,0x01,0x01,0xb8,0xc3,0xce,0xef,0xc6,0xb7,0xd4,0xda,0xce,0xd2,0xb5,0xc4,0xc7,0xb0,0xb7,0xbd};
unsigned char f[]={0xfd,0x00,0x12,0x01,0x01,0xb8,0xc3,0xce,0xef,0xc6,0xb7,0xd4,0xda,0xce,0xd2,0xb5,0xc4,0xd7,0xf3,0xb7,0xbd};
unsigned char r[]={0xfd,0x00,0x12,0x01,0x01,0xb8,0xc3,0xce,0xef,0xc6,0xb7,0xd4,0xda,0xce,0xd2,0xb5,0xc4,0xd3,0xd2,0xb7,0xbd};
unsigned char key[]={0xfd,0x00,0x14,0x01,0x01,0xbc,0xd2,0xd6,0xf7,0xc4,0xfa,0xb5,0xc4,0xd4,0xbf,0xb3,0xd7,0xcd,0xfc,0xb4,0xf8,0xc1,0xcb};
unsigned char add[]={0xfd,0x00,0x0a,0x01,0x01,0xcc,0xed,0xbc,0xd3,0xb3,0xc9,0xb9,0xa6};
unsigned char change[]={0xfd,0x00,0x0a,0x01,0x01,0xd0,0xde,0xb8,0xc4,0xb3,0xc9,0xb9,0xa6};
unsigned char del[]={0xfd,0x00,0x0a,0x01,0x01,0xc9,0xbe,0xb3,0xfd,0xb3,0xc9,0xb9,0xa6};


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
//		moter();
		epcscan();
		num=0;
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
