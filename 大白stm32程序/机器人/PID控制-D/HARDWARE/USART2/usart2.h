#ifndef __USRAT3_H
#define __USRAT3_H 
#include "sys.h"	  	
extern u8 Usart3_Receive;
extern uint8_t kp3;
extern uint8_t ki3;
extern uint8_t kd3;
extern int desired_speed[2];  
extern float Yaw,Velocity_Left,Velocity_Right;

void uart2_init(u32 pclk2,u32 bound);
void USART2_IRQHandler(void);
void USART2send(void);
	
#endif

