#include "led.h"
#include "delay.h"
#include "sys.h"
#include "pwm.h"
	
 int main(void)
 {		
	delay_init();	    	     //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	LED_Init();	 
	PWM_A4_Init(900,0);
  while(1)
	{
		LED = !LED;
		delay_ms(500);
	}	 
 }

