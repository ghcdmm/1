/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           Mouse.Drive.h
** Last modified Date:  
** Last Version: 
** Description:         底层驱动程序头文件
** 
**--------------------------------------------------------------------------------------------------------
** Created By:  
** Created date: 
** Version: 
** Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/


#ifndef __Mouse_Drive_h
#define __Mouse_Drive_h


/*********************************************************************************************************
  包含头文件
*********************************************************************************************************/
#include "stm32f10x.h"
#include "Type.h"
#include "Micromouse.h"
#include "Mouse_Config.h"
#include "stmflash.h"
#include "bsp_usart1.h"
#define BLUE_LED_OFF	GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define BLUE_LED_ON		GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define RED_LED_OFF		GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define RED_LED_ON		GPIO_SetBits(GPIOC,GPIO_Pin_3)
void BUZZ_ON(void);
void BUZZ_OFF(void);
#define Left_motor_go      (GPIO_ResetBits(GPIOB,GPIO_Pin_5),GPIO_SetBits(GPIOB,GPIO_Pin_8))//右电机前进cor
#define Left_motor_back    (GPIO_SetBits(GPIOB,GPIO_Pin_5),GPIO_ResetBits(GPIOB,GPIO_Pin_8))//左电机后退
#define Left_motor_stop    (GPIO_SetBits(GPIOB,GPIO_Pin_5),GPIO_SetBits(GPIOB,GPIO_Pin_8)) 	//左电机停止	

#define Right_motor_go     (GPIO_ResetBits(GPIOC,GPIO_Pin_12),GPIO_SetBits(GPIOD,GPIO_Pin_2))//左电机前进
#define Right_motor_back   (GPIO_SetBits(GPIOC,GPIO_Pin_12),GPIO_ResetBits(GPIOD,GPIO_Pin_2))//右电机后退
#define Right_motor_stop   (GPIO_SetBits(GPIOC,GPIO_Pin_12),GPIO_SetBits(GPIOD,GPIO_Pin_2))	//右电机停止
extern unsigned int pulse_L_counter;
extern unsigned int pulse_R_counter;
extern unsigned int pulse_L;
extern unsigned int pulse_R;
extern uint16 Sunlight[6];
extern uint16 ucIRCheck[6];
extern __IO uint16 ADC_ConvertedValue[6];
extern unsigned int aa,bb,cc;
extern float n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11;
extern unsigned int er,el;
extern int xx;
void search(void);

/*********************************************************************************************************
  常量宏定义--传感器
*********************************************************************************************************/
#define __FRONTL              0                                           /*  指向前方传感器左          */
#define __LEFT           	  1                                           /*  指向左方传感器            */
#define __RIGHT          	  2                                           /*  指向右方传感器            */
#define __FRONTR			  3											  /*  指向前方传感器右          */


/*********************************************************************************************************
  常量宏定义--电脑鼠状态
*********************************************************************************************************/
#define __STOP              0                                           /*  电脑鼠停止                  */
#define __GOAHEAD           1                                           /*  电脑鼠前进                  */
#define __TURNLEFT          2                                           /*  电脑鼠向左转                */
#define __TURNRIGHT         3                                           /*  电脑鼠向右转                */
#define __TURNBACK          4                                           /*  电脑鼠向后转                */
#define __GOBACK            5											/*  电脑鼠返回					*/
/*********************************************************************************************************
  常量宏定义--电机加减速度
*********************************************************************************************************/
#define __SPEEDUP         0                                           /*  电机加速                    */
#define __SPEEDDOWN       1                                           /*  电机减速                */

/*********************************************************************************************************
  常量宏定义--电机状态
*********************************************************************************************************/
#define __MOTORSTOP         0                                           /*  电机停止                    */
#define __WAITONESTEP       1                                           /*  电机暂停一步                */
#define __MOTORRUN          2                                           /*  电机运行                    */
#define __MOTORTURNBACK		3
#define __MOTORTURN			4
/*********************************************************************************************************
  常量宏定义--电机运行方向
*********************************************************************************************************/
#define __MOTORGOAHEAD      0                                           /*  电机前进                    */
#define __MOTORGOBACK       1                                           /*  电机后退                    */
#define __MOTORGOSTOP       2                                           /*  电机停止                    */

/*********************************************************************************************************
  常量宏定义--PID
*********************************************************************************************************/
#define __KP 0.50 	  //比例 80
#define __KI 0.3       //积分 0.01
#define __KD 0.02        //微分

#define U_MAX 400       //返回的最大值,是pwm的周期值 
#define U_MIN 0 
#define error_IMAX 4   //积分限幅 
#define Deadband 2 	    //速度PID，设置死区范围

/*********************************************************************************************************
  结构体定义
*********************************************************************************************************/
struct __motor {
    int8    cState;                                                     /*  电机运行状态                */
    int8    cDir;                                                       /*  电机运行方向                */
    uint32  uiPulse;                                                    /*  电机需要运行的脉冲          */
    uint32  uiPulseCtr;                                                 /*  电机已运行的脉冲            */
    int16   sSpeed;                                                     /*  当前占空比                    */
};
typedef struct __motor __MOTOR;

struct __pid       //定义数法核心数据 
{  
    uint16 usFeedBack;  //速度PID，速度反馈值
    
    float sRef;
    int16 sFeedBack;
    float sPreError;  //速度PID，前一次，速度误差,,vi_Ref - vi_FeedBack 
    float sPreDerror; //速度PID，前一次，速度误差之差，d_error-PreDerror; 
  
    fp32 fKp;      //速度PID，Ka = Kp 
    fp32 fKi;      //速度PID，Kb = Kp * ( T / Ti ) 
    fp32 fKd;      //速度PID， 
       
    int16 iPreU;    //电机控制输出值      
};
typedef struct __pid __PID;
/*********************************************************************************************************
  常量宏定义
*********************************************************************************************************/
extern __MOTOR  __GmRight;
extern __MOTOR  __GmLeft;
void mazeSearch(void);                                                  /*  前进N格                     */

void mouseTurnleft(void);                                               /*  向左转90度                  */
void mouseTurnright(void);                                              /*  向右转90度                  */
void mouseTurnback(void);                                               /*  向后转                      */
void onestep(void);														/*前进一格						*/
static void __delay(uint32  uiD);
static void __rightMotorContr(void);									/*左直流电机驱动				*/
static void __leftMotorContr(void);										/*右直流电机驱动				*/
static void __mouseCoorUpdate(void);									/* 根据当前方向更新坐标值		*/
static void __irSend(int8  __cNumber);									/* 红外发射						*/
unsigned char __irCheck(void);													/* 红外接收						*/
void __Encoder(void);
static void __wallCheck(void);                                          /* 墙壁检测                     */
void MouseInit(void);
void mouseGoahead(int8  cNBlock);                                       /*  前进N格                     */
void mouseStop(void); 
extern void mouseInit (void);
void PIDInit(void);

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
