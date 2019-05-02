#include "Mouse_Drive_Init.h"
#include "Mouse_Drive.h"

#include "adc.h"
#include "MPU6050.h"
#include "iic_analog.h"
unsigned int pulse_L_counter;
unsigned int pulse_R_counter;
unsigned int pulse_L;//用于拐弯时记录
unsigned int pulse_R;
unsigned int xxx=0;

/*********************************************************************************************************
  定义全局变量
*********************************************************************************************************/
#define SYS_CLK   72000000
#define ADC1_DR_Address   ((u32)0x40012400+0x4c)
/*********************************************************************************************************
** Function name:       SysTick_Configuration
** Descriptions:        系统节拍定时器初始化。
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        LED IO初始化，初始化PA0和PC3为输出口.并使能这两个口的时钟
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LED_Init(void)
{
	
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PA.0 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.0
 GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PA.0 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED1-->PC.3 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIO0.3
 GPIO_SetBits(GPIOC,GPIO_Pin_3);
	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	    		 //LED1-->PC.3 端口配置, 推挽输出
// GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
// GPIO_SetBits(GPIOC,GPIO_Pin_3); 						 //PC.3 输出高 

}
/*********************************************************************************************************
** Function name:       BUZZ_Init
** Descriptions:        设置蜂鸣器频率为72000000/(19999+1)=3.6KHz,接近“滴”声
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BUZZ_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
   //设置该引脚为复用输出功能,输出TIM3 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = 20000-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频�
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_CtrlPWMOutputs(TIM3,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
 
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
** Descriptions:        红外发射管引脚初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void IR_GPIO_Config(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);//使能PA,PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //左侧红外发射-->PC.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.8

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //左斜红外发射-->PC.7 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //前左红外发射-->PC.6 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	    		 //前右红外发射-->PA.4 端口配置, 推挽输出
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //右斜红外发射-->PA.5 端口配置, 推挽输出
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //右侧红外发射-->PA.7 端口配置, 推挽输出
 GPIO_Init(GPIOA, &GPIO_InitStructure);
        
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);
}


/*********************************************************************************************************
** Function name:       Motor_Mode_Config
** Descriptions:        电机IO初始化  初始化PB5 PB8(左轮)   PC12 PD2(右轮) 为输出口.并使能这四个口的时钟
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MOTOR_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //MOTOR_L_GO-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //MOTOR_L_BACK-->PB.8 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_8); 						 //PC.8 输出高 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //MOTOR_R_GO-->PC.12 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_12); 						 //PC.12 输出高
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //MOTOR_R_BACK-->PD.2 端口配置, 推挽输出
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 输出高		
}
/*********************************************************************************************************
** Function name:       Motor_Mode_Config
** Descriptions:        电机PWM输出初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
//电机PWM范围20~100KHz
//初始化为72000/(2880)=25KHz
void PWM_R_Init(void)//TIM4_CH1
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
   //设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 2880; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
 
 }

 void PWM_L_Init(void)//TIM4_CH2
{  
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
   //设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH1cor
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 2880; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE 主输出使能	

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH2预装载使能	 
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
 
 }


/*********************************************************************************************************
** Function name:       Motor_EXTI_Init
** Descriptions:        电机编码器脉冲计数中断初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Motor_EXTI_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure; 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;       		          //PB9
 	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
 	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	  
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       		          //PC10
 	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
 	 GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIOB.9 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    //GPIOC.10	  中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource10);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			    //使能PB9所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
 
   	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能PC10所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 

}
 void EXTI9_5_IRQHandler(void)
{					 
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)	{
		pulse_L_counter++;
		pulse_L++;
	}
 	 EXTI_ClearITPendingBit(EXTI_Line9);    //清除LINE9上的中断标志位  
	
	xxx++;
	//printf("读数：%d\n",xxx);
}


void EXTI15_10_IRQHandler(void)
{			 
  if(EXTI_GetITStatus(EXTI_Line10) != RESET)	{
		pulse_R_counter++;
	    pulse_R++;
	}
	 EXTI_ClearITPendingBit(EXTI_Line10);  //清除LINE10线路挂起位

	
//	if (xxx>=4000)
//	{
//		Right_motor_stop;
//		Left_motor_stop;
//		printf("读数：%d\n",xxx);
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
