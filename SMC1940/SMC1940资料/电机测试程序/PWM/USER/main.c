#include "led.h"
#include "delay.h"
#include "sys.h"
#include "pwm.h"
	
 int main(void)
 {		
	delay_init();	    	     //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	LED_Init();	 
	PWM_A4_Init(900,0);
  while(1)
	{
		LED = !LED;
		delay_ms(500);
	}	 
 }

