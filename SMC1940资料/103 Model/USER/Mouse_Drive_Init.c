#include "Mouse_Drive_Init.h"
#include "Mouse_Drive.h"

#include "adc.h"
#include "MPU6050.h"
#include "iic_analog.h"
unsigned int pulse_L_counter;
unsigned int pulse_R_counter;
unsigned int pulse_L;//ÓÃÓÚ¹ÕÍäÊ±¼ÇÂ¼
unsigned int pulse_R;
unsigned int xxx=0;

/*********************************************************************************************************
  ¶¨ÒåÈ«¾Ö±äÁ¿
*********************************************************************************************************/
#define SYS_CLK   72000000
#define ADC1_DR_Address   ((u32)0x40012400+0x4c)
/*********************************************************************************************************
** Function name:       SysTick_Configuration
** Descriptions:        ÏµÍ³½ÚÅÄ¶¨Ê±Æ÷³õÊ¼»¯¡£
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void SysTick_Configuration(void)
{ 
    if (SysTick_Config(SYS_CLK/1000))     //1ms
    {        
        while (1);
    }
    NVIC_SetPriority(SysTick_IRQn, 0x0);
}
/*********************************************************************************************************
** Function name:       LED_Init
** Descriptions:        LED IO³õÊ¼»¯£¬³õÊ¼»¯PA0ºÍPC3ÎªÊä³ö¿Ú.²¢Ê¹ÄÜÕâÁ½¸ö¿ÚµÄÊ±ÖÓ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void LED_Init(void)
{
	
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //Ê¹ÄÜPA,PC¶Ë¿ÚÊ±ÖÓ
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PA.0 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOA.0
 GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PA.0 Êä³ö¸ß

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED1-->PC.3 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIO0.3
 GPIO_SetBits(GPIOC,GPIO_Pin_3);
	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	    		 //LED1-->PC.3 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
// GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
// GPIO_SetBits(GPIOC,GPIO_Pin_3); 						 //PC.3 Êä³ö¸ß 

}
/*********************************************************************************************************
** Function name:       BUZZ_Init
** Descriptions:        ÉèÖÃ·äÃùÆ÷ÆµÂÊÎª72000000/(19999+1)=3.6KHz,½Ó½ü¡°µÎ¡±Éù
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void BUZZ_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	                                                                     	
   //ÉèÖÃ¸ÃÒý½ÅÎª¸´ÓÃÊä³ö¹¦ÄÜ,Êä³öTIM3 CH1µÄPWMÂö³å²¨ÐÎ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = 20000-1; //ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ
	TIM_TimeBaseStructure.TIM_Prescaler =0; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = 0; //ÉèÖÃ´ý×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Êä³ö¼«ÐÔ:TIMÊä³ö±È½Ï¼«ÐÔ¸ß
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //¸ù¾ÝTIM_OCInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèTIMx

	TIM_CtrlPWMOutputs(TIM3,ENABLE);	//MOE Ö÷Êä³öÊ¹ÄÜ	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1Ô¤×°ÔØÊ¹ÄÜ	 
	
	TIM_ARRPreloadConfig(TIM3, ENABLE); //Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æÆ÷
	
	TIM_Cmd(TIM3, ENABLE);  //Ê¹ÄÜTIM3
 
}
void BUZZ_ON(void)
{
	TIM_SetCompare1(TIM3,15000);
}
void BUZZ_OFF(void)
{
	TIM_SetCompare1(TIM3,0);
}
/*********************************************************************************************************
** Function name:       IR_GPIO_Config
** Descriptions:        ºìÍâ·¢Éä¹ÜÒý½Å³õÊ¼»¯
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void IR_GPIO_Config(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);//Ê¹ÄÜPA,PC¶Ë¿ÚÊ±ÖÓ
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //×ó²àºìÍâ·¢Éä-->PC.8 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOC.8

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //×óÐ±ºìÍâ·¢Éä-->PC.7 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //Ç°×óºìÍâ·¢Éä-->PC.6 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	    		 //Ç°ÓÒºìÍâ·¢Éä-->PA.4 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //ÓÒÐ±ºìÍâ·¢Éä-->PA.5 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //ÓÒ²àºìÍâ·¢Éä-->PA.7 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOA, &GPIO_InitStructure);
        
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);
}


/*********************************************************************************************************
** Function name:       Motor_Mode_Config
** Descriptions:        µç»úIO³õÊ¼»¯  ³õÊ¼»¯PB5 PB8(×óÂÖ)   PC12 PD2(ÓÒÂÖ) ÎªÊä³ö¿Ú.²¢Ê¹ÄÜÕâËÄ¸ö¿ÚµÄÊ±ÖÓ
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void MOTOR_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //MOTOR_L_GO-->PB.5 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 Êä³ö¸ß

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //MOTOR_L_BACK-->PB.8 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_8); 						 //PC.8 Êä³ö¸ß 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //MOTOR_R_GO-->PC.12 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_12); 						 //PC.12 Êä³ö¸ß
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //MOTOR_R_BACK-->PD.2 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 Êä³ö¸ß		
}
/*********************************************************************************************************
** Function name:       Motor_Mode_Config
** Descriptions:        µç»úPWMÊä³ö³õÊ¼»¯
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
//µç»úPWM·¶Î§20~100KHz
//³õÊ¼»¯Îª72000/(2880)=25KHz
void PWM_R_Init(void)//TIM4_CH1
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	                                                                     	
   //ÉèÖÃ¸ÃÒý½ÅÎª¸´ÓÃÊä³ö¹¦ÄÜ,Êä³öTIM4 CH1µÄPWMÂö³å²¨ÐÎ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 2880; //ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ
	TIM_TimeBaseStructure.TIM_Prescaler =0; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = 0; //ÉèÖÃ´ý×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Êä³ö¼«ÐÔ:TIMÊä³ö±È½Ï¼«ÐÔ¸ß
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //¸ù¾ÝTIM_OCInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèTIMx

	TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE Ö÷Êä³öÊ¹ÄÜ	

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ô¤×°ÔØÊ¹ÄÜ	 
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æÆ÷
	
	TIM_Cmd(TIM4, ENABLE);  //Ê¹ÄÜTIM4
 
 }

 void PWM_L_Init(void)//TIM4_CH2
{  
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	                                                                     	
   //ÉèÖÃ¸ÃÒý½ÅÎª¸´ÓÃÊä³ö¹¦ÄÜ,Êä³öTIM4 CH1µÄPWMÂö³å²¨ÐÎ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH1cor
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 2880; //ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ
	TIM_TimeBaseStructure.TIM_Prescaler =0; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = 0; //ÉèÖÃ´ý×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Êä³ö¼«ÐÔ:TIMÊä³ö±È½Ï¼«ÐÔ¸ß
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //¸ù¾ÝTIM_OCInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèTIMx

	TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE Ö÷Êä³öÊ¹ÄÜ	

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH2Ô¤×°ÔØÊ¹ÄÜ	 
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æÆ÷
	
	TIM_Cmd(TIM4, ENABLE);  //Ê¹ÄÜTIM4
 
 }


/*********************************************************************************************************
** Function name:       Motor_EXTI_Init
** Descriptions:        µç»ú±àÂëÆ÷Âö³å¼ÆÊýÖÐ¶Ï³õÊ¼»¯
** input parameters:    ÎÞ
** output parameters:   ÎÞ
** Returned value:      ÎÞ
*********************************************************************************************************/
void Motor_EXTI_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure; 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//Íâ²¿ÖÐ¶Ï£¬ÐèÒªÊ¹ÄÜAFIOÊ±ÖÓ
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;       		          //PB9
 	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ÉÏÀ­ÊäÈë
 	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	  
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       		          //PC10
 	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ÉÏÀ­ÊäÈë
 	 GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIOB.9 ÖÐ¶ÏÏßÒÔ¼°ÖÐ¶Ï³õÊ¼»¯ÅäÖÃ
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//ÏÂ½µÑØ´¥·¢
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//¸ù¾ÝEXTI_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷

    //GPIOC.10	  ÖÐ¶ÏÏßÒÔ¼°ÖÐ¶Ï³õÊ¼»¯ÅäÖÃ
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource10);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//¸ù¾ÝEXTI_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			    //Ê¹ÄÜPB9ËùÔÚµÄÍâ²¿ÖÐ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//ÇÀÕ¼ÓÅÏÈ¼¶2£¬ 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//×ÓÓÅÏÈ¼¶1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖÐ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure); 
 
   	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//Ê¹ÄÜPC10ËùÔÚµÄÍâ²¿ÖÐ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//ÇÀÕ¼ÓÅÏÈ¼¶2£¬ 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//×ÓÓÅÏÈ¼¶1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖÐ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure); 

}
 void EXTI9_5_IRQHandler(void)
{					 
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)	{
		pulse_L_counter++;
		pulse_L++;
	}
 	 EXTI_ClearITPendingBit(EXTI_Line9);    //Çå³ýLINE9ÉÏµÄÖÐ¶Ï±êÖ¾Î»  
	
	xxx++;
	//printf("¶ÁÊý£º%d\n",xxx);
}


void EXTI15_10_IRQHandler(void)
{			 
  if(EXTI_GetITStatus(EXTI_Line10) != RESET)	{
		pulse_R_counter++;
	    pulse_R++;
	}
	 EXTI_ClearITPendingBit(EXTI_Line10);  //Çå³ýLINE10ÏßÂ·¹ÒÆðÎ»

	
//	if (xxx>=4000)
//	{
//		Right_motor_stop;
//		Left_motor_stop;
//		printf("¶ÁÊý£º%d\n",xxx);
//		
//	}
}
void MouseInit(void)
{
	SysTick_Configuration();
	LED_Init();
	BUZZ_Init();
    IR_GPIO_Config();
	MOTOR_Init();
	PWM_R_Init();
	PWM_L_Init();	
	Motor_EXTI_Init();
	ADC1_Config();
	IIC_GPIO_Configuration( IIC_GOIO_SDA , IIC_SDA , IIC_GPIO_SCL , IIC_SCL );
	MPU6050_Inital();
}
